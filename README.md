# cpsc3600ChatProject

## About
This project was an open-ended project for my Network Systems class. The project is a chat room server that includes a client software that runs a minimalist GUI. Multiple clients can be connected to the server and messages sent from a client show up on the other clients. The server is multithreaded to accomplish this.

This project is written in C and GTK was used to provide a GUI. The program is only tested on Fedora, but other Linux distributions should work if GTK 4.0 was installed.

## Contributors
Luke Welborn & Joel Pittard
## FUTURE TO-DO LIST
* exit threads on server cleanly
    * remove and close socket in the array of sockets 
    * mark port as open maybe?
    * then close thread, need a way to keep track of thread ids (maybe put in struct w connfd in connections array)
* why are there multiple client threads?
* add username option for client
* add multiple lines in client text view
* able to pull chat history to client
* scroll the text view maybe?
* get rid of any warnings (use -Wall)
