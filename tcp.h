#define MAX 1024
#define SA struct sockaddr
int create_socket(int PORT);
int local(int sockfd,int saved_stdout);