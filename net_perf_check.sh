#!/bin/bash
#
# This file is part of the rc_genicam_api package.
#
# Copyright (c) 2017-2026 Roboception GmbH
# All rights reserved
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Checks the Linux network settings for receiving many UDP packets, e.g. for
# streaming images via GigE Vision. Counters of dropped packets are only
# meaningful while or after streaming. The exit status is 1 if there were any
# warnings.
#
# Only /proc and /sys are used, since tools like ip or ethtool are not always
# installed. ethtool is used if available.

# recommended values, see README.md

MIN_MTU=8000
REC_MTU=9000
MIN_SPEED=1000
REC_RMEM_MAX=33554432
REC_BACKLOG=2000
REC_BUDGET=600

usage() {
  echo "Usage: net_perf_check.sh [-n] [-i <interface>]"
  echo
  echo "Checks the Linux network settings for receiving many UDP packets, e.g."
  echo "for GigE Vision. It should be run while or after streaming images."
  echo
  echo "Options:"
  echo "  -i, --iface <interface>  Check only this interface. Default is all"
  echo "                           physical interfaces that are up."
  echo "  -n, --no-color           Disable colored output."
  echo "  -h, --help               Show this help."
}

ifaces=()
color=1
[[ -t 1 ]] || color=0

while [[ $# -gt 0 ]]; do
  case $1 in
    -i|--iface)
      [[ $# -ge 2 ]] || { usage >&2; exit 2; }
      ifaces+=("$2"); shift ;;
    -n|--no-color|-x|--nocolor)
      color=0 ;;
    -h|--help|-\?)
      usage; exit 0 ;;
    *)
      usage >&2; exit 2 ;;
  esac
  shift
done

if [[ $color -eq 1 ]]; then
  C_HEAD='\033[1m' C_OK='\033[32m' C_WARN='\033[33m' C_RESET='\033[0m'
else
  C_HEAD='' C_OK='' C_WARN='' C_RESET=''
fi

warnings=0

heading() { echo -e "\n${C_HEAD}$*${C_RESET}"; }
ok() { echo -e "  ${C_OK}OK${C_RESET}    $*"; }
info() { echo -e "  INFO  $*"; }
hint() { echo -e "        $*"; }
warn() { echo -e "  ${C_WARN}WARN${C_RESET}  $*"; warnings=$((warnings+1)); }

read_value() {
  local v
  v=$(cat "$1" 2>/dev/null) || return 1
  echo "$v"
}

# --- interfaces ---

# physical interfaces have a device link, virtual ones like lo, docker0 or
# bridges do not

physical_ifaces() {
  local d
  for d in /sys/class/net/*; do
    [[ -e $d/device && $(read_value "$d/operstate") == up ]] && basename "$d"
  done
}

check_iface() {
  local iface=$1 d=/sys/class/net/$1 v name max cur rp

  heading "Interface $iface"

  if [[ ! -d $d ]]; then
    warn "Interface does not exist"
    return
  fi

  # link speed in Mbit/s, which is -1 or unreadable if there is no link

  v=$(read_value "$d/speed")
  if [[ -z $v || $v -le 0 ]]; then
    warn "No link or unknown link speed"
  elif [[ $v -lt $MIN_SPEED ]]; then
    warn "Link speed is only $v Mbit/s, GigE Vision requires at least $MIN_SPEED Mbit/s"
  else
    ok "Link speed is $v Mbit/s"
  fi

  # jumbo frames reduce the number of packets considerably

  v=$(read_value "$d/mtu")
  if [[ -n $v && $v -lt $MIN_MTU ]]; then
    warn "MTU is only $v, jumbo frames are strongly recommended"
    hint "sudo ip link set $iface mtu $REC_MTU   (all switches must support it)"
  elif [[ -n $v ]]; then
    ok "MTU is $v"
  fi

  # packets that were dropped by the network card or its driver

  for name in rx_dropped rx_missed_errors rx_fifo_errors rx_over_errors; do
    v=$(read_value "$d/statistics/$name")
    if [[ -n $v && $v -gt 0 ]]; then
      warn "$v packets counted as $name"
      hint "This is only a problem if the value increases while streaming. Increasing the"
      hint "receive ring buffer of the network card usually helps (see below)."
    fi
  done

  # receive ring buffer of the network card

  if command -v ethtool >/dev/null 2>&1; then
    v=$(ethtool -g "$iface" 2>/dev/null)
    max=$(echo "$v" | awk '/^Pre-set maximums/{m=1} /^Current hardware/{m=0} m && /^RX:/{print $2; exit}')
    cur=$(echo "$v" | awk '/^Current hardware/{m=1} m && /^RX:/{print $2; exit}')
    if [[ $max =~ ^[0-9]+$ && $cur =~ ^[0-9]+$ ]]; then
      if [[ $cur -lt $max ]]; then
        warn "Receive ring buffer has $cur of max. $max entries"
        hint "sudo ethtool -G $iface rx $max"
      else
        ok "Receive ring buffer has the maximum of $cur entries"
      fi
    fi
  else
    info "ethtool is not installed, receive ring buffer is not checked"
  fi

  # Strict reverse path filtering drops packets of devices whose IP address
  # is not in the subnet of the interface, e.g. while a device is still
  # without proper IP configuration. This affects the discovery of devices.

  rp=$(read_value "/proc/sys/net/ipv4/conf/$iface/rp_filter")
  v=$(read_value "/proc/sys/net/ipv4/conf/all/rp_filter")
  if [[ -n $rp && -n $v && $v -gt $rp ]]; then
    rp=$v
  fi

  if [[ $rp == 1 ]]; then
    info "Strict reverse path filtering (rp_filter=1) may hide devices with an IP address"
    hint "outside the subnet of $iface. If needed, use loose mode:"
    hint "sudo sysctl -w net.ipv4.conf.$iface.rp_filter=2"
  fi
}

# --- socket receive buffer ---

check_rmem() {
  local max errors header values i

  heading "Socket receive buffer"

  max=$(read_value /proc/sys/net/core/rmem_max)
  if [[ -z $max ]]; then
    warn "/proc/sys/net/core/rmem_max is not readable"
  elif [[ $max -lt $REC_RMEM_MAX ]]; then
    warn "net.core.rmem_max is only $max"
    hint "sudo sysctl -w net.core.rmem_max=$REC_RMEM_MAX"
  else
    ok "net.core.rmem_max is $max"
  fi

  # the column of RcvbufErrors is taken from the header line, since it
  # differs between kernel versions

  if [[ -r /proc/net/snmp ]]; then
    read -ra header < <(grep -m1 '^Udp:' /proc/net/snmp)
    read -ra values < <(grep '^Udp:' /proc/net/snmp | sed -n 2p)
    for i in "${!header[@]}"; do
      [[ ${header[$i]} == RcvbufErrors ]] && errors=${values[$i]}
    done
  fi

  if [[ -z $errors ]]; then
    info "UDP receive buffer errors cannot be read from /proc/net/snmp"
  elif [[ $errors -gt 0 ]]; then
    warn "$errors UDP packets were dropped since the socket receive buffer was full"
    hint "This is only a problem if the value increases while streaming."
  else
    ok "No UDP packets were dropped due to a full socket receive buffer"
  fi
}

# --- packet processing of the kernel ---

check_softnet() {
  local fields dropped=0 squeezed=0 backlog budget

  heading "Kernel packet processing"

  if [[ ! -r /proc/net/softnet_stat ]]; then
    info "/proc/net/softnet_stat is not readable"
    return
  fi

  # one line per CPU with hexadecimal counters, the 2nd column counts packets
  # that were dropped since the backlog queue was full, the 3rd column counts
  # how often packet processing stopped since the budget was exhausted

  while read -ra fields; do
    dropped=$((dropped + 16#${fields[1]}))
    squeezed=$((squeezed + 16#${fields[2]}))
  done < /proc/net/softnet_stat

  backlog=$(read_value /proc/sys/net/core/netdev_max_backlog) || backlog=unknown
  budget=$(read_value /proc/sys/net/core/netdev_budget) || budget=unknown

  if [[ $dropped -gt 0 ]]; then
    warn "$dropped packets were dropped since the backlog queue was full (net.core.netdev_max_backlog is $backlog)"
    hint "sudo sysctl -w net.core.netdev_max_backlog=$REC_BACKLOG"
  else
    ok "No packets were dropped due to a full backlog queue (net.core.netdev_max_backlog is $backlog)"
  fi

  if [[ $squeezed -gt 0 ]]; then
    warn "Packet processing was interrupted $squeezed times due to the budget (net.core.netdev_budget is $budget)"
    hint "This is only a problem if the value increases while streaming."
    hint "sudo sysctl -w net.core.netdev_budget=$REC_BUDGET"
  else
    ok "Packet processing was never interrupted due to the budget (net.core.netdev_budget is $budget)"
  fi
}

# --- main ---

if [[ ${#ifaces[@]} -eq 0 ]]; then
  mapfile -t ifaces < <(physical_ifaces)
  if [[ ${#ifaces[@]} -eq 0 ]]; then
    heading "Interfaces"
    warn "No physical network interface is up"
  fi
fi

for iface in "${ifaces[@]}"; do
  check_iface "$iface"
done

check_rmem
check_softnet

echo
if [[ $warnings -gt 0 ]]; then
  echo "$warnings warning(s), see README.md of rc_genicam_api for details"
  exit 1
fi

echo "No problems found"
