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

void listener(int sockfd, unsigned int len, struct sockaddr_in cli);

int port;


// sends file, could be modified to also send 1 line updates
void sendMessages(int connfd){
	FILE* fp = fopen("test.txt", "r");


	char ch;
	int numchar = 0;
	char numString[5];

	// find out how many characters in file
	while((ch=fgetc(fp)) != EOF) {
		numchar++;
	}

	char str[numchar];

	sprintf(numString, "%d", numchar);

	// send num of characters to client
	send(connfd, numString, 5, 0);

	fclose(fp);
	
	fp = fopen("test.txt", "r");
	
	// copies file into a string
	for (int i = 0; i < numchar; i++){
		str[i] = fgetc(fp);
	}

	// sends file in one operation
	send(connfd, str, strlen(str), 0);

	fclose(fp);

}

// main operations of receiving from client
int func(int connfd){
    
    char buffer[MAX_SIZE];

	sendMessages(connfd);

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
	
	int sockfd, connfd;
	unsigned int len;
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
	int sockfd;
	unsigned int len;
	struct sockaddr_in servaddr, cli;


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

	listener(sockfd, len, cli);
}

void listener(int sockfd, unsigned int len, struct sockaddr_in cli){

	for ( ;; ) {
		char sport[5];
		int connfd;
		
		// Now server is ready to listen and verification
		if ((listen(sockfd, 5)) != 0) {
			printf("Listen failed...\n");
			exit(0);
		}
		else
			printf("Server listening..\n");
		len = sizeof(cli);
		
		// Accept the data packet from client and verification
		connfd = accept(sockfd, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("server accept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");

		sprintf(sport, "%d", ++port);

		// child process creates a new socket for the user
		if (fork() == 0){
			createSocket();
			return;
		}

		sleep(1);

		send(connfd, sport, strlen(sport), 0);
		//close(sockfd);
		sleep(5);
	}



}

int main(){

	port = PORT;

	listenerSocketCreate();


	// Function for chatting between client and server
	//func(connfd);

	// After chatting close the socket

	return 0;
}