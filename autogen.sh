#! /bin/sh

for subdir in .
do
  ( cd $subdir
    echo "Generating in `pwd`..."
    if [ -d config/m4 ]
    then
        echo "Found local macros directory..."
        ACLOCAL_FLAGS="$ACLOCAL_FLAGS -I config/m4"
    fi
    echo "Running aclocal $ACLOCAL_FLAGS..."
    aclocal $ACLOCAL_FLAGS
    echo "Running autoheader..."
    autoheader
    echo "Running autoconf..."
    autoconf )
done
# arch-tag: dfae6604-8b7a-4020-9225-ca440b9e9cff
