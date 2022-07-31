make:
	gcc server.c -o server
	gcc client.c -o client
#gcc $( pkg-config --cflags gtk4 ) -o client3 client3.c $( pkg-config --libs gtk4 )

clean:
	rm server client