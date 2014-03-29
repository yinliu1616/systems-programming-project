
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#define PORT 52016
#define HANLELEN 128
#define MSGLEN 1024

typedef enum {
    WHO,       /* sever->client:who?      */
    HANDLE,     /* client->client: my name.   */ 
    MATCH,         /* server->client: match,opp_handle,x/o                     */
    ASK_MOVE, /* server->client: your move? , BOARD */
    MY_MOVE,      /* client->client: my move  */
    RESULT,  /* server->client: result , BOARD */
    SHUTDOWN,
    OPPO_QUIT
}MSG_TYPE;
static char* typeStrings[]= { "WHO", "HANDLE",
                               "MATCH", "ASK_MOVE",
                               "MY_MOVE","RESULT","SHUTDOWN","OPPO_QUIT" };



struct Message{
  MSG_TYPE type;
  char handle[HANLELEN];
  char opp_handle[HANLELEN];
  int xoID;
  int move;
  int result;
  char board[10];

};

struct UDPmsg{
  char handle[HANLELEN];
  char opp_handle[HANLELEN];
  int game_in_process;
  int total_connection;

};



/*
struct ServerStatus{
  int connection[7];
  int someone_waiting;
  int opp1;
  char handle1[LEN];
  int opp2;
  char handle2[LEN];
  char board[9];
  int total_connection;

};
*/


struct Game{
  int match[2];//all the connection the server maintains
  char opponent[2][HANLELEN];//handle of the two opponent
  char board[10];
  int result;
};


/*
struct Game * newgame(int conn0,int conn1,char* oppo0,char *oppo1){
  struct Game *game=(struct Game* )malloc(sizeof(struct Game));
  game->match[0]=conn0;
  game->match[1]=conn1;
  game->opponent[0]=oppo0;
  game->opponent[1]=oppo1;
  int i;
  for (i=0;i<9;i++){
    game->board[i]='1'+i;
  }
  return game;
}
*/
/*

void initGameStatues(struct GameStatus *gamestatus){
  gamestatus->in_progress=0;
  gamestatus->someone_waiting=0;
  gamestatus->opp1=-1;
  //gamestatus->handle1=NULL;
  gamestatus->opp2=-1;
  //gamestatus->handle2=NULL;
  //gamestatus->board=NULL;
  gamestatus->total_connection=0;

}
*/
