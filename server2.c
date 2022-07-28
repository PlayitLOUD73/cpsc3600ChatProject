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
#define MAX 80

FILE* fp;

int func(int connfd){
    
    char buffer[MAX];
    int n;
	int test;

    while (1) {
        bzero(buffer, MAX);

        ssize_t num = recv(connfd, buffer, sizeof(buffer), 0);
		if (num != 0){
			printf("%d\n",num);
		}
			printf("received message %s", buffer);
			if (strncmp(buffer, "exit", 4) == 0){
				fclose(fp);
				return 0;
			}
        	fprintf(fp, "%s\n", buffer);
    }
}


int main(){

    fp = fopen("test.txt", "a+");

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

    // Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}