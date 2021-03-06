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
  int conn[2];//all the connection the server maintains
  char oppo[2][HANLELEN];//handle of the two opponent
  struct Game* game;

//share hostname a file
  if( remove( "hostname.txt" ) != 0 )
    perror( "Error deleting hostname.txt" );
  else
    printf( "File hostname.txt successfully deleted\n" );
  int hostnamefile=open("hostname.txt",O_RDWR|O_CREAT,0644);

  char hostname[1024];
  memset(hostname,0,1024);
  gethostname(hostname, 1023);
  hostname[1023]='\0';

  //printf(hostname);
  //printf("\n");
  write(hostnamefile,hostname,1024);
  close(hostnamefile);
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

  //printsin(localaddr,"localaddr::", "localaddr"); //all 0.0.0.0

  /*
  char host[NI_MAXHOST];
  
  ecode = getnameinfo(localaddr, sizeof(*localaddr),
                          host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
  if (ecode) {
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(ecode));
    exit(1);
  }

  printf("Hostname: %s\n", host); 
  */

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
        initGame(game);
        game->match[0]=conn[0];
        game->match[1]=conn[1];
        strcpy(game->opponent[0],oppo[0]);
        strcpy(game->opponent[1],oppo[1]);
        /*
        int i;
        for (i=0;i<9;i++){
        game->board[i]='1'+i;
        }
        */
        //game->result=-1; 
        startGame(game);
        initGame(game);
        memset(oppo[0],0,HANLELEN);
        memset(oppo[1],0,HANLELEN);
        total_conn=0;
    }

  }
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
  sendmsg[1]=(struct Message* )malloc(sizeof(struct Message));
  recvmsg[0]=(struct Message* )malloc(sizeof(struct Message));
  recvmsg[1]=(struct Message* )malloc(sizeof(struct Message));

  memset(sendmsg[0],0,sizeof(*sendmsg));
  memset(sendmsg[1],0,sizeof(*sendmsg));

  sendmsg[0]->type=MATCH;
  strcpy(sendmsg[0]->handle,game->opponent[0]);
  strcpy(sendmsg[0]->opp_handle,game->opponent[1]);
  sendmsg[0]->xoID=0;
  sendMsg(game->match[0],sendmsg[0]);
  
  sendmsg[1]->type=MATCH;
  strcpy(sendmsg[1]->handle,game->opponent[1]);
  strcpy(sendmsg[1]->opp_handle,game->opponent[0]);
  sendmsg[1]->xoID=1;
  sendMsg(game->match[1],sendmsg[1]);

  int i=0;
  int pos;
  while(1){
    i=i%2;
    memset(sendmsg[i],0,sizeof(*sendmsg));
    sendmsg[i]->type=ASK_MOVE;
    sendmsg[i]->xoID=i;
    strcpy(sendmsg[i]->opp_handle,game->opponent[(i+1)%2]);

    strcpy(sendmsg[i]->handle,game->opponent[i]);
    strcpy(sendmsg[i]->board,game->board);
    sendMsg(game->match[i],sendmsg[i]);

    pos=-1;
    recvMsg(game->match[i],recvmsg[i],sendmsg[i]);
    if (recvmsg[i]->type==MY_MOVE){
        printf("received MOVE from player %d\n",i);
        pos=recvmsg[i]->move-1;
        if (i==0){
          game->board[pos]='o';
          if (checkBoard(game->board,'o')==1){
              printf("o win!!!\n");
              game->result=0;
              break;
          }else if (checkBoard(game->board,'o')==-1){
              printf("A draw!!!\n");
              game->result=-1;
              break;
          }






        }else if (i==1){
          game->board[pos]='x';
          if (checkBoard(game->board,'x')==1){
              printf("x win!!!\n");
              game->result=1;
              break;
          }else if (checkBoard(game->board,'x')==-1){
              printf("A draw!!!\n");
              game->result=-1;
              break;
          }
        }
    }

    

    i++;

  }

  sendmsg[0]->type=RESULT;
  strcpy(sendmsg[0]->handle,game->opponent[0]);
  strcpy(sendmsg[0]->opp_handle,game->opponent[1]);

  sendmsg[1]->type=RESULT;
  strcpy(sendmsg[1]->handle,game->opponent[1]);
  strcpy(sendmsg[1]->opp_handle,game->opponent[0]);

  if (game->result==0){
    sendmsg[0]->result=1;
    sendmsg[1]->result=0;

  }else if (game->result==1){
    sendmsg[0]->result=0;
    sendmsg[1]->result=1;

  }else if (game->result==-1){
    sendmsg[0]->result=-1;
    sendmsg[1]->result=-1;
  }
  
  sendMsg(game->match[0],sendmsg[0]);
  
  sendMsg(game->match[1],sendmsg[1]);






}
