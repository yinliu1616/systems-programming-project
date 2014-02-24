//#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>


#define bufferLen 1024
extern int errno;
int main(int argc, char *argv[]){
	
	int scan2even[2],even2scan[2],odd2scan[2],scan2odd[2];
	int scanner,even,odd;
	int curfd;
	char c;
	char bufWord[bufferLen];
	char s2e_w[10],e2s_r[10],s2o_w[10],o2s_r[10],scannerID[10];
	
	pipe(scan2even);
	pipe(even2scan);
	pipe(scan2odd);
	pipe(odd2scan);
	printf("after pipe(), scan2even[0]=%d,scan2even[1]=%d\n",scan2even[0],scan2even[1]);
	printf("after pipe(), even2scan[0]=%d,even2scan[1]=%d\n",even2scan[0],even2scan[1]);
	printf("after pipe(), scan2odd[0]=%d,scan2odd[1]=%d\n",scan2odd[0],scan2odd[1]);
	printf("after pipe(), odd2scan[0]=%d,odd2scan[1]=%d\n",odd2scan[0],odd2scan[1]);
	
	
	if (argc!=2){
		fprintf(stderr,"Invalid Use. Please just input the finename\n");
		exit(1);	
	}
	if((curfd=open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"Cannot open %s\n",argv[1]);
		exit(1);
	}
	
	
	
	
	
	if((scanner=fork())==0){
		close(0);dup2(curfd,0);close(curfd);
		close(scan2even[0]);
		close(even2scan[1]);
		sprintf(s2e_w,"%d",scan2even[1]);
		sprintf(e2s_r,"%d",even2scan[0]);
		close(scan2odd[0]);
		close(odd2scan[1]);
		sprintf(s2o_w,"%d",scan2odd[1]);
		sprintf(o2s_r,"%d",odd2scan[0]);
		
		
		printf("after sprintf(), s2e_w=%s,e2s_r=%s\n",s2e_w,e2s_r);
		printf("after sprintf(), s2o_w=%s,o2s_r=%s\n",s2o_w,o2s_r);
		
		
		execl("./scanner","scanner",s2e_w,e2s_r,s2o_w,o2s_r,(char*)NULL);
		fprintf(stderr,"Unable to exec scanner\n");
	}
	printf("scannerID=%d\n",scanner);
	sprintf(scannerID,"%d",scanner);
	
	
	if((even=fork())==0){
		printf("enter into new child process even\n");
		
		close(curfd);
		close(scan2odd[0]);close(scan2odd[1]);close(odd2scan[0]);close(odd2scan[1]);
		close(0);dup2(scan2even[0],0);close(scan2even[0]);close(scan2even[1]);
		close(1);dup2(even2scan[1],1);close(even2scan[1]);close(even2scan[0]);
		printf("enter into new child process even,about to exec\n");
		execl("./evenodd","even",scannerID,(char*)NULL);
		printf("Unable to exec even");
	}
	
	if((odd=fork())==0){
		printf("enter into new child process odd\n");
		
		close(curfd);
		close(scan2even[0]);close(scan2even[1]);close(even2scan[0]);close(even2scan[1]);
		
		close(0);dup2(scan2odd[0],0);close(scan2odd[0]);close(scan2odd[1]);
		close(1);dup2(odd2scan[1],1);close(odd2scan[1]);close(odd2scan[0]);
		printf("enter into new child process odd,about to exec\n");
		execl("./evenodd","odd",scannerID,(char*)NULL);
		printf("Unable to exec odd");
	}
	printf(" even PID=%d,odd PID=%d\n ",even,odd);
	
	close(curfd);
	close(scan2even[0]);close(scan2even[1]);
	close(even2scan[1]);close(even2scan[0]);
	close(scan2odd[0]);close(scan2odd[1]);
	close(odd2scan[1]);close(odd2scan[0]);
	/*
	wait(NULL);
	wait(NULL);
	wait(NULL);
	*/
	int pid;
	while (pid = waitpid(-1, NULL, 0)) {
	   if (errno == ECHILD) {
	      break;
	   }
	}
	
	printf("driver is after fork/exec\n");
	

	return 0;
}