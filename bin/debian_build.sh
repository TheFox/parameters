#!/usr/bin/env bash

# This script build the repository for Debian.

SCRIPT_BASEDIR=$(dirname "$0")

cd "${SCRIPT_BASEDIR}/.."

if [[ ! -d debian/ ]]; then
    dh_make --native --single --packagename parameters_1.0.0
fi

dpkg-buildpackage -b -j 2
dpkg-buildpackage -rfakeroot -Tclean
