#!/bin/sh
# Script Settings
version=1.9.17_20200930

appname=OpenCoreFlasher
dmgfolder=OpenCoreFlasher-dmg

# Create DMG
cd -- "$(dirname "$0")"

PkgInfoContents="OpenCoreFlasher#"

rm -dfr $dmgfolder

echo "Creating $dmgfolder..."
mkdir -p $dmgfolder
chmod +x files/tycmd
chmod +x files/*.command

cp files/upload-macos.command $dmgfolder/
cp files/tycmd $dmgfolder/
cp files/OpenCore.$version.hex $dmgfolder/
chmod +x $dmgfolder/tycmd
chmod +x $dmgfolder/*.command

rm -f *.dmg
hdiutil create tmp.dmg -ov -volname "OpenCoreUpdater $version" -fs HFS+ -srcfolder "./$dmgfolder/" 
hdiutil convert tmp.dmg -format UDZO -o OpenCoreUpdater-macos-$version.dmg
rm -dfr $dmgfolder
rm -f tmp.dmg
