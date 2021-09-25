#!/bin/bash

set -e

OLD_CWD=$(readlink -f .)

sudo apt install mingw-w64 build-essential libgl1-mesa-dev python

sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix
sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix

# first install OpenSSL
git clone --depth 1 https://github.com/openssl/openssl.git

pushd openssl

./Configure --cross-compile-prefix=x86_64-w64-mingw32- mingw64

make -j$(nproc)

make install DESTDIR="$HOME/mingw-w64/openssl"

popd

rm -rf openssl

# Now install Qt 5.15

QT_VERSION_MM=5.15
QT_VERSION=5.15.2

QT_ARCHIVE_NAME="qt-everywhere-src-${QT_VERSION}"
QT_ARCHIVE_EXT=".tar.xz"
QT_ARCHIVE="${QT_ARCHIVE_NAME}${QT_ARCHIVE_EXT}"

curl http://ftp.yz.yamagata-u.ac.jp/pub/qtproject/archive/qt/${QT_VERSION_MM}/${QT_VERSION}/single/${QT_ARCHIVE} --output $QT_ARCHIVE

tar -xf $QT_ARCHIVE

pushd ${QT_ARCHIVE_NAME}

./configure \
    -xplatform win32-g++ \
    -device-option CROSS_COMPILE=/usr/bin/x86_64-w64-mingw32- \
    -prefix "$HOME/mingw-w64/Qt${QT_VERSION}" \
    -opensource \
    -confirm-license \
    -no-compile-examples \
    -nomake examples \
    -opengl desktop \
    -skip qtactiveqt -skip qtcharts -skip qtdoc -skip qtlocation \
    -skip qtremoteobjects -skip qtserialbus -skip qtwebchannel \
    -skip qtwebview -skip qtandroidextras -skip qtconnectivity \
    -skip qtgamepad -skip qtmacextras -skip qtpurchasing -skip qtscript \
    -skip qttranslations -skip qtwebengine -skip qtwinextras \
    -skip qtdatavis3d -skip qtgraphicaleffects -skip qtmultimedia \
    -skip qtquickcontrols -skip qtscxml -skip qtspeech \
    -skip qtvirtualkeyboard -skip qtwebglplugin -skip qtx11extras \
    -skip qt3d -skip qtcanvas3d -skip qtdeclarative \
    -skip qtimageformats -skip qtnetworkauth -skip qtquickcontrols2 \
    -skip qtsensors -skip qtwayland -skip qtwebsockets

make -j$(nproc)

make install

popd

rm -rf ${QT_ARCHIVE_NAME}

mkdir build-mingw-w64

pushd build-mingw-w64

cmake .. \
    -DQt5_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5" \
    -DQt5Core_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Core" \
    -DQt5Network_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Network" \
    -DQt5Widgets_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Widgets" \
    -DQt5Gui_DIR="$HOME/mingw-w64/Qt${QT_VERSION}/lib/cmake/Qt5Gui" \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/mingw-w64.cmake \
    -DOPENSSL_USE_STATIC_LIBS=TRUE \
    -DOPENSSL_ROOT_DIR="$HOME/mingw-w64/openssl/usr/local/" \
    -DOPENSSL_INCLUDE_DIR="$HOME/mingw-w64/openssl/usr/local/include/openssl" \
    -DOPENSSL_LIBRARIES="$HOME/mingw-w64/openssl/usr/local/lib64" \
    -DOPENSSL_CRYPTO_LIBRARY="$HOME/mingw-w64/openssl/usr/local/lib64/libcrypto.a" \
    -DOPENSSL_SSL_LIBRARY="$HOME/mingw-w64/openssl/usr/local/lib64/libssl.a"

cmake --build . -- -j$(nproc)

mv qide/qide.exe $OLD_CWD

popd
