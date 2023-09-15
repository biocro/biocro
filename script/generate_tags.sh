#!/usr/bin/env sh

# This is a simple script for Emacs users that will generate a tags
# table for all C++ src files under the "src" directory.

pushd ../src &&
    rm -f TAGS &&
    find . \( -name "*.cpp" -o -name "*.h" \) -exec etags --language=c++ --append {} \; &&
    popd


