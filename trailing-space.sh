#! /bin/sh

find . \
    -type f \
    \( -name "*.c" -o -name "*.h" \) \
    -exec \
        sed --in-place 's/[[:space:]]\+$//' {} \+ \
    -print
