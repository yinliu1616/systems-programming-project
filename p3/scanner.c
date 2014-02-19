#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define FILENAME 256
#define bufferLen 1024
extern int errno;
int wordProcess(char *);

int main(){
	/*
	if (argc!=2){
		fprintf(stderr,"Invalid Use. Please just input the finename\n");
		exit(1);	
	}
	FILE *curFile=fopen(argv[1],"r");
	*/
	//char c;
	char bufWord[bufferLen];
	printf("enter into the child\n");
	lseek(0,0,SEEK_SET);
	
	while(scanf("%s", bufWord)==1){
		//fprintf(stderr, "fscanf failure.\n");
		//exit(1);
		printf("%s\n",bufWord);
		
		if (wordProcess(bufWord)%2==0){
			if(wordProcess(bufWord)!=0)
				printf("Even: %s\n",bufWord);
		}else{
			printf("Odd: %s\n",bufWord);
		}
		

	}
	
	//read(0,bufWord,10);
	//printf("Kid got char from stdin:%s \n",bufWord);
	printf("after the while loop, about to exit\n");
	
	/*	
	if (errno!=0){
		//fprintf(stderr, "Sorry. 1st argument is too big.\n");
		perror("fscanf failure\n");
		exit(1);
	}	
	*/
	//fclose (curFile);
	//exit(0);
	
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