#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define BUFFSIZE 1024
#define FILENAME 256


struct sayNode {
	char fileName[FILENAME];
	int lineNo;
	char sayContent[BUFFSIZE];
	struct sayNode* next;
};

void sayFind(struct sayNode *head);
int delDup(struct sayNode *head);//delete duplicate and return real num of sayings

void printSay(struct sayNode *head,int hflag,char* pattern);



int main(int argc, char**argv){
		int argCount=1;			// the number of arguments 
						// for a particular command
		int bflag=0;
		int hflag=0;
		
		char* pattern;
		//printf("\e[7mReversed\e[0m Normal\n");
		
		for (argc--,argv++;argc>0;argc -= argCount, argv += argCount){
			if (!strcmp(*argv, "-b")){
				printf ("accept \" -b\" \n");
				bflag=1;
			}               
			else if (!strcmp(*argv, "-h")) {	
			    printf ("accept \" -h\" \n");
				hflag=1;
			}else{
				pattern=*argv;
			}
		}

		printf("pattern=%s\n",pattern);
		
		struct sayNode *newNode;
		struct sayNode *now=NULL;
		struct sayNode *head=NULL;
		
		char lineNum[FILENAME];
		
		char buf[BUFFSIZE];
		int result=0;
		while((fgets(buf,BUFFSIZE,stdin))!=NULL){
			//printf("fgets in buf\n%s\n",buf);
			if((newNode=(struct sayNode *)malloc(sizeof(struct sayNode)))==NULL){
				fprintf(stderr,"nodeList:malloc failure for new sayNode\n");
				exit(1);
			}
			newNode->next=NULL;
			
			sscanf(buf,"%[^:]:%[^:]",newNode->fileName,lineNum);
			newNode->lineNo=atoi(lineNum);
			//printf("newNode->fileName=%s,newNode->lineNo=%d\n",newNode->fileName,newNode->lineNo);

			if(now!=NULL){
				now->next=newNode;
			}else{
				head=newNode;
			}
			now=newNode;
		}
		
		sayFind(head);
		result=delDup(head);
		if(bflag){
			printf("%d witty sayings contain the string %s\n",result,pattern);
		}
		printSay(head,hflag,pattern);
		
		
		struct sayNode *current=head;
		struct sayNode *next;	
			
		while(current){
			next=current->next;
			free(current);
			current=next;
		}
			
		return 0;
}



void sayFind(struct sayNode *head){
	struct sayNode *current=head;
	char curFileName[FILENAME];
	
	FILE *currentFile=NULL;
	FILE *nextFile;
	
	int currentLine;
	char buf[BUFFSIZE];
	char tmpstr[BUFFSIZE];
	
	

	while(current){
		//printf("current->fileName=%s,current->lineNo=%d\n",current->fileName,current->lineNo);
		if(strcmp(curFileName,current->fileName)!=0){
			nextFile = fopen(current->fileName,"r");
			if (nextFile == NULL) {
			perror ("Error opening file");
			exit(1);
			}
			if (currentFile!=nextFile){
				if(currentFile==NULL){
					currentFile=nextFile;
					strcpy(curFileName,current->fileName);
				}
					
				else{
					fclose(currentFile);
					currentFile=nextFile;
					strcpy(curFileName,current->fileName);
				}
			}
		}
		//printf("currently handle file %s\n",curFileName);
		fseek ( currentFile , 0 , SEEK_SET );
		currentLine=1;
		tmpstr[0]='\0';
		while((fgets(buf,BUFFSIZE,currentFile))!=NULL){
			if(strcmp(buf,"%\n")==0){
				if(currentLine>current->lineNo){
					break;
				}else{
					tmpstr[0]='\0';
				}
			}else{
				strcat(tmpstr,buf);
			}
			currentLine++;
		}
		strcpy(current->sayContent,tmpstr);
		
		current=current->next;
	}
	
	if (currentFile){
		fclose(currentFile);
	}
	
	
}

int delDup(struct sayNode *head){//delete duplicate and return real num of sayings
	struct sayNode *current=head;
	struct sayNode *nextToDel=NULL;
	int num=0;
	while(current){
		if((current->next!=0)&&(strcmp(current->sayContent,current->next->sayContent)==0)){
			nextToDel=current->next;
			current->next=current->next->next;
			free(nextToDel);
			continue;
		}
		num++;
		current=current->next;
	}
	return num;	
}


void printSay(struct sayNode *head,int hflag,char* pattern){
	struct sayNode *current=head;
	char new[BUFFSIZE];
	char left[BUFFSIZE];
	char tmp[BUFFSIZE];
	
	char* subPtr=NULL;
	
	char replace[BUFFSIZE];
	replace[0]='\0'; 
	printf("%s\n",pattern);
	strcpy(replace,"\e[7m");
	strcat(replace,pattern);
	strcat(replace,"\e[0m");
	printf("%s\n",replace);
	
	
	int oldSubLen=strlen(pattern);
	
	
	
	
	if(hflag){
		
		while(current){
			if (current!=head)
				printf("-------------------------------------\n");
			new[0]='\0';
			strcpy(left,current->sayContent);
			while((subPtr=strstr(left,pattern))!=NULL){
				strncat(new,left,subPtr-left);
				strcat(new,replace);
				strcpy(tmp,subPtr+oldSubLen);
				strcpy(left,tmp);
			}
			strcat(new,left);
			printf("%s",new);
			current=current->next;
			new[0]='\0';
		}	
	}else{
		while(current){
			if (current!=head)
				printf("-------------------------------------\n");
			printf("%s",current->sayContent);
			current=current->next;
		}	
	}
}


