
#include "common.h"

void printMsg(struct Message *msg){
  printf("------------\n");
  printf("type:%s\n",typeStrings[msg->type]);
  printf("handle: %s\n",msg->handle);
  printf("opp_handle: %s\n",msg->opp_handle);
  printf("xoID: %d\n",msg->xoID);
  printf("move: %d\n",msg->move);
  printf("board: %s\n",msg->board);
  printf("result: %d\n",msg->result);
  printf("------------\n");
  
}

void printBoard(char board[]){
  int i,j;
  printf("board:\n");
  for (i=0;i<3;i++){
    for (j=0;j<3;j++){
      putchar(board[i*3+j]);
      putchar(' ');

    }
  putchar('\n');
 }
}

int checkBoard(char board[],char role){
    
    int i;
    for(i=0;i<3;i++){
      if((board[i*3+0]==role)&&(board[i*3+1]==role)&&(board[i*3+2]==role))
        return 1;
      if((board[i+0]==role)&&(board[i+3]==role)&&(board[i+6]==role))
        return 1;
    }
    if((board[0]==role)&&(board[4]==role)&&(board[8]==role))
        return 1;
    if((board[2]==role)&&(board[4]==role)&&(board[6]==role))
        return 1;

    int j;
    for (j=0;j<9;j++){
        if (isdigit(board[j]))
            return 0;
    }

    return -1;

}

void initMsg(struct Message *msg){
    msg->type=WHO; 
    strcpy(msg->handle,"yinliu");
    strcpy(msg->opp_handle,"yishanhe");
    msg->xoID=0;
    msg->move=0;
    msg->result=0;

    strcpy(msg->board,"123456789");
}

void initGame(struct Game* game){
    int i;
    for (i=0;i<9;i++){
      game->board[i]='1'+i;
    }
    game->result=-1;  
}


int sendMsg(int sock, struct Message *msg){
  char data[MSGLEN];
  memset(data,0,MSGLEN);
  memcpy(data,msg,sizeof(*msg)); 

  //send(sock, data,sizeof(data), 0);
  int retval;
  retval=write(sock, &data, MSGLEN);
  if (retval!=MSGLEN)
      return -1;
  //printf("Msg before sending : \n");
  //printMsg(msg);
  return 0;
}

int recvMsg(int sock, struct Message *recvmsg,struct Message *sendmsg){
  char data[MSGLEN];
  memset(data,0,MSGLEN);
  //recv(sock, data, MSGLEN, 0);
  int retval;
  retval=read(sock, &data, MSGLEN);
  if (retval!=MSGLEN){
    //printf("somebody disconnect\n");
      return -1;
  }
      


  memset(recvmsg,0,sizeof(*recvmsg));
    memset(sendmsg,0,sizeof(*sendmsg));

  memcpy(recvmsg,data,sizeof(*recvmsg));
  //recvmsg=(struct Message*)data;
  recvmsg->board[10]='/0';
  //printf("Msg after receiving : \n");
  //printMsg(recvmsg);

  switch(recvmsg->type){
   case WHO:
    //create message to send back
	   printf("Enter your Handle:\n");

     sendmsg->type=HANDLE;
     scanf("%s",sendmsg->handle);
     sendmsg->handle[strlen(sendmsg->handle)]='\0';
     //printf("sendmsg->handle[LEN]'s lenth is actually =%d\n",strlen(sendmsg->handle));
     //printf("sendmsg->handle=%s\n",sendmsg->handle);
     sendMsg(sock,sendmsg);
    printf("waiting !!!\n");


  	break;
   case HANDLE:
    break;
   case MATCH:
    printf("your opponet is %s\n",recvmsg->opp_handle);
    printf("let's start game!\n");

    if (recvmsg->xoID==0){
      printf("you are o\n");
      printf("waiting for input request\n");


    }else if (recvmsg->xoID==1){
      printf("you are x\n");
      printf("waiting for input request\n");
    }
    break;
    case ASK_MOVE:
      if (recvmsg->xoID==0){
      printf("you are o\n");
      
      printf("please input the position according to remaining numbers in the board\n");
      printBoard(recvmsg->board);

      scanf ("%d",&(sendmsg->move));
      while ((sendmsg->move>10)||(sendmsg->move<0)||(recvmsg->board[sendmsg->move-1]=='o')||(recvmsg->board[sendmsg->move-1]=='x')){
        printf("invalid input \n");

        printf("please input the position according to remaining numbers in the board\n");
        printBoard(recvmsg->board);
        scanf ("%d",&(sendmsg->move));

      }


      sendmsg->type=MY_MOVE;
      printf("your move has been sent, waiting for opponet's move\n");
      sendMsg(sock,sendmsg);


    }else if (recvmsg->xoID==1){
      printf("you are x\n");
      printf("please input the position according to remaining numbers in the board\n");

      printBoard(recvmsg->board);
      scanf ("%d",&(sendmsg->move));
      while ((sendmsg->move>10)||(sendmsg->move<0)||(recvmsg->board[sendmsg->move-1]=='o')||(recvmsg->board[sendmsg->move-1]=='x')){
        printf("invalid input \n");

        printf("please input the position according to remaining numbers in the board\n");
        printBoard(recvmsg->board);
        scanf ("%d",&(sendmsg->move));

      }


      sendmsg->type=MY_MOVE;
      printf("your move has been sent, waiting for opponet's move\n");

      sendMsg(sock,sendmsg);
    }
    break;
    case RESULT:
      if (recvmsg->result==0){
        printf("you lose!\n");
      }else if (recvmsg->result==1){
        printf("you win!\n");
      }else if (recvmsg->result==-1){
        printf("A draw!\n");
      }
    break;
    case SHUTDOWN:
    break;

}
  return 0;
	
}
