#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define SA struct sockaddr
#define MAX_SIZE 80

int port;

int func(int connfd){
    
    char buffer[MAX_SIZE];

    while (1) {
		// clears the buffer and reads from socket again
        bzero(buffer, MAX_SIZE);
        ssize_t num = recv(connfd, buffer, sizeof(buffer), 0);

		// if buffer isn't empty or an error occurred
		if (num != 0 && num != -1){
   			printf("Size Received: %ld\n", num);
    		printf("Message Recieved: %s\n", buffer);
			
			if (strncmp(buffer, "exit", 4) == 0)
				return 0;
			
			// write message to file
			FILE* fp = fopen("test.txt", "a");
        	fprintf(fp, "%s\n", buffer);
			fclose(fp);
		}
    }
}

int createSocket(){
	
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	
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
		printf("socket bind failed...\n");
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

	len = sizeof(cli);

	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");
	

	func(connfd);

	return 0;
}

void listenerSocketCreate() {
	int sockfd, len;
	struct sockaddr_in servaddr, cli;

	port = PORT;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("socket creation failed...\n");
    } else {
        printf("Socket created\n");
    }

    servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
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
	len = sizeof(cli);

	listener(sockfd, len, cli);
}

void listener(int sockfd, int len, struct sockaddr_in cli){

	char sport[5];
	int connfd;
	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	sprintf(sport, "%d", ++port);

	if (fork() == 0){
		createSocket();
		return 0;
	}

	sleep(1);

	send(connfd, sport, strlen(sport), 0);
	fclose(sockfd);
	listenerSocketCreate();

}

int main(){

	listenerSocketCreate();


	// Function for chatting between client and server
	//func(connfd);

	// After chatting close the socket

	return 0;
}