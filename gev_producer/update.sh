#!/bin/bash
#
# Takes over the prebuilt GenTL producer rc_gev_producer from its
# redistributable bundle, which the CI of rc_gev_producer creates for every
# release.
#
# Usage: gev_producer/update.sh <version>|<bundle.tar.gz>
#
# With a version, the bundle is downloaded from the generic package registry of
# rc_gev_producer, which needs a GitLab access token with the scope read_api in
# GITLAB_TOKEN. With a file, that bundle is used directly.
#
# All files of this directory, except CMakeLists.txt and this script, are
# replaced by the contents of the bundle. The parts between the markers
# "BEGIN rc_gev_producer" and "END rc_gev_producer" in sbom-components.yaml and
# copyright_extra are replaced by the snippets of the bundle. The result is
# left uncommitted for review.

set -e

GITLAB_URL=${GITLAB_URL:-https://gitlab.roboception.de}
PROJECT=${RC_GEV_PRODUCER_PROJECT:-rc%2Frc_gev_producer}

if [ $# -ne 1 ]; then
  echo "Usage: $0 <version>|<bundle.tar.gz>" >&2
  exit 1
fi

dir=$(realpath "$(dirname "$0")")
top=$(realpath "$dir/..")

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

if [ -f "$1" ]; then
  bundle=$(realpath "$1")
else
  version=${1#v}
  name=rc_gev_producer-$version-redist.tar.gz

  if [ -z "$GITLAB_TOKEN" ]; then
    echo "GITLAB_TOKEN must be set for downloading the bundle" >&2
    exit 1
  fi

  bundle="$tmp/$name"
  curl --fail --silent --show-error --location --header "PRIVATE-TOKEN: $GITLAB_TOKEN" \
    --output "$bundle" \
    "$GITLAB_URL/api/v4/projects/$PROJECT/packages/generic/rc_gev_producer/$version/$name"
fi

mkdir "$tmp/x"
tar -xzf "$bundle" -C "$tmp/x"
src="$tmp/x/rc_gev_producer"

(cd "$src" && sha256sum --quiet -c SHA256SUMS)

for p in linux/x86_64 linux/aarch64 windows/x64; do
  if [ ! -f "$src/$p/rc_gev_producer.cti" ]; then
    echo "The bundle does not contain the producer for $p" >&2
    exit 1
  fi
done

# Replaces the marked block in a file by the snippet, which contains the
# markers itself.

replace_block() {
  file=$1
  snippet=$2

  if ! grep -q "BEGIN rc_gev_producer" "$file" || ! grep -q "END rc_gev_producer" "$file"; then
    echo "$file has no markers for rc_gev_producer" >&2
    exit 1
  fi

  awk -v snippet="$snippet" '
    /BEGIN rc_gev_producer/ { while ((getline line < snippet) > 0) print line; skip=1; next }
    /END rc_gev_producer/ { skip=0; next }
    !skip { print }
  ' "$file" > "$tmp/replaced"

  cat "$tmp/replaced" > "$file"
}

replace_block "$top/sbom-components.yaml" "$src/sbom-components.yaml"
replace_block "$top/copyright_extra" "$src/copyright_extra"

find "$dir" -mindepth 1 -maxdepth 1 ! -name CMakeLists.txt ! -name update.sh -exec rm -rf {} +
cp -r "$src"/. "$dir/"

echo "Updated to rc_gev_producer $(cat "$dir/VERSION")"
