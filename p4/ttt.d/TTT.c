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
  int listener,udplistener,socket_fd;  /* fd for socket on which we get connection requests */
  //int conn,conn2;      /* fd for socket thru which we pass data */
  struct sockaddr_in *localaddr,*udplocaladdr, peer,udppeer;


  int ecode,hits;
  socklen_t length;
  char ch;
  struct addrinfo hints, udphints, *addrlist, *udpaddrlist;

  fd_set mask;
  struct timeval timeout;


  //struct GameStatus *gamestatus;
  //initGameStatues(gamestatus);
  int total_conn=0;//total number of 
  int conn[7];//all the connection the server maintains
  char oppo[2][HANLELEN];//handle of the two opponent
  struct Game* game;
  int game_in_process=0;
  int handle_received=0;

//*************************************************share hostname a file
  /*
  if( remove( "hostname.txt" ) != 0 )
    perror( "Error deleting hostname.txt" );
  else
    printf( "File hostname.txt successfully deleted\n" );
    */
  remove( "hostname.txt" );
  int hostnamefile=open("hostname.txt",O_RDWR|O_CREAT,0644);

  char hostname[1024];
  memset(hostname,0,1024);
  gethostname(hostname, 1023);
  hostname[1023]='\0';

  //printf(hostname);
  //printf("\n");
  write(hostnamefile,hostname,1024);
  close(hostnamefile);
//*************************************************End //share hostname a file


/* 
   Want to specify local server address of:
      addressing family: AF_INET
      ip address:        any interface on this system 
      port:              0 => system will pick free port
*/
//create an TCP socket, and bind to addr*************************************************
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
   Create socket on which we will accept connections. This is NOT the
   same as the socket on which we pass data.
*/
  if ( (listener = socket( addrlist->ai_family, addrlist->ai_socktype, 0 )) < 0 ) {
    perror("liu_TTT TCP:socket");
    exit(1);
  }


  if (bind(listener, (struct sockaddr *)localaddr, sizeof(struct sockaddr_in)) < 0) {
    perror("liu_TTT TCP:bind");
    exit(1);
  }

  /*
   Print out the port number assigned to this process by bind().
*/
  length = sizeof(struct sockaddr_in);
  if (getsockname(listener, (struct sockaddr *)localaddr, &length) < 0) {
    perror("liu_TTT TCP:getsockname");
    exit(1);
  }
  printf("liu_TTT TCP:: port number %d\n", ntohs(localaddr->sin_port));
//*************************************************End//create an TCP socket, and bind to addr

//create an UDP socket, and bind to addr*************************************************
  memset(&udphints, 0, sizeof(udphints));
  udphints.ai_family = AF_INET; udphints.ai_socktype = SOCK_DGRAM;
  udphints.ai_flags = AI_NUMERICSERV | AI_PASSIVE; udphints.ai_protocol = 0;
  udphints.ai_canonname = NULL; udphints.ai_addr = NULL;
  udphints.ai_next = NULL;

  ecode = getaddrinfo(NULL, "52116", &udphints, &udpaddrlist);
  if (ecode != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ecode));
    exit(1);
  }

  udplocaladdr = (struct sockaddr_in *) udpaddrlist->ai_addr;

  //printsin(udplocaladdr, "liu_TTT UDP", "Local socket is:"); fflush(stdout);
  printf ("liu_TTT UDP:: port number %d\n",ntohs((unsigned short)(udplocaladdr -> sin_port)));

  udplistener = socket (udpaddrlist->ai_family, udpaddrlist->ai_socktype, 0);
  if (udplistener < 0) {
    perror ("iu_TTT UDP:socket");
    exit (1);
  }
  /*
   bind port 0x3333 on the current host to the socket accessed through
   socket_fd. If port in use, die.
*/
  if (bind(udplistener, (struct sockaddr *)udplocaladdr, sizeof(struct sockaddr_in)) < 0) {
    perror("liu_TTT UDP:bind");
    exit(1);
  }



//*************************************************End//create an UDP socket, and bind to addr

/*
   Now we have 
   Tcp socket listener
   udp socket  udplistener
*/
   //**************************************
   length = sizeof(peer);
  socket_fd=udplistener;
  struct UDPmsg *querymsg;

  while (1){
//select set
    FD_ZERO(&mask);
    FD_SET(listener,&mask);
    FD_SET(udplistener,&mask);

    int k=0;
    for (k=0;k<total_conn;k++){
      FD_SET(conn[k],&mask);

    }

    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
  if ((hits = select(socket_fd+1, &mask, (fd_set *)0, (fd_set *)0,
                           NULL)) < 0) {
      perror("liu_TTT:select");
      exit(1);
    }

 //
    if ( (hits>0) && (FD_ISSET(udplistener,&mask))) {
        querymsg=(struct UDPmsg* )malloc(sizeof(struct UDPmsg));
        ecode = recvfrom(udplistener,querymsg,sizeof(*querymsg),0,(struct sockaddr *)&udppeer,&length);
        if (ecode < 0) perror("TTT_udp:recvfrom");
        printsin( &udppeer, "TTT_udp: ", "receive query message from:");
        fflush(stdout);
        memset(querymsg,0,sizeof(*querymsg));

        querymsg->game_in_process=game_in_process;
        querymsg->total_connection=total_conn;
        strcpy(querymsg->handle,oppo[0]);
        strcpy(querymsg->opp_handle,oppo[1]);

        ecode = sendto(udplistener,querymsg,sizeof(*querymsg),0,(struct sockaddr *) &udppeer,sizeof(struct sockaddr_in));
        if (ecode < 0) perror("send_udp:sendto");
        continue;
    } 
    if ( (hits>0) && (FD_ISSET(listener,&mask))){
        listen(listener,1);
        if ((conn[total_conn++]=accept(listener, (struct sockaddr *)&peer, &length)) < 0) {
            perror("liu_TTT:accept");
            exit(1);
        }
        printsin(&peer,"liu_TTT::", "accepted connection from"); 
        if ((total_conn<3)&&(game_in_process==0)){
            struct Message* whoMsg=(struct Message* )malloc(sizeof(struct Message));
            whoMsg->type=WHO; 
            sendMsg(conn[total_conn-1],whoMsg);
        }
        if (total_conn==7){
            
            struct Message* shutdownMsg=(struct Message* )malloc(sizeof(struct Message));
            shutdownMsg->type=SHUTDOWN; 
            sendMsg(conn[6],shutdownMsg);
            total_conn--;
            close(conn[6]);
        }
        socket_fd=conn[total_conn-1];
        printf("total connection is %d\n", total_conn);
        
        continue;
    }
    //if ( (hits>0) && ((FD_ISSET(conn[0],&mask))||(FD_ISSET(conn[1],&mask))) ){

        int retval;
        struct Message* recvmsg=(struct Message* )malloc(sizeof(struct Message));
        struct Message* sendmsg=(struct Message* )malloc(sizeof(struct Message));
        int player;
        if (FD_ISSET(conn[0],&mask)){
            player=0;

        }else if (FD_ISSET(conn[1],&mask)){
            player=1;

        }
        retval=recvMsg(conn[player],recvmsg,sendmsg);
        if (retval==-1){
              printf("somebody disconnect\n");
              initGame(game);
              memset(oppo[0],0,HANLELEN);
              memset(oppo[1],0,HANLELEN);

              struct Message* oppoquitMsg=(struct Message* )malloc(sizeof(struct Message));
              oppoquitMsg->type=OPPO_QUIT; 
              sendMsg(conn[(player+1)%2],oppoquitMsg);
              close(conn[0]);
              close(conn[1]);
              int var=0;
              for (var=2;var<total_conn;var++){
                conn[var-2]=conn[var];
              }


              total_conn=total_conn-2;
              game_in_process=0;
              handle_received=0;
              int min=total_conn>2?2:total_conn;
              for (var=0;var<min;var++){
                struct Message* whoMsg=(struct Message* )malloc(sizeof(struct Message));
                whoMsg->type=WHO; 
                sendMsg(conn[var],whoMsg);

              }
        continue;

        }
//********************************************************* HANDLE
        if (recvmsg->type==HANDLE){
        //printf("received HANDLE: recvmsg->handle:%s, length=%d\n",recvmsg->handle,strlen(recvmsg->handle));
        strcpy(oppo[player],recvmsg->handle);
        printf("The oppo %d's handle is %s\n",player,oppo[player]);
        handle_received++;
        if (handle_received==2){
          game_in_process=1;
          printf("there is a match:\n");
          printf("the oppo %d's handle is %s\n,the oppo %d's handle is %s\n", 0,oppo[0],1,oppo[1]);
          game=(struct Game* )malloc(sizeof(struct Game));
          initGame(game);
          game->match[0]=conn[0];
          game->match[1]=conn[1];
          strcpy(game->opponent[0],oppo[0]);
          strcpy(game->opponent[1],oppo[1]);

//send both player there is a match
          int i=0;
          for (i=0;i<2;i++){
            memset(sendmsg,0,sizeof(*sendmsg));
            sendmsg->type=MATCH;
            strcpy(sendmsg->handle,game->opponent[i]);
            strcpy(sendmsg->opp_handle,game->opponent[(i+1)%2]);
            sendmsg->xoID=i;
            sendMsg(game->match[i],sendmsg);

          }
//ask player o to move
          memset(sendmsg,0,sizeof(*sendmsg));
          sendmsg->type=ASK_MOVE;
          sendmsg->xoID=0;
          strcpy(sendmsg->opp_handle,game->opponent[1]);
          strcpy(sendmsg->handle,game->opponent[0]);
          strcpy(sendmsg->board,game->board);
          sendMsg(game->match[0],sendmsg);
          }//end of handle_received==2
          }//end of HANDLE
//********************************************************* MY_MOVE
        if (recvmsg->type==MY_MOVE){
          printf("received MOVE from player %d\n",player);
          int pos=recvmsg->move-1;
        if (player==0){
          game->board[pos]='o';
          if (checkBoard(game->board,'o')==1){
              printf("o win!!!\n");
              game->result=0;
              //break;//cannot break, need end this game
              game_in_process=0;

              


          }else if (checkBoard(game->board,'o')==-1){
              printf("A draw!!!\n");
              game->result=-1;
              game_in_process=0;
              //break;
          }
        }else if (player==1){
          game->board[pos]='x';
          if (checkBoard(game->board,'x')==1){
              printf("x win!!!\n");
              game->result=1;
              game_in_process=0;
              //break;
          }else if (checkBoard(game->board,'x')==-1){
              printf("A draw!!!\n");
              game->result=-1;
              game_in_process=0;
              //break;
          }
        }

        if (game_in_process==0){
              struct Message* sendmsg[2];//=(struct Message* )malloc(sizeof(struct Message));
              struct Message* recvmsg[2];//=(struct Message* )malloc(sizeof(struct Message));
              //struct Message* sendmsg_x;//=(struct Message* )malloc(sizeof(struct Message));
              sendmsg[0]=(struct Message* )malloc(sizeof(struct Message));
              sendmsg[1]=(struct Message* )malloc(sizeof(struct Message));
              recvmsg[0]=(struct Message* )malloc(sizeof(struct Message));
              recvmsg[1]=(struct Message* )malloc(sizeof(struct Message));

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

              initGame(game);
              memset(oppo[0],0,HANLELEN);
              memset(oppo[1],0,HANLELEN);
              close(conn[0]);
              close(conn[1]);
              int var=0;
              for (var=2;var<total_conn;var++){
                conn[var-2]=conn[var];
              }


              total_conn=total_conn-2;
              game_in_process=0;
              handle_received=0;
              int min=total_conn>2?2:total_conn;
              for (var=0;var<min;var++){
                struct Message* whoMsg=(struct Message* )malloc(sizeof(struct Message));
                whoMsg->type=WHO; 
                sendMsg(conn[var],whoMsg);

              }
              
        }else{
            memset(sendmsg,0,sizeof(*sendmsg));
            sendmsg->type=ASK_MOVE;
            sendmsg->xoID=(player+1)%2;
            strcpy(sendmsg->opp_handle,game->opponent[player]);
            strcpy(sendmsg->handle,game->opponent[(player+1)%2]);
            strcpy(sendmsg->board,game->board);
            sendMsg(game->match[(player+1)%2],sendmsg);


        }




        }
        //continue;


    //}

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
