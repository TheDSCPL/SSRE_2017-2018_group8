#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import itertools

sys.path.append('Projeto/Python/cryptopy')

import crypto.cipher.rijndael
import crypto.cipher.aes
import crypto.cipher.cbc

KEY = "ligTNvexz0Zr1cJQEBrEbRwyHkTYJORV"
DATA_FILE_PATH = "Projeto/data"

iter_count = int(sys.argv[1])
algorithm = int(sys.argv[2])
library = int(sys.argv[3])
mode = int(sys.argv[4])



def open_data(path):
	with open(path, mode = 'rb') as file:
		return file.read()


def md5():
	alg = crypto.hash.md5Hash.MD5()
	result = alg(data)

def sha1():
	alg = crypto.hash.sha1Hash.SHA1()
	result = alg(data)

def aes256(mode):
        r = crypto.cipher.rijndael.Rijndael(KEY,
                                            keySize = len(KEY),
                                            blockSize = 32,
                                            padding = crypto.cipher.aes.noPadding()
                                            )

        alg = crypto.cipher.cbc.CBC(r, padding = crypto.cipher.aes.noPadding())
        
	#alg = crypto.cipher.aes.AES(key, keySize = len(key), padding = crypto.cipher.aes.noPadding())

	if mode == 0: result = alg.encrypt(data)
	elif mode == 1: result = alg.decrypt(data)


data = open_data(DATA_FILE_PATH)

func = None
if algorithm == 0: func = md5
elif algorithm == 1: func = sha1
elif algorithm == 4: func = aes256(mode)

if func:
	for _ in itertools.repeat(None, iter_count):
		func()

















