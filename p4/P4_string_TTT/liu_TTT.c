/************************************************************************\
* 			  INET_RSTREAM.C       			         *
* Test of TCP/IP. Set up a socket for establishing a connection at       *
* some free port on the local host. After accepting a connection, speak  *
* briefly to peer, and then read char at a time from the stream. Write   *
* to stdout. At EOF, shut down.                                          *
* 								         *
* Phil Kearns							         *
* April 11, 1987						         *
* 								         *
* Modified February 2009 to use getaddrinfo()                            *
\************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "common.h"

void printsin(struct sockaddr_in*, char*, char*);



void startGame(struct Game* game);

int main()
{
  int listener;  /* fd for socket on which we get connection requests */
  //int conn,conn2;      /* fd for socket thru which we pass data */
  struct sockaddr_in *localaddr, peer,peer2;
  int ecode;
  socklen_t length,length2;
  char ch;
  struct addrinfo hints, *addrlist;

  //struct GameStatus *gamestatus;
  //initGameStatues(gamestatus);
  int total_conn=0;//total number of 
  int conn[7];//all the connection the server maintains
  char oppo[2][HANLELEN];//handle of the two opponent
  struct Game* game;


/* 
   Want to specify local server address of:
      addressing family: AF_INET
      ip address:        any interface on this system 
      port:              0 => system will pick free port
*/

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV | AI_PASSIVE; hints.ai_protocol = 0;
  hints.ai_canonname = NULL; hints.ai_addr = NULL;
  hints.ai_next = NULL;

  ecode = getaddrinfo(NULL, "52016", &hints, &addrlist);
  if (ecode != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ecode));
    exit(1);
  }

  localaddr = (struct sockaddr_in *) addrlist->ai_addr;
 
/*
   Create socket on which we will accept connections. This is NOT the
   same as the socket on which we pass data.
*/
  if ( (listener = socket( addrlist->ai_family, addrlist->ai_socktype, 0 )) < 0 ) {
    perror("inet_rstream:socket");
    exit(1);
  }


  if (bind(listener, (struct sockaddr *)localaddr, sizeof(struct sockaddr_in)) < 0) {
    perror("inet_rstream:bind");
    exit(1);
  }

/*
   Print out the port number assigned to this process by bind().
*/
  length = sizeof(struct sockaddr_in);
  if (getsockname(listener, (struct sockaddr *)localaddr, &length) < 0) {
    perror("inet_rstream:getsockname");
    exit(1);
  }
  printf("RSTREAM:: assigned port number %d\n", ntohs(localaddr->sin_port));

/*
   Now accept a single connection. Upon connection, data will be
   passed through the socket on descriptor conn.
*/
   //**************************************
   length = sizeof(peer);
  

  while (1){
    listen(listener,1);
    if ((conn[total_conn++]=accept(listener, (struct sockaddr *)&peer, &length)) < 0) {
        perror("inet_rstream:accept");
        exit(1);
    }
    printsin(&peer,"RSTREAM::", "accepted connection from"); 

    struct Message* whoMsg=(struct Message* )malloc(sizeof(struct Message));
    whoMsg->type=WHO; 
    sendMsg(conn[total_conn-1],whoMsg);

    struct Message* recvmsg=(struct Message* )malloc(sizeof(struct Message));
    //initMsg(recvmsg);

    struct Message* sendmsg=(struct Message* )malloc(sizeof(struct Message));
    //initMsg(sendmsg);
    recvMsg(conn[total_conn-1],recvmsg,sendmsg);
    if (recvmsg->type==HANDLE){
        //printf("received HANDLE: recvmsg->handle:%s, length=%d\n",recvmsg->handle,strlen(recvmsg->handle));
        strcpy(oppo[total_conn-1],recvmsg->handle);
        printf("The oppo %d's handle is %s\n",total_conn-1,oppo[total_conn-1]);
    }
    if (total_conn==2){
        printf("there is a match:\n");
        printf("the oppo %d's handle is %s\n,the oppo %d's handle is %s\n", 0,oppo[0],1,oppo[1]);
        //game=newgame(conn[0],conn[1],oppo[0],oppo[1]);
        game=(struct Game* )malloc(sizeof(struct Game));
        game->match[0]=conn[0];
        game->match[1]=conn[1];
        strcpy(game->opponent[0],oppo[0]);
        strcpy(game->opponent[1],oppo[1]);
        int i;
        for (i=0;i<9;i++){
        game->board[i]='1'+i;
        }
        startGame(game);

    }

  }
  



  //write(conn, "Who", 3);
/*
  printf("\n\nRSTREAM:: data from stream:\n");
  while ( read(conn, &ch, 1) == 1)
    putchar(ch);
  putchar('\n');
  */
   /*
  listen(listener,1);
  length = sizeof(peer2);
  if ((conn2=accept(listener, (struct sockaddr *)&peer2, &length2)) < 0) {
    perror("inet_rstream:accept");
    exit(1);
  }
  printsin(&peer2,"RSTREAM::", "accepted connection from"); 

  printf("\n\nRSTREAM:: data from stream:\n");
  while ( read(conn2, &ch, 1) == 1)
    putchar(ch);
  putchar('\n');
   */


  exit(0);
}

void printsin(struct sockaddr_in *sin, char *m1, char *m2 )
{
  char fromip[INET_ADDRSTRLEN];

  printf ("%s %s:\n", m1, m2);
  printf ("  family %d, addr %s, port %d\n", sin -> sin_family,
	    inet_ntop(AF_INET, &(sin->sin_addr.s_addr), fromip, sizeof(fromip)),
            ntohs((unsigned short)(sin -> sin_port)));
}


void startGame(struct Game* game){

  //struct Message* recvmsg_o;//=(struct Message* )malloc(sizeof(struct Message));
  struct Message* sendmsg[2];//=(struct Message* )malloc(sizeof(struct Message));
  struct Message* recvmsg[2];//=(struct Message* )malloc(sizeof(struct Message));
  //struct Message* sendmsg_x;//=(struct Message* )malloc(sizeof(struct Message));
  sendmsg[0]=(struct Message* )malloc(sizeof(struct Message));
  initMsg(sendmsg[0]);
  sendmsg[0]->type=MATCH;
  strncpy(sendmsg[0]->opp_handle,game->opponent[1],strlen(game->opponent[1]));

  sendmsg[0]->xoID=0;
  sendMsg(game->match[0],sendmsg[0]);


  sendmsg[1]=(struct Message* )malloc(sizeof(struct Message));
  initMsg(sendmsg[1]);
  sendmsg[1]->type=MATCH;
  strncpy(sendmsg[1]->opp_handle,game->opponent[0],strlen(game->opponent[0]));

  sendmsg[1]->xoID=1;
  sendMsg(game->match[1],sendmsg[1]);






}
