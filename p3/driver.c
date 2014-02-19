#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#define bufferLen 1024

int main(int argc, char *argv[]){
	//FILE *curFile;
	int driver2scan[2],scan2even[2],even2scan[2],odd2scan[2],scan2odd[2];
	int scanner,even,odd;
	int curfd;
	char c;
	char bufWord[bufferLen];
	
	if (argc!=2){
		fprintf(stderr,"Invalid Use. Please just input the finename\n");
		exit(1);	
	}
	if((curfd=open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"Cannot open %s\n",argv[1]);
		exit(1);
	}
	/*
	read(curfd,bufWord,100);
	bufWord[100]='\0';
	printf("Parent got char from curfd:%s \n",bufWord);
	*/
	
	if((scanner=fork())==0){
		//close(0);dup(driver2scan[0]);close(driver2scan[0]);close(driver2scan[1]);
		close(0);dup2(curfd,0);close(curfd);
		execl("./scanner","scanner",(char*)NULL);
		fprintf(stderr,"Unable to exec\n");
	}
	
	/*
	close(driver2scan[0]);
	while (read(curfd,&c,sizeof(char)!=0)){
		write(driver2scan[1],&c,sizeof(char));
	}
	close(driver2scan[1]);
	*/
	close(curfd);
	
	wait(NULL);
	printf("Parent is after fork/exec\n");
	

	return 0;
}