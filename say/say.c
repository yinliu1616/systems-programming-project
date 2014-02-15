#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define BUFFSIZE 2048
#define FILENAME 1024


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
		int k;
		char c;
		
		char* pattern=NULL;
		//printf("length is %d\n",strlen(""));
		
		for (argc--,argv++;argc>0;argc -= argCount, argv += argCount){
			if (!strcmp(*argv, "-b")){
				//printf ("accept \" -b\" \n");
				bflag=1;
			}               
			else if (!strcmp(*argv, "-h")) {	
			    //printf ("accept \" -h\" \n");
				hflag=1;
			}else{
				pattern=*argv;
			}
		}
		if(pattern==NULL){
			fprintf(stderr,"Please input searchstring\n");
			exit(1);
			
		}

		//printf("pattern=%s\n",pattern);
		
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
			newNode->fileName[0]='\0';
			newNode->lineNo=0;
			newNode->next=NULL;
			if(strchr(buf,':')==NULL){
				continue;
			}
			sscanf(buf,"%[^:]:%[^:]",newNode->fileName,lineNum);
			
			for (k=0;k<strlen(lineNum);k++){
				c=lineNum[k];
				if (!isdigit(c)){
				    fprintf(stderr, "Grep output contains line numbers which are non-numeric\n");
				    exit(1);
				}
			}
			
			newNode->lineNo=atoi(lineNum);
			if (errno!=0){
			    fprintf(stderr, "Grep output contains invalid line numbers.\n");
				//perror("1st argument is invalid\n");
			    exit(1);
			}
			//printf("%s:%d\n",newNode->fileName,newNode->lineNo);

			if(now!=NULL){
				now->next=newNode;
			}else{
				head=newNode;
			}
			now=newNode;
		}
		
		sayFind(head);
	
		if(strstr(head->sayContent,pattern)==NULL){
			fprintf(stderr,"mismatched search strings\n");
			exit(1);
		}
	
		result=delDup(head);
		if(bflag){
			printf("\n%d witty sayings contain the string %s\n\n",result,pattern);
		}
		printSay(head,hflag,pattern);
		
		
		struct sayNode *current=head;
		struct sayNode *next;	
			
		while(current){
			next=current->next;
			free(current);
			current=next;
		}
		/*
		if(bflag){
			printf("%d witty sayings contain the string %s\n",result,pattern);
		}
		*/	
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
	int tmplen;
	
	

	while(current){
		//printf("%s:%d\n",current->fileName,current->lineNo);
		currentFile = fopen(current->fileName,"r");
		/*
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
		*/
		//printf("currently handle file %s\n",curFileName);
		fseek ( currentFile , 0 , SEEK_SET );
		currentLine=1;
		tmpstr[0]='\0';
		buf[0]='\0';

		while((fgets(buf,BUFFSIZE,currentFile))!=NULL){
			//printf("%s",buf);
			//buf[strlen(buf)]='\0';
			if(strcmp(buf,"%\n")==0){
				if(currentLine>current->lineNo){
					break;
				}else{
					tmpstr[0]='\0';
				}
			}else{
				tmplen=strlen(tmpstr);
				if(tmplen+strlen(buf)>BUFFSIZE-1){
					currentLine++;
					buf[0]='\0';
					continue;
				}
				//printf("strlen(tmpstr)=%d,strlen(buf)=%d\n",strlen(tmpstr),strlen(buf));
				strcat(tmpstr,buf);
				tmpstr[tmplen+strlen(tmpstr)]='\0';
				
			}
			currentLine++;
			
			buf[0]='\0';
		}
		strcpy(current->sayContent,tmpstr);
		
		current=current->next;
		
		if (currentFile){
		fclose(currentFile);
		}
		
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
	char new[BUFFSIZE*4];
	char left[BUFFSIZE];
	char tmp[BUFFSIZE];
	int tmplen=0;
	
	char* subPtr=NULL;
	
	char replace[BUFFSIZE];
	replace[0]='\0'; 
	//printf("%s\n",pattern);
	strcpy(replace,"\e[7m");
	strcat(replace,pattern);
	
	
	strcat(replace,"\e[0m");
	//printf("%s\n",replace);
	
	
	int oldSubLen=strlen(pattern);
	
	
	if(hflag){
		
		while(current){
			if (current!=head)
				printf("-------------------------------------\n");
			new[0]='\0';
			strcpy(left,current->sayContent);
			while((subPtr=strstr(left,pattern))!=NULL){
				
				tmplen=strlen(new);
				strncat(new,left,subPtr-left);
				new[tmplen+(int)(subPtr-left)]='\0';
				
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


