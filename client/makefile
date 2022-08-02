make:
	gcc `pkg-config --cflags gtk4` -o client client.c `pkg-config --libs gtk4` -lpthread

debug:
	gcc `pkg-config --cflags gtk4` -o client client.c `pkg-config --libs gtk4` -lpthread -g

clean:
	rm client