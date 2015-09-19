#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#define TIMEOUT 0
#define BLOCKSIZE 4096

void *connection_proc(void *);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

typedef struct {
	struct sockaddr_in *serv_addr;
	int addr_len;
	int httpver;
} parameter_t;

typedef struct {
	pthread_t pid;
	int num;
} pthread_info;

int main(int argc, char *argv[])
{
	//parse arguments
	if (argc < 5) {
       printf("usage: %s <server> <portnumber> <httpversion> <# of proc/threads>\n", argv[0]);
       exit(0);
    }

    int portno, numOfThreads, httpver;
    struct sockaddr_in serv_addr;
    struct hostent *server = gethostbyname(argv[1]);

    if (server == NULL) {
        printf("ERROR, no such host\n");
        exit(0);
    }
    numOfThreads = atoi(argv[4]);
    httpver = atoi(argv[3]);
    portno = atoi(argv[2]);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    parameter_t para;
    (&para)->serv_addr = &serv_addr;
    (&para)->httpver = httpver;
    (&para)->addr_len = sizeof(serv_addr);

    int i = 0;
    pthread_info *p_info = (pthread_info *)malloc(numOfThreads * sizeof(pthread_info));
    while (i < numOfThreads) {
    	p_info[i].num = i;
    	if (pthread_create(
    		&p_info[i].pid,
    		NULL,
    		connection_proc,
    		&para
    		) != 0)
    		printf("pthread_create fails!\n");
    	++i;
    };
    for (i = 0; i < numOfThreads; ++i)
    	pthread_join(p_info[i].pid, NULL);
    return 0;
}

void *connection_proc(void *argv) {
	parameter_t *p = argv;
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    printf("Start connecting\nSockfd: %d\n", sockfd);

    if (connect(sockfd,(struct sockaddr *) p->serv_addr,p->addr_len) < 0) 
        error("ERROR connecting");
    close(sockfd);
    printf("Stop connecting\n");

    return 0;
}
