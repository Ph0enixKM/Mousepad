all:
	gcc main.c -lX11 -lXtst -o mousepad

clean:
	rm mousepad

install:
	cp ./mousepad /bin/mousepad

uninstall:
	rm /bin/mousepad
