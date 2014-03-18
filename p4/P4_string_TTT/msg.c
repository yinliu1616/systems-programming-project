
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

void initMsg(struct Message *msg){
    msg->type=WHO; 
    strcpy(msg->handle,"yinliu");
    strcpy(msg->opp_handle,"yishanhe");
    msg->xoID=0;
    msg->move=0;
    msg->result=0;

    strcpy(msg->board,"123456789");
}




void sendMsg(int sock, struct Message *msg){
  char data[MSGLEN];
  memset(data,0,MSGLEN);
  memcpy(data,msg,sizeof(*msg)); 

  send(sock, data,sizeof(data), 0);

  printf("Msg before sending : \n");
  printMsg(msg);
}

void recvMsg(int sock, struct Message *recvmsg,struct Message *sendmsg){
  char data[MSGLEN];
  memset(data,0,MSGLEN);
  recv(sock, data, MSGLEN, 0);
  memset(recvmsg,0,sizeof(*recvmsg));
    memset(sendmsg,0,sizeof(*sendmsg));

  memcpy(recvmsg,data,sizeof(*recvmsg));
  //recvmsg=(struct Message*)data;
  recvmsg->board[10]='/0';
  printf("Msg after receiving : \n");
  printMsg(recvmsg);

  switch(recvmsg->type){
   case WHO:
    //create message to send back
	   printf("Enter your Handle:\n");

     sendmsg->type=HANDLE;
     scanf("%s",sendmsg->handle);
     sendmsg->handle[strlen(sendmsg->handle)]='\0';
     printf("sendmsg->handle[LEN]'s lenth is actually =%d\n",strlen(sendmsg->handle));
     printf("sendmsg->handle=%s\n",sendmsg->handle);
     sendMsg(sock,sendmsg);

	   //printf("sending Msg: \n");
     //printMsg(sendmsg);
  
  
  	break;
   case HANDLE:
    break;
   case MATCH:
    printf("your opponet is %s\n",recvmsg->opp_handle);
    printf("let's start game!\n");

    if (recvmsg->xoID==0){
      printf("you are o\n");
      printBoard(recvmsg->board);
      printf("please input your move\n");
    }else if (recvmsg->xoID==1){
      printf("you are x\n");
      printf("waiting your oppnonet's move\n");
    }
    break;




}
  
	
}
