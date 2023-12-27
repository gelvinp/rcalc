#!/bin/zsh

SCRIPT_DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

cd "$SCRIPT_DIR/.." # Root level directory
cd bin

codesign -s "$1" -f --strict=all --options runtime RCalc.macos.release.arm64.app
codesign -s "$1" -f --strict=all --options runtime RCalc.macos.release.x86_64.app

ditto -c -k --keepParent RCalc.macos.release.arm64.app RCalc.macos.release.arm64.zip
ditto -c -k --keepParent RCalc.macos.release.x86_64.app RCalc.macos.release.x86_64.zip

# TODO: Send one off without wait, somehow capture submission ID from the output, wait on the second, and then wait on the first one
xcrun notarytool submit RCalc.macos.release.arm64.zip --keychain-profile "AppStoreConnectAPI" --wait
xcrun notarytool submit RCalc.macos.release.x86_64.zip --keychain-profile "AppStoreConnectAPI" --wait

rm RCalc.macos.release.arm64.zip
rm RCalc.macos.release.x86_64.zip

xcrun stapler staple RCalc.macos.release.arm64.app
xcrun stapler staple RCalc.macos.release.x86_64.app

zip -q -r -9 RCalc_MacOS_arm64.zip RCalc.macos.release.arm64.app
zip -q -r -9 RCalc_MacOS_x86_64.zip RCalc.macos.release.x86_64.app