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
