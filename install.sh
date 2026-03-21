#!/bin/bash
echo "Building ltfm..."
gcc -O2 -o ltfm main.c
echo "Installing to /usr/local/bin..."
sudo mv ltfm /usr/local/bin/ltfm
echo "Done! Type 'ltfm' to run."