#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


#define PORT 8080
#define SA struct sockaddr

int sockfd;

static void sendMessage (GtkWidget *widget, gpointer data){
    // converts the callback data to a useable format
    GtkWidget *entry = gtk_entry_new();
    entry = (GtkWidget*) data;

    // gets text from the entry field 
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
    const char *text = gtk_entry_buffer_get_text(buffer);

    // sends the message to the server
    ssize_t n = send(sockfd, text, sizeof(text), 0);
    printf("Size Sent: %d\n", n);
    printf("Message: %s\n", text);
}

static void activate (GtkApplication *app, gpointer user_data) {


    GtkBuilder *builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "builder.ui", NULL);

    /* Connect signal handlers to the constructed widgets. */
    GObject *window = gtk_builder_get_object (builder, "window");
    gtk_window_set_application (GTK_WINDOW (window), app);
    
    GObject *entry = gtk_builder_get_object(builder, "text");

    GObject *button = gtk_builder_get_object (builder, "button1");
    g_signal_connect (button, "clicked", G_CALLBACK (sendMessage), entry);

    gtk_widget_show (GTK_WIDGET (window));

    g_object_unref (builder);
}

int createSocket(){
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
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
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    return sockfd;
}


int main (int argc, char *argv[]){
#ifdef GTK_SRCDIR
    g_chdir (GTK_SRCDIR);
#endif

    sockfd = createSocket();


    
    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run (G_APPLICATION(app), argc, argv);

    g_object_unref (app);

    return status;
}