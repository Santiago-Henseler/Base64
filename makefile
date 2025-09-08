CFLAGS=-std=c99 -Wall -Werror -g

all: clean normal

clean:  
	rm -rf base64

normal: 
	gcc $(CFLAGS) base64.c -o base64


