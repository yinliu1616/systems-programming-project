#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

//extern int h_errno;


#define FILENAMESIZE 4096
#define IPlen 16
#define Hostlen 128

#define ASSERT(condition)                                                     \
    if (!(condition)) {                                                       \
        fprintf(stderr, "Assertion failed: line %d, file \"%s\"\n",           \
                __LINE__, __FILE__);                                          \
	fflush(stderr);							      \
        exit(1);                                                              \
    }

int checkPosinteger(char* argv){
	int k;
	char c;
	for (k=0;k<strlen(argv);k++){
		c=argv[k];
		if (!isdigit(c)){
			return 0;
			    //fprintf(stderr, "Sorry. Both arguments must be positive nonzero integers.\n");
			    //exit(1);
		}
	}
	return 1;
	
}


struct buffer
{
  //char (*buffer_pointer)[IPlen];	/* the actual data */
  char buffer_pointer[1000][IPlen];
  int size;
  pthread_mutex_t lock;		/* mutex ensuring exclusive access to buffer */
  int readpos, writepos;	/* positions for reading and writing */
  pthread_cond_t notempty;	/* signaled when buffer is not empty */
  pthread_cond_t notfull;	/* signaled when buffer is not full */
};

/* Initialize a buffer */
static void
init (struct buffer *b,int buffersize)
{
  //b->buffer_pointer=(char (*)[IPlen])malloc(sizeof(char*)*(buffersize+1));
  b->size=buffersize;
  pthread_mutex_init (&b->lock, NULL);
  pthread_cond_init (&b->notempty, NULL);
  pthread_cond_init (&b->notfull, NULL);
  b->readpos = 0;
  b->writepos = 0;
}


/* Store a string of IP addr in the buffer */
static void
put (struct buffer *b, char* data)
{
  if (b->size==1){
    pthread_mutex_lock (&b->lock);
    while (b->writepos != 0)
      {
          pthread_cond_wait (&b->notfull, &b->lock);
          /* pthread_cond_wait reacquired b->lock before returning */
      }
      strcpy(b->buffer_pointer[b->writepos] , data);
      b->writepos=1;
    pthread_cond_signal (&b->notempty);
    pthread_mutex_unlock (&b->lock);


  }else{
      pthread_mutex_lock (&b->lock);
      /* Wait until buffer is not full */
      while ((b->writepos + 1) % b->size == b->readpos)
      {
          pthread_cond_wait (&b->notfull, &b->lock);
          /* pthread_cond_wait reacquired b->lock before returning */
      }
        /* Write the data and advance write pointer */
        strcpy(b->buffer_pointer[b->writepos] , data);
        b->writepos++;
      if (b->writepos >= b->size)
      b->writepos = 0;
      /* Signal that the buffer is now not empty */
      pthread_cond_signal (&b->notempty);
      pthread_mutex_unlock (&b->lock);
  }




  
}

/* Read and remove an integer from the buffer */
static void
get (struct buffer *b, char* data)
{

  if (b->size==1){
       pthread_mutex_lock (&b->lock);
       while (b->writepos == 0)
      {
          pthread_cond_wait (&b->notempty, &b->lock);
      }
      strcpy(data , b->buffer_pointer[b->readpos]);
      b->writepos=0;
      pthread_cond_signal (&b->notfull);

       pthread_mutex_unlock (&b->lock);

  }else{

    pthread_mutex_lock (&b->lock);
  /* Wait until buffer is not empty */
  while (b->writepos == b->readpos)
    {
      pthread_cond_wait (&b->notempty, &b->lock);
    }
  /* Read the data and advance read pointer */
  strcpy(data , b->buffer_pointer[b->readpos]);
  b->readpos++;
  if (b->readpos >= b->size)
    b->readpos = 0;
  /* Signal that the buffer is now not full */
  pthread_cond_signal (&b->notfull);
  pthread_mutex_unlock (&b->lock);
  }
  
  
 
}

struct cache_item
{
  char ipAddr[IPlen];
  char hostname[Hostlen];
  struct cache_item* next;
};

struct cache
{
    struct cache_item *cache_head;
    //struct cache_item *cache_tail;

    int cachesize;
    int num_items;
    pthread_mutex_t cache_lock;   /* mutex ensuring exclusive access to cache */

};

/* Initialize a cache */
static void
initCache (struct cache *c,int cachesize)
{
  c->cache_head=NULL;
  //c->cache_tail=NULL;
  c->cachesize=cachesize;
  c->num_items=0;
  pthread_mutex_init (&c->cache_lock, NULL);
  
}

static int
lookupCache (struct cache *c,char* ipAddr,char*hostname)
{
   
  if (c->cache_head==NULL){
    return 0;
  }
  if (!strcmp(ipAddr,c->cache_head->ipAddr)){
      strcpy(hostname,c->cache_head->hostname);
      return 1;
  }

  struct cache_item *current=c->cache_head->next;
  struct cache_item *prev=c->cache_head;

  while (current!=NULL){
      if (!strcmp(ipAddr,current->ipAddr)){
          strcpy(hostname,current->hostname);

          //pthread_mutex_lock (&c->cache_lock);

          prev->next=current->next;
          current->next=c->cache_head;
          c->cache_head=current;

          //pthread_mutex_unlock (&c->cache_lock);
          return 1;
      }
      current=current->next;
      prev=prev->next;
  }

  return 0;
  
}
static void
insertCache (struct cache *c,struct cache_item* newItem)
{
    //pthread_mutex_lock (&c->cache_lock);
    if(c->cache_head==NULL){
        c->cache_head=newItem;
        //c->cache_tail=newItem;
        c->num_items++;
        //pthread_mutex_unlock (&c->cache_lock);
        return;
    }
    if (c->num_items>=c->cachesize){
        int i;
        struct cache_item *current=c->cache_head;

        for (i=0;i<c->cachesize-1;i++){
            current=current->next;
        }
            
        
        free(current->next);
        current->next=NULL;
        c->num_items--;
    }

    newItem->next=c->cache_head;
    c->cache_head=newItem;
    c->num_items++;
    //pthread_mutex_unlock (&c->cache_lock);
}

struct output_item
{
  //char ipAddr[IPlen];
  char hostname[Hostlen];
  int count;
  struct output_item* next;
};


struct Output
{
    struct output_item *output_head;
    pthread_mutex_t output_lock;   /* mutex ensuring exclusive access to cache */

};

/* Initialize a output */
static void
initOutput (struct Output *out)
{
  out->output_head=NULL;

  pthread_mutex_init (&out->output_lock, NULL);
  
}


static void
insertOutput (struct Output *out,char* ipAddr,char*hostname){
  //printf("enter insertOutput\n");
  struct output_item *current=NULL;
  struct output_item *newItem=NULL;
  
  struct output_item *prev=NULL;
  struct output_item *tmp=NULL;

  if ((newItem=(struct output_item *) malloc(sizeof(struct output_item)))==NULL){
      fprintf(stderr,"output_itemList:malloc failure for new output_item\n");
      exit(1);
  }
  if (hostname==NULL){
      strcpy(newItem->hostname,ipAddr);
  }else{
      strcpy(newItem->hostname,hostname);
  }
   newItem->count=1;
   newItem->next=NULL;

  if(out->output_head==NULL){
      out->output_head=newItem;
     // printf("first hostname in the head is %s\n",newItem->hostname);
      return;
  }else {
      current=out->output_head;
      prev=out->output_head;
      while(current){
        if(strcmp(newItem->hostname,current->hostname)==0){
            //printf("new existing hostname is %s\n",newItem->hostname);
            (current->count)++;
           // free(newItem);
            break;
        }else if(strcmp(newItem->hostname,current->hostname)<0){
            if(current==out->output_head){
              tmp=out->output_head;
              out->output_head=newItem;
              newItem->next=tmp;
             // printf("brand new hostname is %s\n",newItem->hostname);
            
            }else{

              prev->next=newItem;
              newItem->next=current;
            
            }
            break;
        }else{
            prev=current;
            current=current->next;
        }
        if(current==NULL){
            prev->next=newItem;
            //printf("brand new hostname in the tail is %s\n",newItem->hostname);

        }
    }


  }
  
}

void printOutput(struct Output *out){
  struct output_item *current=out->output_head;
  
  while(current){
    printf("%s---->%d\n",current->hostname,current->count);
    current=current->next;
  }
}


struct buffer buf;
struct cache cac;
struct Output out;
//struct output_item* output;
pthread_mutex_t threadsafe_lock;
pthread_mutex_t hit_lock;
//pthread_mutex_t output_lock;

float total=0;
float hit=0; 
int filedelay=0;
int threaddelay=0;  


static void *
producer (void *filename)
{
  FILE * logFile;
  char oneLine[IPlen];

   struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = filedelay * 1000000L;

    //printf("will delay %d msec each read\n",filedelay);

  logFile=fopen ((char*)filename , "r");
  if (logFile == NULL) perror ("Error opening file");
  
   //while( fgets (oneLine , IPlen , logFile) != NULL ){
    while( fscanf(logFile,"%s",oneLine) == 1){
      
      nanosleep(&delay, (struct timespec *)NULL);

    total++;
		//printf("produce %s",oneLine);
   	put (&buf, oneLine);
   }
  fclose (logFile);
 
  put (&buf, "OVER");
  put (&buf, "OVER");
  return NULL;
}

static void *
consumer (void *threadname)
{
  char oneLine[IPlen];
  char hostname[Hostlen];
  struct cache_item* newItem;
  struct timespec delay;
  delay.tv_sec = 0;
  delay.tv_nsec = threaddelay * 1000000L;
      //printf("will delay %d msec each lookup\n",threaddelay);

  while (1)
  {
      nanosleep(&delay, (struct timespec *)NULL);
      memset(oneLine, '\0', IPlen);
      memset(hostname, '\0', Hostlen);

      get (&buf,oneLine);
      if (!strcmp(oneLine,"OVER")){
          //printf ("%s consume OVER\n", (char*)threadname);
          break;
      }
		  
      //printf ("%s consume %s", (char*)threadname,oneLine);
      //printf("oneLine:%s,len=%d, last=%c ,last2=%c\n",oneLine,(int)strlen(oneLine),oneLine[strlen(oneLine)-1],oneLine[strlen(oneLine)-2]);
     
      oneLine[strlen(oneLine)-1]='\0';
      //printf("oneLine:%s,len=%d, last=%c ,last2=%c\n",oneLine,(int)strlen(oneLine),oneLine[strlen(oneLine)-1],oneLine[strlen(oneLine)-2]);
      
       pthread_mutex_lock (&(cac.cache_lock));
      if (lookupCache (&cac,oneLine,hostname)){
          pthread_mutex_unlock (&(cac.cache_lock));

          pthread_mutex_lock (&hit_lock);

          hit++;
          //total++;
          pthread_mutex_unlock (&hit_lock);

          //printf("%s cache hit: %s\n",(char*)threadname,hostname);

      }else{
        //pthread_mutex_unlock (&(cac.cache_lock));

          struct hostent *host;
          struct in_addr addr;
          int retval;

          //total++;

          //pthread_mutex_lock (&threadsafe_lock);
          retval=inet_aton(oneLine, &addr);
          if(retval==0){
            //printf("inet_pton: invalid address\n");
            strcpy(hostname,"invalid address");
          }      
          else{
                pthread_mutex_lock (&threadsafe_lock);
                host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
              pthread_mutex_unlock (&threadsafe_lock);


                if (host==NULL){
                    //printf("gethostbyaddr: problematic address %s\n", oneLine);
                    strcpy(hostname,"problematic address");
                }else{
                    
                   // printf("%s cache miss: %s\n",(char*)threadname,host->h_name);
                    strcpy(hostname,host->h_name);
                }
          }
          newItem= (struct cache_item*)malloc(sizeof(struct cache_item));
          strcpy(newItem->ipAddr,oneLine);
          strcpy(newItem->hostname,hostname);
          newItem->next=NULL;
          //pthread_mutex_lock (&(cac.cache_lock));
          insertCache (&cac,newItem);
          pthread_mutex_unlock (&(cac.cache_lock));


      }

      if ((strcmp(hostname,"invalid address")==0)||(strcmp(hostname,"problematic address")==0)){
          pthread_mutex_lock (&(out.output_lock));
          insertOutput(&out,oneLine,NULL);
          pthread_mutex_unlock (&(out.output_lock));
      }else{
          pthread_mutex_lock (&(out.output_lock));
          insertOutput(&out,oneLine,hostname);
          pthread_mutex_unlock (&(out.output_lock));
      }

        
      //pthread_mutex_unlock (&threadsafe_lock);
      
    }
  return NULL;
}



int main(int argc, char**argv){
	int numlines=100,cachesize=1000;
	char *logFile;

	for (argc--,argv++;argc>0;argc -= 1, argv += 1){
			if (!strcmp(*argv, "-b")){
				ASSERT(argc > 1);
				if (!checkPosinteger(*(argv+1))){
					fprintf(stderr, "Sorry. numlines must be positive non-zero integers between 0 and 1000.\n");
			    	exit(1);
				}

				numlines=atoi(*(argv+1));
				if ((numlines > 1000) || (numlines <= 0)){
					fprintf(stderr, "Sorry. numlines must be positive non-zero integers between 0 and 1000.\n");
			    	exit(1);
				}


				printf ("accept numlines=%d\n",numlines);
				argc--;
				argv++;
			}               
			else if (!strcmp(*argv, "-N")) {
				ASSERT(argc > 1);
				if (!checkPosinteger(*(argv+1))){
					fprintf(stderr, "Sorry. cachesize must be positive non-zero integers between 0 and 10000.\n");
			    	exit(1);
				}	
			    cachesize=atoi(*(argv+1));
			    if ((cachesize > 10000) || (cachesize <= 0)){
					fprintf(stderr, "Sorry. cachesize must be positive non-zero integers between 0 and 10000.\n");
			    	exit(1);
				}

			    printf ("accept cachesize=%d\n",cachesize);
			    argc--;

			    argv++;


			}else if (!strcmp(*argv, "-d")){
				ASSERT(argc > 1);
				if (!checkPosinteger(*(argv+1))){
					fprintf(stderr, "Sorry. filedelay must be positive integers.\n");
			    	exit(1);
				}	
			    filedelay=atoi(*(argv+1));
			    printf ("accept filedelay=%d\n",filedelay);
			    argc--;

			    argv++;

			}else if (!strcmp(*argv, "-D")){
				ASSERT(argc > 1);
				if (!checkPosinteger(*(argv+1))){
					fprintf(stderr, "Sorry. threaddelay must be positive integers.\n");
			    	exit(1);
				}
			    threaddelay=atoi(*(argv+1));
				printf ("accept threaddelay=%d\n",threaddelay);
				argc--;

			    argv++;
			}else{
				logFile=*argv;
				printf("accept logFile=%s\n",logFile);
			}
	}

    FILE * fd;
    fd=fopen ((char*)logFile , "r");
    if (fd == NULL) {
        perror ("Error opening file");
        exit(1);

    }
    fclose (fd);
	pthread_t reader, lookup1, lookup2;
	void *retval;
	init(&buf,numlines);
  initCache(&cac,cachesize);
  initOutput(&out);
  
  pthread_mutex_init (&threadsafe_lock, NULL);
  pthread_mutex_init (&hit_lock, NULL);
  //pthread_mutex_init (&output_lock, NULL);



	
  	pthread_create (&reader, NULL, producer, (void *)logFile);
  	pthread_create (&lookup1, NULL, consumer, (void *)"lookup1");
  	pthread_create (&lookup2, NULL, consumer, (void *)"lookup2");



  	pthread_join (reader, &retval);
  	pthread_join (lookup1, &retval);
  	pthread_join (lookup2, &retval);

    printf("OUTPUT:\n");

    printOutput(&out);

    printf("HIT RATE:\n");
    printf("total=%f,hit=%f,hit rate =%f\n",total,hit,hit/total);


  	return 0;

}