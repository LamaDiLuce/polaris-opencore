#!/bin/bash
version=1.9.17_20200930

clear
echo ""
echo "--------------------------------------------"
echo " OpenCore Saber Updater - v.$version"
echo "--------------------------------------------"
echo "$(dirname "$BASH_SOURCE")"
cd "$(dirname "$BASH_SOURCE")"
echo ""
echo "--------------------------------------------"
echo ""
if [ $# -eq 0 ]
then
echo "Uploading OpenCore.$version.hex"
echo "--------------------------------------------"
echo ""
./tycmd upload OpenCore.$version.hex

else
echo Uploading $1
echo "--------------------------------------------"
echo ""
./tycmd upload $1

fi

echo ""
echo "-------------------------------"
echo "Press  cmd-W  to close Terminal"
echo "-------------------------------"
echo ""    
exit 0
