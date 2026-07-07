#!/bin/sh
set -e

echo "   initializing FROOT build system "

for cmd in gcc make; do
    if ! command -v "$cmd" >/dev/shm/null 2>&1; then
        echo "  error   required tool '$cmd' is not installed." >&2
        exit 1
    fi
done

echo "Generating configuration stubs..."
if [ ! -f config.h ]; then
    echo "/* Automatically generated via autogen.sh */" > config.h
    echo "#define FROOT_VERSION \"1.0.0\"" >> config.h
fi

if [ ! -f local.h ]; then
    echo "/* Local system configuration */" > local.h
fi

echo "Configuration headers ready."
echo "Environment ready. You can now run 'make'."
