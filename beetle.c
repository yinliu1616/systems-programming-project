//beetle.c
//The first argument must be a positive nonzero integer, which specifies the length of one side of the square in inches.
//The second argument must be a positive nonzero integer, which specifies how many beetles are boiled in order to get the average lifetime. 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

extern int errno;

long oneTest(long);

int main (int argc, char *argv[]){
	
	//printf("sizeof(int) = %ld\n sizeof(long) = %ld\n",sizeof(int),sizeof(long));
	//printf("sizeof(char) = %ld\n sizeof(int*) = %ld\n",sizeof(char),sizeof(char*));
	
	//printf("LONG_MAX=%ld\n",LONG_MAX);
	//printf("INT_MAX=%d\n",INT_MAX);
	
	if (argc < 3) {
	        fprintf(stderr, "Sorry. Need two command line arguments.\n");
	        exit(1);
	    }
	if (argc > 3) {
		    fprintf(stderr, "Sorry. Too many command line arguments.\n");
		    exit(1);
		}
		
	int i,k;
	char c;
	for (i=1;i<argc;i++){
		for (k=0;k<strlen(argv[i]);k++){
			c=argv[i][k];
			if (!isdigit(c)){
			    fprintf(stderr, "Sorry. Both arguments must be positive nonzero integers.\n");
			    exit(1);
			}
		}
	}
	/*
	printf("argc=%d\n",argc);
	for (i=0;i<argc;i++){
		printf("argv[%d]=%s\n",i,argv[i]);
	}
	*/
	long square=atol(argv[1]);
	if (errno!=0){
	    fprintf(stderr, "Sorry. 1st argument is too big.\n");
		//perror("1st argument is invalid\n");
	    exit(1);
	}
	long beetleNum=atol(argv[2]);
	if (errno!=0){
	    fprintf(stderr, "Sorry. 2nd argument is too big.\n");
		//perror("2nd argument is invalid\n");
		exit(1);
	}
	//printf("square=%ld\n",square);
	//printf("beetleNum=%ld\n",beetleNum);
	
	if ((square==0)||(beetleNum==0)){
		fprintf(stderr, "Sorry. Both arguments must be positive nonzero integer.\n");
		exit(1);
	}
	
	long sum=0;
	int j=0;
	long newlife;
	for (j=0;j<beetleNum;j++){
		newlife=oneTest(square);
		sum+=newlife;
		//printf("sum =%ld\n", sum);
	}
		
	//printf("sum= %ld\n",sum);

	double averLife=((double)sum)/beetleNum;
	printf("%ld by %ld square, %ld beetles, mean beetle lifetime is  %.1f\n",square,square,beetleNum,averLife);

	return 0;
}



long oneTest(long square){
	double curX=0;
	double curY=0;
	long lifeTime=0;
	double bound=square/2.0;//what if square is odd???
	//printf("%f\n",bound);
	while((fabs(curX)<=bound)&&(fabs(curY)<=bound)){
		lifeTime+=1;
		double theta=M_PI*2*random()/(RAND_MAX+0.0);
		//printf("theta=%f\n",theta);
		double x=cos(theta);
		double y=sin(theta);
		curX+=x;
		curY+=y;
		//printf("abs(curX)=%f,abs(curY)=%f\n",fabs(curX),fabs(curY));
		if ((fabs(curX)>bound)||(fabs(curY)>bound))
			break;
		lifeTime+=1;
	}
	return lifeTime;
}