/************************************************************************\
* 			  INET_WSTREAM.C       			         *
* Test of TCP/IP. Invoke as                                              *
*             inet_wstream hostname portnumber                           *
* in order to send a fragment of an obnoxious Dijkstra quote to a        *
* process assumed to be reading from a stream at the specified           *
* address.                                                               *
* 								         *
* Phil Kearns							         *
* April 11, 1987						         *
* 								         *
* Modified February 2009 to use getaddrinfo()                            * 
\************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "common.h"



//char msg[] = { "false pearls before real swine" };

int main(argc, argv)
int argc;
char **argv;
{
  int sock, left, num, put, ecode,hits;
  struct sockaddr_in *server;
  struct addrinfo hints, *addrlist;
  char ch;

  int mode=0;//default connection mode, 1 is query mode
  int time_out=-1;

  fd_set mask;
  struct timeval timeout;

  
  if (argc > 3) {
    fprintf(stderr,"too many arguments\n");
    exit(1);
  }

  if (argc==2){
      if (!strcmp(*(argv+1), "-q")){
        printf ("query mode \n");
        mode=1;
      }    


  }
  if (argc==3){
      if (!strcmp(*(argv+1), "-t")){
          //printf("time_out: %s\n",argv[2]);

          time_out=atoi(argv[2]);
          printf("will shutdown if no game to play after %d secs\n",time_out);

      }

  }
 


  

  int hostnamefile=open("hostname.txt",O_RDONLY);
  int var=1;
  if (hostnamefile<0){
    perror("control file missing");
    printf("Will time out in 60 secs if there is still no control file\n ");
    while (hostnamefile<0){
      sleep(10);
      if (var >= 6){
          printf("shutting down!!!\n");
          exit(0);

      }
          
      hostnamefile=open("hostname.txt",O_RDONLY);
      var++;
    }
  }


  char hostname[1024];
  char c;
  int i=0;
  while (read(hostnamefile,&c,1)==1){
      if (c==0)
        break;
      hostname[i++]=c;
  }
  hostname[i]='\0';
  printf("hostname is %s\n",hostname);


/*
   Want a sockaddr_in containing the ip address for the system
   specified in argv[1] and the port specified in argv[2].
*/

if (mode ==1){
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_NUMERICSERV; hints.ai_protocol = 0;
  hints.ai_canonname = NULL; hints.ai_addr = NULL;
  hints.ai_next = NULL;

  ecode = getaddrinfo(hostname, "52116", &hints, &addrlist);
  if (ecode != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ecode));
    exit(1);
  }

  server = (struct sockaddr_in *) addrlist->ai_addr; // Will use in sendto().
  sock = socket (addrlist->ai_family, addrlist->ai_socktype, 0);
  if (sock == -1) {
    perror ("liu_ttt:socket");
    exit (1);
  }
  struct UDPmsg* msg=(struct UDPmsg* )malloc(sizeof(struct UDPmsg));
  ecode = sendto(sock,msg,sizeof(*msg),0,(struct sockaddr *) server,
                  sizeof(struct sockaddr_in));
  if (ecode < 0) {
    perror("liu_ttt:sendto");
    exit(1);
  }
  int length=sizeof(*server);
  ecode = recvfrom(sock,msg,sizeof(*msg),0,(struct sockaddr *)&server,&length);
  if (ecode < 0) perror("TTT_udp:recvfrom");
  if (msg->game_in_process==0){
    printf("There are no game in progress\n");
      if (msg->total_connection>0){
        printf("But %s is waiting for an opponent\n",msg->handle);
      }
  }else if (msg->game_in_process==1){
    printf("%s and %s are playing game\n",msg->handle,msg->opp_handle);
    //printf("There are %d connections at TTT\n",msg->total_connection);
  }

}else{
  memset( &hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV; hints.ai_protocol = 0;
  hints.ai_canonname = NULL; hints.ai_addr = NULL;
  hints.ai_next = NULL;

  ecode = getaddrinfo(hostname, "52016", &hints, &addrlist);
  if (ecode != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ecode));
    exit(1);
  }

  server = (struct sockaddr_in *) addrlist->ai_addr;

/*
   Create the socket.
*/
  if ( (sock = socket( addrlist->ai_family, addrlist->ai_socktype, 0 )) < 0 ) {
    perror("liu_ttt:socket");
    exit(1);
  }

/*
   Connect to data socket on the peer at the specified Internet
   address.


*/
  if ( connect(sock, (struct sockaddr *)server, sizeof(struct sockaddr_in)) < 0) {
    perror("inet_wstream:connect");
    exit(1);
  }

  struct Message* recvmsg=(struct Message* )malloc(sizeof(struct Message));
  struct Message* sendmsg=(struct Message* )malloc(sizeof(struct Message));

  while (1){
    if (!(time_out==-1)){
        FD_ZERO(&mask);
        FD_SET(sock,&mask);

        timeout.tv_sec = time_out;
        timeout.tv_usec = 0;
        //printf("will shutdown if no game to play after %d secs\n",timeout.tv_sec);

        if ((hits = select(sock+1, &mask, (fd_set *)0, (fd_set *)0,
                           &timeout)) < 0) {
                perror("liu_ttt:select");
                exit(1);
          }
        if (hits==0){//Timeout
        printf("Time out! Shutting down\n");
        exit(0);
        }
        time_out=-1;

    }
    
    int retval=recvMsg(sock,recvmsg,sendmsg);
    if (retval==-1){
      printf("server crash!\n");
      exit(0);
    }
    if (recvmsg->type==RESULT)
      break;
    if (recvmsg->type==SHUTDOWN){
      printf("Sorry, rejection for too many connetion in server\n");
      break;
    }
    if (recvmsg->type==OPPO_QUIT){
      printf("Sorry, your opponent quit, you're forced to leave!\n");
      break;
    }
      
    }
  }

  exit(0);
}

