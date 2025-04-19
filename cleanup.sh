#!/bin/bash

# Cleanup script for orderbook engine

echo "Cleaning up repository..."

# Remove generated binaries
rm -f orderbook
rm -f orderbook_test
rm -f a.out
rm -f *.o
rm -f *.d
rm -f *.a

# Remove test artifacts
rm -f mini_orderbook
rm -f mini_orderbook_cancel
rm -f minimal_orderbook
rm -f simple_orderbook
rm -f orderbook_debug
rm -f orderbook_fixed

# Remove unnecessary test source files
rm -f orderbook_bugfix.cpp
rm -f mini_orderbook.cpp
rm -f mini_orderbook_cancel.cpp
rm -f minimal_orderbook.cpp
rm -f simple_orderbook.cpp

# Remove debug directories
rm -rf *.dSYM/
rm -rf build/

echo "Cleanup complete!" 