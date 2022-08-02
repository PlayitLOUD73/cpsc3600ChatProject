#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>


#define PORT 8080
#define MAX_SIZE 80
#define SA struct sockaddr

int sockfd;
pthread_t thread_id;

static void sendMessage (GtkWidget *widget, gpointer data){
    // converts the callback data to a useable format
    // from: https://stackoverflow.com/questions/3225302/get-text-from-a-gtkentry 
    GtkWidget *entry = gtk_entry_new();
    entry = (GtkWidget*) data;

    // gets text from the entry field 
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
    const char *text = gtk_entry_buffer_get_text(buffer);

    // sends the message to the server
    ssize_t n = send(sockfd, text, strlen(text), 0);
}

static void *receiveMessage(void *widget){
    GtkWidget *view = (GtkWidget*) widget;
    char buffer[MAX_SIZE];
    //char* text = malloc(sizeof(char) * 10000); 

    // waits for new messages and updates screen
    while(1){
        bzero(buffer, MAX_SIZE);
        ssize_t messageSize = recv(sockfd, buffer, MAX_SIZE, 0);
        if(messageSize != 0 && messageSize != -1){
            //strcat(text, buffer);
            GtkTextBuffer* gtkBuff = gtk_text_buffer_new(gtk_text_tag_table_new());
            gtk_text_buffer_set_text(gtkBuff, buffer/*text*/, strlen(buffer));
            gtk_text_view_set_buffer(GTK_TEXT_VIEW(view), gtkBuff);
        }
    }
}

static void activate (GtkApplication *app, gpointer user_data) {

    GtkBuilder *builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "builder.ui", NULL);

    /* Connect signal handlers to the constructed widgets. */
    GObject *window = gtk_builder_get_object (builder, "window");
    gtk_window_set_application (GTK_WINDOW (window), app);

    // sets the text that goes into the viewer
    GObject *view = gtk_builder_get_object(builder, "view");
    pthread_create(&thread_id, NULL, receiveMessage, (void*)GTK_TEXT_VIEW(view));

    GObject *entry = gtk_builder_get_object(builder, "text");

    // attaches the send message function to the button click
    GObject *button = gtk_builder_get_object (builder, "button1");
    g_signal_connect (button, "clicked", G_CALLBACK (sendMessage), entry);

    gtk_widget_show (GTK_WIDGET (window));

    g_object_unref (builder);
}


// loads the file sent from the server (could be modified to append to the end and only be 1 line as well)
// void loadMessages(int sockfd){
//
//
//     FILE* fp = fopen("messages.txt", "w");
//     char hold[5];
//
//     // get the length of the file
//     recv(sockfd, hold, 5 * sizeof(char), 0);
    //
//     int numchar = atoi(hold);
//
//     // create proper length of buffer
//     char buffer[numchar];
//     bzero(buffer, numchar);
//
//     // get the file in one operation
//     recv(sockfd, buffer, sizeof(buffer), 0);
//     fprintf(fp, "%s", buffer);
//
//     fclose(fp);
//
// }


int createSocket(int portNum){
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    char* newSocket = malloc(sizeof(char) * 10);
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("159.223.187.217");
    // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    servaddr.sin_port = htons(portNum);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else{
        printf("connected to the server..\n");
        // rebinds to a different server port to allow server to have multiple connections
        if(portNum == PORT){
            ssize_t num = recv(sockfd, newSocket, sizeof(newSocket), 0);
            if(num == 0 || num == -1){
                printf("socket rebind failed\n");
                exit(0);
            }
            close(sockfd);
            createSocket(atoi(newSocket));
        }
    }

    return sockfd;
}


int main (int argc, char *argv[]){
    #ifdef GTK_SRCDIR
        g_chdir (GTK_SRCDIR);
    #endif
    // connects socket to server
    sockfd = createSocket(PORT);

    // sets up data for ui
    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);

    // runs ui, then exits
    int status = g_application_run (G_APPLICATION(app), argc, argv);
    g_object_unref (app);

    close(sockfd);
    pthread_join(thread_id, NULL);
    return status;
}