#!/bin/bash
#Script to build buildroot configuration
#Author: Chris R
echo "CLEANING!"

cd `dirname $0`
cd buildroot
make distclean
cd `dirname $0`
