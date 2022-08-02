#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define SA struct sockaddr
#define MAX_SIZE 80

void listener(int sockfd, unsigned int len, struct sockaddr_in cli);

// the array of sockets used
int* connections;
int numConnections = 0;

int port;

// sends file, could be modified to also send 1 line updates
// void sendMessages(int connfd){
// 	FILE* fp = fopen("test.txt", "r");


// 	char ch;
// 	int numchar = 0;
// 	char numString[10];

// 	// find out how many characters in file
// 	while((ch=fgetc(fp)) != EOF) {
// 		numchar++;
// 	}

// 	char str[numchar];

// 	sprintf(numString, "%d", numchar);

// 	// send num of characters to client
// 	send(connfd, numString, 5, 0);

// 	fclose(fp);
	
// 	fp = fopen("test.txt", "r");
	
// 	// copies file into a string
// 	for (int i = 0; i < numchar; i++){
// 		str[i] = fgetc(fp);
// 	}

// 	// sends file in one operation
// 	send(connfd, str, strlen(str), 0);

// 	fclose(fp);

// }

// main operations of receiving from client
void *func(void* socket){
    int connfd = *((int*) socket);
    char buffer[MAX_SIZE];

    while (1) {
		// clears the buffer and reads from socket again
        bzero(buffer, MAX_SIZE);
        ssize_t num = recv(connfd, buffer, sizeof(buffer), 0);

		// if buffer isn't empty or an error occurred
		if (num != 0 && num != -1){
   			printf("Size Received: %ld\n", num);
    		printf("Message Recieved: %s\n", buffer);
			
			if (strncmp(buffer, "exit", 4) == 0){
				// clear out the entry in the connections array?
				// socket is just a placeholder
				return socket;
			}
			
			// write message to file
			FILE* fp = fopen("test.txt", "a");
        	fprintf(fp, "%s\n", buffer);
			fclose(fp);

			// sends the message received to all connected clients
			ssize_t sent;
			for(int i = 0; i < numConnections; i++){
				if(connections[i] != connfd && connections[i] == 0){
					sent = send(connections[i], buffer, strlen(buffer), 0);
					if(sent == 0 || sent == -1)
						printf("Failed to send message to port %d\n", connections[i]);
					else
						printf("Sent Message: %s\n", buffer);
				}
			}
		}
    }
}

int createSocket(){
	int sockfd;
	int *connfd = malloc(sizeof(*connfd));
	unsigned int len;
	struct sockaddr_in servaddr, cli;
	
	// creates initial socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
    } else {
        printf("Socket created\n");
    }

    servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	// accepts a new connection
	len = sizeof(cli);
	*connfd = accept(sockfd, (SA*)&cli, &len);
	if (*connfd < 0) {
		printf("Server accept failed...\n");
		exit(0);
	}
	else
		printf("Server accepted the client...\n");

	// hands the new connection off to a thread
	// client reconnects to the new port/socket
	connections[numConnections++] = *connfd;
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, func, connfd);

	return *connfd;
}

// creates the initial socket
void listenerSocketCreate() {
	int sockfd;
	unsigned int len;
	struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("Listener socket creation failed...\n");
    } else {
        printf("Listener socket created\n");
    }

    servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Listener socket bind failed...\n");
		exit(0);
	}
	else
		printf("Listener socket successfully binded..\n");

	listener(sockfd, len, cli);
}

// stays running on the main thread and handles any new connections to the server
void listener(int sockfd, unsigned int len, struct sockaddr_in cli){

	for ( ;; ) {
		char sport[10];
		int connfd;
		
		// Now server is ready to listen and verification
		if ((listen(sockfd, 5)) != 0) {
			printf("Listen failed...\n");
			exit(0);
		}
		else
			printf("Listening for new clients..\n");
		len = sizeof(cli);
		
		// Accept the data packet from client and verification
		connfd = accept(sockfd, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("server accept failed...\n");
			exit(0);
		}
		else
			printf("Redirecting client...\n");

		sprintf(sport, "%d", ++port);

		// child process creates a new socket for the user
		send(connfd, sport, strlen(sport), 0);
		createSocket();
		
		sleep(2);
	}
}

int main(void){
	port = PORT;
	connections = malloc(sizeof(int) * 100);

	listenerSocketCreate();

	return 0;
}