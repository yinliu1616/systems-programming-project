#include <stdio.h>
#include <string.h>
#define bufferLen 1024
#include <signal.h>


//need a sorted linked list
struct wordNode
{
	char word[bufferLen];
	int count;
	struct wordNode *next;
};
void printWordLink(struct wordNode *head);
void writeBack(struct wordNode *head);


int main(int argc, char* argv[]){
	/*
	if (strcmp(argv[0],"even")==0){
		printf("Enter even\n");
	}
	if (strcmp(argv[0],"odd")==0){
		printf("Enter odd\n");
	}
	*/
	if(argc!=2){
		fprintf(stderr, "Invalid use: specify 0 for standalone mode\n");
		exit(1);
	}
	int scannerID=atoi(argv[1]);
	
	char bufWord[bufferLen];
	struct wordNode *head=NULL;
	struct wordNode *newWord=NULL;
	struct wordNode *current=NULL;
	struct wordNode *prev=NULL;
	struct wordNode *tmp=NULL;


	while(scanf("%s", bufWord)==1){
		//printf("evenodd accept %s\n",bufWord);
		if (head==NULL){
			if ((newWord=(struct wordNode *) malloc(sizeof(struct wordNode)))==NULL){
				fprintf(stderr,"wordList:malloc failure for new wordNode\n");
				exit(1);
			}
			strcpy(newWord->word,bufWord);
			//printf("strlen(newWord->word) is %d, strlen(bufWord) is %d\n",strlen(newWord->word),strlen(bufWord));
			//newWord->wordLen=strlen(newWord);
			newWord->count=1;
			newWord->next=NULL;
			head=newWord;
			//printf("first input is %s\n",newWord->word);

			
		}else{
			current=head;
			prev=head;
			while(current){
				//printf("compare result is %d\n", strcmp(bufWord,current->word));
				if(strcmp(bufWord,current->word)==0){
					//printf("new existing element is %s\n",bufWord);
					(current->count)++;
					break;
				}else if(strcmp(bufWord,current->word)<0){
					if ((newWord=(struct wordNode *) malloc(sizeof(struct wordNode)))==NULL){
						fprintf(stderr,"wordList:malloc failure for new wordNode\n");
						exit(1);
					}
						
					strcpy(newWord->word,bufWord);
					//printf("brand new element is %s\n",newWord->word);
					//newWord->wordLen=strlen(newWord);
					newWord->count=1;
					newWord->next=NULL;
					if(current==head){
						tmp=head;
						head=newWord;
						newWord->next=tmp;
						
					}else{

						prev->next=newWord;
						newWord->next=current;
						
					}
					break;

				}else{
					prev=current;
					current=current->next;
				}
			}
			if(current==NULL){
				if ((newWord=(struct wordNode *) malloc(sizeof(struct wordNode)))==NULL){
					fprintf(stderr,"wordList:malloc failure for new wordNode\n");
					exit(1);
				}
				
				strcpy(newWord->word,bufWord);
				//printf("brand new element is %s\n",newWord->word);
			//newWord->wordLen=strlen(newWord);
				newWord->count=1;
				newWord->next=NULL;
				prev->next=newWord;
			}
		}//end else
	}//end first while()
	
	sleep(10);
	if ((strcmp(argv[0],"even")==0)&&(strcmp(argv[1],"0")!=0)){
	//if (strcmp(argv[1],"0")!=0){
		
		kill(scannerID,SIGTERM);
	}
	
	writeBack(head);
	
	
	
	
	//printWordLink(head);
	
	
	//printf("%s:after the while loop, about to exit\n",argv[0]);
	//sleep(10);
	
	exit(0);



}

void writeBack(struct wordNode *head){
	struct wordNode *current=head;
	char str_count[10];
	//printf("Write back to scanner Now:\n");
	while(current){
		//printf("%s     %d\n",current->word,current->count);
		write(1,current->word,strlen(current->word));
		write(1,"      ",6);
		sprintf(str_count,"%d",current->count);
		write(1,str_count,strlen(str_count));
		write(1,"\n",1);
		current=current->next;
	}
	return;
}


void printWordLink(struct wordNode *head){
	struct wordNode *current=head;
	
	while(current){
		printf("%s->",current->word);
		current=current->next;
	}
	printf("\n");
	
}