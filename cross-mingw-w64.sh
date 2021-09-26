#!/bin/bash

set -e

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

# first make sure OpenSSL is installed

OPENSSL_INSTALL_DIR=$HOME/mingw-w64/openssl

if ! [ -d "$OPENSSL_INSTALL_DIR" ]; then
    git clone --depth 1 https://github.com/openssl/openssl.git

    pushd openssl

    ./Configure --cross-compile-prefix=x86_64-w64-mingw32- mingw64

    make -j$(nproc)

    make install_sw DESTDIR="$HOME/mingw-w64/openssl"

    popd
fi

# Now install Qt 5.15

QT_VERSION=5.15

QT_INSTALL_DIR="$HOME/mingw-w64/Qt${QT_VERSION}"

if ! [ -d "$QT_INSTALL_DIR" ]; then
    git clone --depth 1 -b ${QT_VERSION} git://code.qt.io/qt/qt5.git

    pushd qt5

    QT_SUBMODULES="qtbase,qtfeedback,qtimageformats,qtlottie,qtpim,qtqa,qtsvg,qtsystems"

    ./init-repository --module-subset="$QT_SUBMODULES"

    ./configure \
        -xplatform win32-g++ \
        -device-option CROSS_COMPILE=/usr/bin/x86_64-w64-mingw32- \
        -prefix "$HOME/mingw-w64/Qt${QT_VERSION}" \
        -opensource \
        -confirm-license \
        -no-compile-examples \
        -nomake examples \
        -nomake tests \
        -nomake tools \
        -opengl desktop \
        -release \
        -skip qtactiveqt -skip qtcharts -skip qtdoc -skip qtlocation \
        -skip qtremoteobjects -skip qtserialbus -skip qtserialport -skip qtwebchannel \
        -skip qtwebview -skip qtandroidextras -skip qtconnectivity \
        -skip qtgamepad -skip qtmacextras -skip qtpurchasing -skip qtscript \
        -skip qttranslations -skip qtwebengine -skip qtwinextras \
        -skip qtdatavis3d -skip qtgraphicaleffects -skip qtmultimedia \
        -skip qtquickcontrols -skip qtscxml -skip qtspeech \
        -skip qtvirtualkeyboard -skip qtwebglplugin -skip qtx11extras \
        -skip qt3d -skip qtcanvas3d -skip qtdeclarative \
        -skip qtnetworkauth -skip qtquickcontrols2 \
        -skip qtsensors -skip qtwayland -skip qtwebsockets -skip qtxmlpatterns

    make -j$(nproc)

    make install

    popd
fi

mkdir build-mingw-w64

pushd build-mingw-w64

cmake "$REPO_ROOT" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_TOOLCHAIN_FILE="$REPO_ROOT/toolchains/mingw-w64.cmake" \
    -DQt5_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5" \
    -DQt5Core_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Core" \
    -DQt5Network_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Network" \
    -DQt5Widgets_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Widgets" \
    -DQt5Gui_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Gui" \
    -DQt5Svg_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Svg" \
    -DQt5Zlib_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Zlib" \
    -DQt5ThemeSupport_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5ThemeSupport" \
    -DQt5AccessibilitySupport_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5AccessibilitySupport" \
    -DQt5EventDispatcherSupport_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5EventDispatcherSupport" \
    -DQt5FontDatabaseSupport_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5FontDatabaseSupport" \
    -DQt5WindowsUIAutomationSupport_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5WindowsUIAutomationSupport" \
    -DOPENSSL_USE_STATIC_LIBS=TRUE \
    -DOPENSSL_ROOT_DIR="$HOME/mingw-w64/openssl/usr/local/" \
    -DOPENSSL_INCLUDE_DIR="$HOME/mingw-w64/openssl/usr/local/include/openssl" \
    -DOPENSSL_LIBRARIES="$HOME/mingw-w64/openssl/usr/local/lib64" \
    -DOPENSSL_CRYPTO_LIBRARY="$HOME/mingw-w64/openssl/usr/local/lib64/libcrypto.a" \
    -DOPENSSL_SSL_LIBRARY="$HOME/mingw-w64/openssl/usr/local/lib64/libssl.a"

cmake --build . -- -j$(nproc)

popd

mkdir qide-w64

mv build-mingw-w64/qide/qide.exe qide-w64

cp $QT_INSTALL_DIR/bin/Qt5{Core,Gui,Svg,Widgets,Network}.dll qide-w64

QT_RUNTIME_DIRS=(bearer iconengines imageformats platforms styles)

for runDir in ${QT_RUNTIME_DIRS[@]}; do
    mkdir qide-w64/$runDir
    cp $QT_INSTALL_DIR/plugins/$runDir/*.dll qide-w64/$runDir
done

GCC_LIBRARIES=(libgcc_s_seh-1.dll libstdc++-6.dll)

for lib in ${GCC_LIBRARIES[@]}; do
    cp /usr/lib/gcc/x86_64-w64-mingw32/9.3-posix/$lib qide-w64
done

cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll qide-w64

zip -r qide-w64.zip qide-w64

mv qide-w64.zip $OLD_CWD
