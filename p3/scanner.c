#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>





#define FILENAME 256
#define bufferLen 1024
extern int errno;
int wordProcess(char *);

void usr1handler(){
	char ch;
	printf("\n Handling signal:Words with even letters:\n");
	while (read(5,&ch,1)==1){
		printf("%c",ch);
	}
	printf("Handling signal:Words with odd letters:\n");
	while (read(9,&ch,1)==1){
		printf("%c",ch);
	}
	exit(0);
}



int main(int argc, char* argv[]){
	signal(SIGTERM,usr1handler);
	
	printf("enter the scanner\n");
	/*
	if (argc!=2){
		fprintf(stderr,"Invalid Use. Please just input the finename\n");
		exit(1);	
	}
	FILE *curFile=fopen(argv[1],"r");
	*/
	//char c;
	int s2e_w=atoi(argv[1]);
	int e2s_r=atoi(argv[2]);
	int s2o_w=atoi(argv[3]);
	int o2s_r=atoi(argv[4]);
	printf("scanner:Successfully accept arg s2e_w=%d,e2s_r=%d \n",s2e_w,e2s_r);
	printf("scanner:Successfully accept arg s2o_w=%d,o2s_r=%d \n",s2o_w,o2s_r);
	
	
	
	
	char bufWord[bufferLen];
	
	//lseek(0,0,SEEK_SET);
	
	while(scanf("%s", bufWord)==1){
		//fprintf(stderr, "fscanf failure.\n");
		//exit(1);
		//printf("%s\n",bufWord);
		
		if (wordProcess(bufWord)%2==0){
			if(wordProcess(bufWord)!=0){
				//printf("Even: %s\n",bufWord);
				write(s2e_w,bufWord,strlen(bufWord));
				write(s2e_w," ",1);
			}
				
		}else{
			//printf("Odd: %s\n",bufWord);
			write(s2o_w,bufWord,strlen(bufWord));
			write(s2o_w," ",1);
		}
		

	}
	close(s2e_w);
	close(s2o_w);
	
	printf("scanner:after the while loop, before printing ****\n");

	
	while(1){
		sleep(1);
		printf("*");
	}
	
	
	//read(0,bufWord,10);
	//printf("Kid got char from stdin:%s \n",bufWord);
	
	/*	
	if (errno!=0){
		//fprintf(stderr, "Sorry. 1st argument is too big.\n");
		perror("fscanf failure\n");
		exit(1);
	}	
	*/
	//fclose (curFile);
	exit(0);
	
}

int wordProcess(char *bufWord){
	int len=strlen(bufWord);
	int i,j;
	for(i=0,j=0;i<len;){
		if(ispunct(bufWord[i])){
			i++;
		}else{
			bufWord[j++]=tolower(bufWord[i++]);
		}
			
	}
	bufWord[j]='\0';
	return strlen(bufWord);
}