#!/usr/bin/env sh

pushd ../src &&
    rm -f TAGS &&
    find . \( -name "*.cpp" -o -name "*.h" \) -exec etags --language=c++ --append {} \; &&
    popd


