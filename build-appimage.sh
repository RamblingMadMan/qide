#!/bin/bash

####################################
#  - QIDE Appimage build script -
####################################
#
# Based mostly off of the stuff available from the AppImage website:
# https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#using-cmake-and-make-install
#
####################################

# terminate on command error
set -e

TEMP_TEMPLATE="qide-appimage-build-XXXXXX"

if [ "$CI" == "" ] && [ -d /dev/shm ]; then
	BUILD_DIR=$(mktemp -d -p /dev/shm $TEMP_TEMPLATE)
else
	BUILD_DIR=$(mktemp -d $TEMP_TEMPLATE)
fi

cleanup(){
	if [ -d "$BUILD_DIR" ]; then
		rm -rf "$BUILD_DIR"
	fi
}

# Basically RAII for bash
trap cleanup EXIT

REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

# Switch to build dir
pushd "$BUILD_DIR"

# Configure the project
cmake "$REPO_ROOT" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr

# Build the project
make -j$(nproc)
make install DESTDIR=AppDir

# now, build AppImage using linuxdeploy and linuxdeploy-plugin-qt
# download linuxdeploy and its Qt plugin
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage

# make them executable
chmod +x linuxdeploy*.AppImage

# make sure Qt plugin finds QML sources so it can deploy the imported files
# just in case QML ever does get added
export QML_SOURCES_PATHS="$REPO_ROOT/qide/qml"

./linuxdeploy-x86_64.AppImage --appdir AppDir --plugin qt --output appimage

mv QIDE*.AppImage "$OLD_CWD"
