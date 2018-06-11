#1/bin/sh
# I have no idea how to set up tests properly for Autotools.
cd ../..
build/autotools/unittest ~[!nonportable] -r automake
cd -
