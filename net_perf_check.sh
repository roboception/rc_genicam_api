#!/bin/bash

#
# Simple test of network settings for receiving lots of UDP packets (e.g. for GigE Vision)
# Copyright 2017 Felix Ruess <felix.ruess@roboception.de>
#

# SOFTIRQ check based on xsos <http://github.com/ryran/xsos>
# Copyright 2012-2016 Ryan Sawhill Aroha <rsaw@redhat.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#    General Public License <gnu.org/licenses/gpl.html> for more details.
#-------------------------------------------------------------------------------

export LC_ALL=en_US.UTF-8

# Colors and colors and colors oh my (but only for bash v4)
if [[ $BASH_VERSINFO -ge 4 ]]; then
  declare -A c
  c[reset]='\033[0;0m'   c[BOLD]='\033[0;0m\033[1;1m'
  c[dgrey]='\033[0;30m'  c[DGREY]='\033[1;30m'  c[bg_DGREY]='\033[40m'
  c[red]='\033[0;31m'    c[RED]='\033[1;31m'    c[bg_RED]='\033[41m'
  c[green]='\033[0;32m'  c[GREEN]='\033[1;32m'  c[bg_GREEN]='\033[42m'
  c[orange]='\033[0;33m' c[ORANGE]='\033[1;33m' c[bg_ORANGE]='\033[43m'
  c[blue]='\033[0;34m'   c[BLUE]='\033[1;34m'   c[bg_BLUE]='\033[44m'
  c[purple]='\033[0;35m' c[PURPLE]='\033[1;35m' c[bg_PURPLE]='\033[45m'
  c[cyan]='\033[0;36m'   c[CYAN]='\033[1;36m'   c[bg_CYAN]='\033[46m'
  c[lgrey]='\033[0;37m'  c[LGREY]='\033[1;37m'  c[bg_LGREY]='\033[47m'
fi

# ==============================================================================
# ENVIRONMENT VARIABLES -- Modify these by setting them in your shell
# environment, e.g. ~/.bash_profile or /etc/profile.d/xsos.sh

# COLORS
# The following configure defaults for various colors to enhance output

#   XSOS_COLORS (bool: y/n)
#     Controls whether color is enabled or disabled by default
#     Can also be controlled by cmdline arg
      : ${XSOS_COLORS:="y"}

#   XSOS_COLOR_RESET -- color to reset terminal to after using other colors
      : ${XSOS_COLOR_RESET:="reset"}

#   XSOS_COLOR_H1 -- color for content modules' primary header
      : ${XSOS_COLOR_H1:="RED"}

#   XSOS_COLOR_H2 -- color for content modules' secondary header
      : ${XSOS_COLOR_H2:="PURPLE"}

#   XSOS_COLOR_H3 -- color for content modules' tertiary header
      : ${XSOS_COLOR_H3:="BLUE"}

#   XSOS_COLOR_H4 -- color used only for SYSCTL() module
      : ${XSOS_COLOR_H4:="reset"}

#   XSOS_COLOR_IMPORTANT -- color for drawing attention to important data
      : ${XSOS_COLOR_IMPORTANT:="BOLD"}

#   XSOS_COLOR_WARN1 -- color for level-1 warnings
      : ${XSOS_COLOR_WARN1:="orange"}

#   XSOS_COLOR_WARN2 -- color for level-2 warnings
      : ${XSOS_COLOR_WARN2:="ORANGE"}

# INDENTATION
#   The following variables are not used universally and that might not change

#   XSOS_INDENT_H1 -- 1st level of indentation
      : ${XSOS_INDENT_H1:="  "}

#   XSOS_INDENT_H2 -- 2nd level of indentation
      : ${XSOS_INDENT_H2:="    "}

#   XSOS_INDENT_H3 -- 3rd level of indentation
      : ${XSOS_INDENT_H3:="      "}


# XSOS_HEADING_SEPARATOR (str)
#   Acts as a separator between content modules
#   Should include at least 1 trailing new-line
    : ${XSOS_HEADING_SEPARATOR:="\n"}

# XSOS_OUTPUT_HANDLER (str: application name)
#   Sets name of application to handle output
    : ${XSOS_OUTPUT_HANDLER:="cat"}


HELP_USAGE() {
  echo "Usage: net_perf_check.sh [-xy] [-i interface]
  or:  net_perf_check.sh [-?|-h|--help]"
}

HELP_OPTS() {
  echo "
Options:"
  echo "
 -i, --iface❚select interface for MTU check
 -x, --nocolor❚disable output colorization
 -y, --less❚send output to \`less -SR\`" | column -ts❚
}

HELP() {
  HELP_USAGE
  HELP_OPTS
  exit
}

# Help? Version?
case $1 in
  -\?|-h)               HELP ;;
  --help|help)          HELP ;;
esac

# GNU getopt short and long options:
sopts='xyi:'
lopts='nocolor,less,iface'

# Check for bad switches
getopt -Q --name=xsos -o $sopts -l $lopts -- "$@" || { HELP_USAGE; exit 64; }

# Parse command-line arguments
PARSE() {
  until [[ $1 == -- ]]; do
    case $1 in
      -x|--nocolor) XSOS_COLORS=n     ;;
      -y|--less)    XSOS_OUTPUT_HANDLER='less -SR' ;;
      -i|--iface)   IFACE=$2; shift ;;
    esac
    shift
  done
  shift #(to get rid of the '--')
}

# Call the parser
PARSE $(getopt -u --name=xsos -o $sopts -l $lopts -- "$@")

# If color should be enabled, taste the rainbow
if [[ $XSOS_COLORS == y && $BASH_VERSINFO -ge 4 ]]; then
  c[0]=${c[$XSOS_COLOR_RESET]}
  c[H1]=${c[$XSOS_COLOR_H1]}
  c[H2]=${c[$XSOS_COLOR_H2]}
  c[H3]=${c[$XSOS_COLOR_H3]}
  c[H4]=${c[$XSOS_COLOR_H4]}
  c[Imp]=${c[$XSOS_COLOR_IMPORTANT]}
  c[Warn1]=${c[$XSOS_COLOR_WARN1]}
  c[Warn2]=${c[$XSOS_COLOR_WARN2]}
else
  unset c
fi


MTU() {
  local mtu= iface=
  if [[ $# == 1 ]]; then
    iface=$1
  else
    #echo -e "Detecting default interface..."
    iface=$(route | grep '^default' | grep -o '[^ ]*$')
  fi
  if [[ ! -e /sys/class/net/$iface ]]; then
    echo -e "${c[Warn2]}Warning:${c[Warn1]} Interface $iface does not exist; skipping MTU check${c[0]}" >&2
    echo -en $XSOS_HEADING_SEPARATOR >&2
    return
  fi
  mtu=$(cat /sys/class/net/$iface/mtu)

  echo -e "${c[H1]}MTU${c[0]}"
  if [[ $mtu -lt 8000 ]]; then
    echo -e "${XSOS_INDENT_H1}${c[Warn1]}Current MTU of $iface is only $mtu, consider setting an MTU up to 9000.${c[0]}"
  else
    echo -e "${XSOS_INDENT_H1}MTU of $iface is sufficient${c[0]} (Current value: $mtu)"
  fi
  echo -en $XSOS_HEADING_SEPARATOR
}

RMEM_MAX () {
  local rcvbuf_errors= rmem_max=

  if [[ ! -r /proc/net/snmp ]]; then
    echo -e "${c[Warn2]}Warning:${c[Warn1]} '/proc/net/snmp' unreadable; skipping receive buffer check${c[0]}" >&2
    echo -en $XSOS_HEADING_SEPARATOR >&2
    return
  fi
  rcvbuf_errors=$(cat /proc/net/snmp | grep -E "Udp\:[[:space:]][0-9]+" | awk '{print $6}')
  rmem_max=$(cat /proc/sys/net/core/rmem_max 2>/dev/null) \
      && rmem_max=" (Current value: net.core.rmem_max = $rmem_max)" \
      || rmem_max=" (However, proc/sys/net/core/rmem_max is missing)"

  echo -e "${c[H1]}RMEM_MAX${c[0]}"
  if [[ $rcvbuf_errors -gt 0 ]]; then
    echo -e "${XSOS_INDENT_H1}${c[Warn1]}There were $rcvbuf_errors RcvbufErrors, consider reviewing rmem_max tunable.${c[0]}$rmem_max"
    echo -e "${XSOS_INDENT_H1}This is only a problem if RcvbufErrors value increases while receiving data."
  else
    echo -e "${XSOS_INDENT_H1}Max receive buffer seems to be sufficient${c[0]}$rmem_max"
  fi
  echo -en $XSOS_HEADING_SEPARATOR
}

SOFTIRQ() {
  # Local vars:
  local softirq_input_file suffix= backlog= budget=

  softirq_input_file=/proc/net/softnet_stat
  if [[ ! -r $softirq_input_file ]]; then
      echo -e "${c[Warn2]}Warning:${c[Warn1]} '/proc/net/softnet_stat' unreadable; skipping softirq check${c[0]}" >&2
      echo -en $XSOS_HEADING_SEPARATOR >&2
      return
  fi
  backlog=$(cat /proc/sys/net/core/netdev_max_backlog 2>/dev/null) \
      && backlog=" (Current value: net.core.netdev_max_backlog = $backlog)" \
      || backlog=" (However, proc/sys/net/core/netdev_max_backlog is missing)"
  budget=$(cat /proc/sys/net/core/netdev_budget 2>/dev/null) \
      && budget=" (Current value: net.core.netdev_budget = $budget)" \
      || budget=" (However, proc/sys/net/core/netdev_budget is missing)"

  echo -e "${c[H1]}SOFTIRQ${c[0]}"

  gawk '{if (strtonum("0x" $2) > 0) exit 177}' "$softirq_input_file"

  if [[ $? -eq 177 ]]; then
    echo -e "${XSOS_INDENT_H1}${c[Warn1]}Backlog max has been reached, consider reviewing backlog tunable.${c[0]}$backlog"
  else
    echo -e "${XSOS_INDENT_H1}Backlog max is sufficient${c[0]}$backlog"
  fi


  gawk '{if (strtonum("0x" $3) > 0) exit 177}' "$softirq_input_file"
  if [[ $? -eq 177 ]]; then
    echo -e "${XSOS_INDENT_H1}${c[Warn1]}Budget is not sufficient, consider reviewing budget tunable.${c[0]}$budget"
  else
    echo -e "${XSOS_INDENT_H1}Budget is sufficient${c[0]}$budget"
  fi
  echo -en $XSOS_HEADING_SEPARATOR
}


# Create sub tempdir in /dev/shm (tons of bash constructs use TMPDIR)
[[ -d /dev/shm && -w /dev/shm ]] && parent=/dev/shm || parent=/tmp
export TMPDIR=$(mktemp -d -p $parent)
# Create tmp file for capturing stderr
stderr_file=$TMPDIR/stderr
# Remove temp dir when we're done
trap "rm -rf $TMPDIR 2>/dev/null" EXIT

{
  # Redirect stderr to temp file
  exec 7>&2 2>$stderr_file

  [[ -n $IFACE ]] && MTU $IFACE || MTU
  RMEM_MAX
  SOFTIRQ

  # If sending output to less or more, let's just append stderr to stdout
  # If just outputting to term (cat), redirect fd2 to tty
  case $XSOS_OUTPUT_HANDLER in
    less*|more)  cat $stderr_file ;;
    cat)         exec 2>&7
  esac

} | $XSOS_OUTPUT_HANDLER

# If output going to term (cat), print stderr tmp file contents to stderr
[[ $XSOS_OUTPUT_HANDLER == cat ]] && cat $stderr_file >&2 || :
