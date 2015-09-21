    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h> 
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <pthread.h>
    //The example on http://www.gnu.org/software/libc/manual/html_node/Server-Example.html omits this library!
    #include <arpa/inet.h>

    #define MAXMSG 4096

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
#define SERVER_STRING "Server: myhttpd_wu636\r\n"
void headers(int fd, const char *filename, const char *type)
{
 char buf[1024];
 (void)filename;  /* could use filename to determine file type */

 strcpy(buf, "HTTP/1.1 200 OK\r\n");
 send(fd, buf, strlen(buf), 0);
 strcpy(buf, SERVER_STRING);
 send(fd, buf, strlen(buf), 0);
 //sprintf(buf, "Content-Type: text/html\r\n");
 sprintf(buf, "Content-Type: %s\r\n", type);
 send(fd, buf, strlen(buf), 0);
 strcpy(buf, "\r\n");
 send(fd, buf, strlen(buf), 0);
}

int read_from_client (int fd) {
  printf("read_from_client: fd = %d\n", fd);
  char buf[MAXMSG];
  bzero(buf,MAXMSG);
  int n;
  n = read(fd,buf,MAXMSG);
  //if EOF, return -1
  if (n < 0) {error("ERROR reading from socket"); return -1;}
  printf("Here is the message: \n%s\n",buf);

  //Parse the request
  char *method;
  char *url;
  char *httpver;
  char *filetype;
  char *delim = " \r\n";
  
  method = strtok (buf, delim);
  url = strtok (NULL, delim);
  httpver = strtok (NULL, delim);
  while(0 != strcmp(strtok (NULL, delim), "Accept:")) ;
  filetype = strtok (NULL, delim);
  //For adapting Chrome & Firefox
  filetype = strtok (filetype, ",");
  
  printf("method is %s, url is %s, httpver is %s, type is %s\n", method, url, httpver, filetype);
  strtok(httpver, "/");
  float vnumber = atof(strtok(NULL, "\0"));
  printf("vnumber = %.1f\n", vnumber);

  char filename[255];
  sprintf(filename, ".%s", url);
  if (filename[strlen(filename) - 1] == '/')
    strcat(filename, "index.html");
  printf("%s\n", filename);
  char outbuf[MAXMSG];
  FILE *resource = NULL;
  resource = fopen(filename, "r");
  if (resource == NULL) {
    printf("Cannot find file %s\n", filename);
    return -1;
  }
  headers(fd, filename, filetype);
  fgets(outbuf, sizeof(outbuf), resource);
  while (!feof(resource)) {
    n = write(fd,outbuf,strlen(outbuf));
    if (n < 0) error("ERROR writing to socket");
    fgets(outbuf, sizeof(outbuf), resource);
  }
  fclose(resource);

  return -1;
}

int main(int argc, char *argv[]) {
  int sockfd, portno, timeout, httpver;
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t clilen;
  fd_set active_fd_set, read_fd_set, write_fd_set;

  if (argc != 4) {
    printf("usage: %s <httpversion> <portnumber> <timeout>\n", argv[0]);
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("ERROR opening socket\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  timeout = atoi(argv[3]);
  portno = atoi(argv[2]);
  httpver = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
      //Not sure if we set addr as INADDR_ANY which is the wildcard
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    printf("ERROR binding socket\n");
    exit(0);
  }
      //Set backlog as 10, which is the queue length of pending request could be
  listen(sockfd, 10);
      //Start the event-driven part
  FD_ZERO (&active_fd_set);
  FD_SET (sockfd, &active_fd_set);
  while (1) {
        /* Block until input arrives on one or more active sockets. */
        /* why read_fd_set = active_fd_set? */
    read_fd_set = active_fd_set;
    printf("select\n");
        //for http 1.1, we may need timeout to determine which socket to close
    if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      printf("Select fails!\n");
      exit(0);
    }

        /* Service all the sockets with input pending. */
    int i;
    for (i = 0; i < FD_SETSIZE; ++i) {
          //printf("%d\n", i);
      if (FD_ISSET (i, &read_fd_set)) {
        if (i == sockfd) {
          int new_sockfd;
          clilen = sizeof(cli_addr);
          new_sockfd = accept (sockfd,
            (struct sockaddr *) &cli_addr,
            &clilen);
          if (new_sockfd < 0) {
            printf("ERROR accepting connection request!\n");
            exit(0);
          }
          printf("create new sockfd: %d\n", new_sockfd);

          printf ("Server: connect from host %s, port %u.\n",
            inet_ntoa (cli_addr.sin_addr),
            ntohs (cli_addr.sin_port));

          FD_SET (new_sockfd, &active_fd_set);
        }
        else {
         if (read_from_client(i) < 0) {
           //Reach the end of input
           close(i);
           FD_CLR (i, &active_fd_set);
         }
       }
     }
     else if(FD_ISSET (i, &write_fd_set)) {

     } 
     else {

     }
   }

 }

 close(sockfd);
 return 0;
}
