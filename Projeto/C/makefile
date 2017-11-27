all: main.c
	gcc -o slave -Wl,--start-group -lgcrypt -lcrypto -Wl,--end-group main.c
