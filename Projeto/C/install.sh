#!/bin/bash

LIBNACL_DL_URL=https://hyperelliptic.org/nacl/nacl-20110221.tar.bz2
LIBNACL_A_DIR=nacl-20110221/build/debian/lib/amd64

LIBGCRYPT_DL_URL=https://gnupg.org/ftp/gcrypt/libgcrypt/libgcrypt-1.8.1.tar.bz2
LIBGCRYPT_A_DIR=libgcrypt-1.8.1/src/.libs/

if [[ ! -r $LIBNACL_A_DIR/libnacl.a ]]; then
	if [ ! -r $(echo $LIBNACL_DL_URL | sed -r "s/.*\/(.+)$/\1/") ]; then
		echo "Downloading nacl..."
		wget -q $LIBNACL_DL_URL >/dev/null
		echo "Downloaded nacl!"
	fi
	echo "Extracting nacl..."
	bunzip2 < $(echo $LIBNACL_DL_URL | sed -r "s/.*\/(.+)$/\1/") | tar -xf - >/dev/null
	echo "Extracted nacl!"
	cd $(echo $LIBNACL_DL_URL | sed -r "s/.*\/(.+)\.tar\.bz2$/\1/") > /dev/null
	echo "Compiling nacl..."
	./do >/dev/null
	echo "Compiled nacl!"
	cd - >/dev/null
	rm -rf $(echo $LIBNACL_DL_URL | sed -r "s/.*\/(.+)$/\1/") >/dev/null
fi

#gcc -o slave -L$LIBNACL_A_DIR -lnacl main.c

#exit 0

_temp_dir=$(mktemp -d)
if [ ! -r $LIBGCRYPT_A_DIR/libgcrypt.so ]; then
	_pwd=$(pwd)
	cd $_temp_dir
        if [[ ! -r $(echo $LIBGCRYPT_DL_URL | sed -r "s/.*\/(.+)$/\1/") ]]; then
		echo "Downloading gcrypt..."
		wget -q $LIBGCRYPT_DL_URL >/dev/null
	        echo "Downloaded gcrypt!"
	fi
        echo "Extracting gcrypt..."
        bunzip2 < $(echo $LIBGCRYPT_DL_URL | sed -r "s/.*\/(.+)$/\1/") | tar -xf - >/dev/null
        echo "Extracted gcrypt!"
        cd $(echo $LIBGCRYPT_DL_URL | sed -r "s/.*\/(.+)\.tar\.bz2$/\1/") > /dev/null
	echo "Configuring gcrypt..."
	./configure >/dev/null
	echo "Configured gcrypt!"
        echo "Compiling gcrypt..."
        make >/dev/null
        echo "Compiled gcrypt!"
	echo "Installing gcrypt..."
	make install >/dev/null
	echo "Installed gcrypt!"
        cd "$_pwd" >/dev/null
        rm -rf _temp_dir >/dev/null
fi


gcc -o slave -Wl,--start-group  -lnacl -lgcrypt -lcrypto -Wl,--end-group -L. main.c
#gcc -o slave main.c
