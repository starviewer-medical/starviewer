#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

export DYLD_LIBRARY_PATH="$LD_LIBRARY_PATH:$STARVIEWER_BUILD_DIR_BASE/bin"
#export DYLD_FRAMEWORK_PATH="$LD_LIBRARY_PATH"

if [[ -n "$APP_BUNDLE_DESTINATION_DIR" ]]
then
    # Copy the app bundle to another directory with a possible different name
    mkdir -p "$APP_BUNDLE_DESTINATION_DIR"
    cp -r "$STARVIEWER_BUILD_DIR_BASE/bin/starviewer.app" "$APP_BUNDLE_DESTINATION_DIR/$APP_BUNDLE_NAME"
    pushd "$APP_BUNDLE_DESTINATION_DIR"
else
    # Keep the app bundle in the build/bin directory, but maybe copy it with another name
    pushd "$STARVIEWER_BUILD_DIR_BASE/bin"
    if [[ "$APP_BUNDLE_NAME" != starviewer.app ]]
    then
        mv starviewer.app "$APP_BUNDLE_NAME"
    fi
fi

$MACDEPLOYQT "$APP_BUNDLE_NAME" -qmldir="$STARVIEWER_SOURCE_DIR_BASE/src/qml"
$MACDEPLOYQT "$APP_BUNDLE_NAME/Contents/MacOS/starviewer_crashreporter.app"
$MACDEPLOYQT "$APP_BUNDLE_NAME/Contents/MacOS/starviewer_sapwrapper.app"

$DYLIBBUNDLER -od -b -x "$APP_BUNDLE_NAME/Contents/MacOS/starviewer" -d "$APP_BUNDLE_NAME/Contents/libs"
install_name_tool -add_rpath @executable_path/../libs "$APP_BUNDLE_NAME/Contents/MacOS/starviewer"

mv "$APP_BUNDLE_NAME/Contents/Resources/releasenotes/changelog.html" "$APP_BUNDLE_NAME/Contents/Resources/releasenotes/releasenotes$STABLE_VERSION.html"

if [[ -n "$PDF_MANUALS_DIR" ]]
then
    cp "$PDF_MANUALS_DIR/Starviewer_User_guide.pdf" "$APP_BUNDLE_NAME/Contents/Resources"
    cp "$PDF_MANUALS_DIR/Starviewer_Quick_start_guide.pdf" "$APP_BUNDLE_NAME/Contents/Resources"
    cp "$PDF_MANUALS_DIR/Starviewer_Shortcuts_guide.pdf" "$APP_BUNDLE_NAME/Contents/Resources"
fi

if [[ -n "$CUSTOM_SCRIPT" ]]
then
    . "$CUSTOM_SCRIPT"
fi

test -f "$DMG_NAME" && rm "$DMG_NAME"
$MACDEPLOYQT "$APP_BUNDLE_NAME" -dmg
mv "${APP_BUNDLE_NAME%app}dmg" "$DMG_NAME"

popd
