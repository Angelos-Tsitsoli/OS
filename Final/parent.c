#include <sys/ipc.h>
#include <sys/mman.h>
#include <errno.h>
#include <semaphore.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int child(int i,sem_t*ptr2,sem_t*ptr3,sem_t* ptr4,sem_t* ptr6,int * ptr7,int * ptr8, char * ptr10,    int  lines,int  other,int max, int lin,int segs,sem_t* read_chil,int * applicatio,int all_requests,int children);
      
int main(int argc, char *argv[])

{

////////////////////////////deal with text.txt////////////////////////
 
FILE *file_open;  //A pointer to point at the text file

int segments=atoi(argv[1]);
int lines_of_every_segment=0;
int other_lines=0;
file_open = fopen("text.txt", "r"); //We open the file and we use a+ to open and write a file.
int maxcharacters=0;  //Finds the max number of characters in a line .
int iterator_through_lines=0; //This variable iterates through the linbes of a segment
int count=1;
int i=0;
char character ;
int number_of_children=atoi(argv[2]);

int requests=atoi(argv[3]);
int counter = 1; //	This is a counter for the lines of the file.We initialize counter with one in order to count the last line when we reach EOF in the file while reading it.

//////////////////counting the segments and the maximum characters in a line of the file ////////////////////////  
	 while (!feof(file_open))            
    {
      character= fgetc(file_open); 

     if (character!= '\n')
      {                                   
         iterator_through_lines++;
         
      }
      if (character == '\n')
      {
        iterator_through_lines+=2;///???????
        if (maxcharacters<iterator_through_lines){
          maxcharacters=iterator_through_lines;
        }
		iterator_through_lines=0;
         counter++; 
      }     
    }  
//////////////////////////////////////////////////////////////////////

fseek(file_open,0,SEEK_SET); //We point the file pointer back to the start of the file 



//////////////////////the lines that every segment will have and the lines that remain if the segmentation is not even //////
lines_of_every_segment=counter/segments;
if(counter%segments==0){
	other_lines=counter%segments;
}
else if (counter%segments!=0){
     other_lines=counter%segments+1;    //Basically i put the remaining lines to the laste segment , which means that the laste segment except the lines that all segments have , the remaining lines will also be added.


}
////////////////////////////////////////////////////////////////////////////////////////////////////////

char *buffer2=malloc(maxcharacters*(lines_of_every_segment+other_lines)*sizeof(char));


/////////////////request semaphore//////////////////////////
sem_t * req_sem;
int shmemid;
shmemid = shmget(IPC_PRIVATE, sizeof(sem_t), 0666|IPC_CREAT ); 
	if (shmemid == -1) {
		perror("Error in Creation");
	} 

req_sem = (sem_t *) shmat(shmemid, NULL, 0);
	if ( req_sem == (void *) -1) {                                           
		perror("Error in Attachment."); 
		exit(2);
	}


int sem = sem_init(req_sem, 1, 0);
	if (sem != 0) {
		perror("Couldn't initialize.");
		exit(3);
	}
//////////////////////////////////////////////////////////
//////////////////////////answer_semaphore//////////////////////
sem_t * an_sem;
int shmemid2;
shmemid2 = shmget(IPC_PRIVATE, sizeof(sem_t), 0666|IPC_CREAT ); 
	if (shmemid2 == -1) {
		perror("Error in Creation");
	} 

an_sem = (sem_t *) shmat(shmemid2, NULL, 0);
	if ( an_sem == (void *) -1) {                  
		perror("Error in Attachment."); 
		exit(2);
	}


int sem2 = sem_init(an_sem, 1, 0);
	if (sem2 != 0) {
		perror("Couldn't initialize.");
		exit(3);
	}

////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////critical_semaphore//////////////////////
sem_t * critical_sem;
int shmemid3;
shmemid3 = shmget(IPC_PRIVATE, sizeof(sem_t), 0666|IPC_CREAT ); 
	if (shmemid3 == -1) {
		perror("Error in Creation");
	} 

critical_sem = (sem_t *) shmat(shmemid3, NULL, 0);
	if ( critical_sem == (void *) -1) {                                            
		perror("Error in Attachment."); 
		exit(2);
	}


int sem3 = sem_init(critical_sem, 1, 1);
	if (sem3!= 0) {
		perror("Couldn't initialize.");
		exit(3);
	}

////////////////////////////////////////////////////////////////////////////////


/////////////////////the mutex_array/////////////////////////
int mutex_id;


  mutex_id= shmget(IPC_PRIVATE, segments*sizeof(sem_t), 0666|IPC_CREAT);    
	if (mutex_id == -1) {
		perror("Error in Creation");
	} 

sem_t* mutex_array = (sem_t *) shmat(sem, NULL, 0);
	if ( mutex_array == (void *) -1) { 
		perror("Error in Attachment.");                                                 
		exit(2);
	}

 int mutex_address;
 for (int i=0;i<10;i++){
 mutex_address = sem_init(mutex_array+i, 1, 1);
	if (mutex_address != 0) {
		perror("Couldn't initialize.");
		exit(3);
	}
   
}

//////////////////////////////////////////////////


//////////////////the buffer////////////////////////
char * buffer;
int shmemid4 = shmget(IPC_PRIVATE, lines_of_every_segment*maxcharacters*sizeof(char ), 0666|IPC_CREAT); 
	if (shmemid2 == -1) {
		perror("Error in Creation");
	}                                                               

 buffer= (char *) shmat(shmemid4, NULL, 0);
	if ( buffer == (void *) -1) { 
		perror("Error in Attachment."); 
		exit(2);
	}

////////////////////////////////////////////////////////

////////////////////////the  readers_count//////////////
int shmemid5= shmget(IPC_PRIVATE, segments*sizeof(int), 0666|IPC_CREAT); 
	if (shmemid5 == -1) {
		perror("Error in Creation");
	} 

int* read_array = (int* ) shmat(shmemid5, NULL, 0);
	if ( read_array == (void *) -1) { 
		perror("Error in Attachment.");                                          
		exit(2);
	}

for(int i=0;i<segments;i++) {
  read_array[i]=0;
}

///////////////////////////////////////////////////////////////


////////////////////the integer for the segment////////
int shmemid6 = shmget(IPC_PRIVATE, sizeof(int), 0666|IPC_CREAT); 
	if (shmemid6 == -1) {
		perror("Error in Creation");
	} 
                                                                             
  int *seg= (int*) shmat(shmemid6, NULL, 0);                                         
	if ( seg == (void *) -1) { 
		perror("Error Attachment."); 
		exit(2);
	}

  *seg=-1;

///////////////////////////////////////////////////////////////

////////////////////the integer ////////
int shmemid7 = shmget(IPC_PRIVATE, sizeof(int), 0666|IPC_CREAT); 
	if (shmemid7 == -1) {
		perror("Error in Creation");
	} 
                                                                             
  int * application= (int*) shmat(shmemid7, NULL, 0);                                          
	if ( application == (void *) -1) { 
		perror("Error Attachment."); 
		exit(2);
	}

  *application=0;



///////////////////////////////////////

//////////////////////// sem for the general request of the semaphore///////////
sem_t * read_child;
int shmemid8;
shmemid8 = shmget(IPC_PRIVATE, sizeof(sem_t), 0666|IPC_CREAT ); 
	if (shmemid8 == -1) {
		perror("Error in Creation");
	} 

read_child = (sem_t *) shmat(shmemid4, NULL, 0);
	if ( read_child == (void *) -1) {                                           
		perror("Error in Attachment."); 
		exit(2);
	}


int sem4 = sem_init(read_child, 1, 1);
	if (sem4!= 0) {
		perror("Couldn't initialize.");
		exit(3);
	}


///////////////////////////////////////////////////////////////////
pid_t pids[number_of_children];
int flag=0;
int iterate=0;
int stop=0;
int the_segment_we_want=0;
int iterator_holder_old=0;
int iterator_holder_new=0;
for (i = 1; i <=number_of_children; i++) {
           if ((pids[i] = fork()) < 0) {
               perror("Failed to create process");
               return 1;
           }
           if (pids[i] == 0) {          
		      
               child(i,req_sem,an_sem,critical_sem,mutex_array,read_array,seg,buffer,lines_of_every_segment,other_lines,maxcharacters,requests,segments,read_child,application,number_of_children*requests,number_of_children);          
			   exit(0);
			   
          }
      
          

       }



while(1){
if(*application==number_of_children*requests){
break;
}
sem_wait(req_sem);   
fseek(file_open,0,SEEK_SET);
iterate=0;
count=0;
stop=0;
flag=0;
the_segment_we_want=*seg;
//printf("Katebasa to req sem \n");
while (!feof(file_open))
    {
        if(the_segment_we_want==segments-1){     
        character= fgetc(file_open);             
        if (character=='\n'){                     
            count++;
        }   

        if(count/lines_of_every_segment==the_segment_we_want&&flag==0){
            flag=1;
            stop=count+lines_of_every_segment+other_lines;
        }
        
        if(flag==1&&count<=stop){
         buffer2[iterate]=character;
         iterate++;
		 
        }
         
        if (count==stop&&flag==1){
			iterator_holder_new=iterate;
			break;
		}

		}

		else{
        character= fgetc(file_open);
        if (character=='\n'){
            count++;
        }   
				
		                                                         ///gia to teleytaio thelei eidikh metaxeirish.

        if(count/lines_of_every_segment==the_segment_we_want&&flag==0){
            flag=1;
            stop=count+lines_of_every_segment;
        }
         
		

        if(flag==1&&count<=stop){
         buffer2[iterate]=character;
         iterate++;
        //printf("count %d",count);
		 
        }
        if (count==stop&&flag==1){
			iterator_holder_new=iterate;
			break;
		}
		}
    }

if(iterator_holder_old>iterator_holder_new){
	while(iterate!=iterator_holder_old){
    iterate++;
    buffer2[iterate]='\0'; 
	}
  
}
//printf("Eftasa prin to critical sem wait me to i :%d\n",i);
//sem_wait(critical_sem);
//printf("Katebasa to critical sem \n");
int iterator6=0;
while(iterator6!=iterator_holder_new){
	buffer[iterator6]=buffer2[iterator6];
	iterator6++;
}

iterator_holder_old=iterator_holder_new;
fseek(file_open,0,SEEK_SET);

//sem_post(critical_sem);
//printf("Anebasa to critical \n"//;

sem_post(an_sem);

//printf("Anebasa to an sem \n");
} 



////////////free and destroy/////////       //MHN ksexaseiss ta freeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee pantoy 
//fclose(file_open);

fclose(file_open);
sem_destroy(req_sem);
sem_destroy(an_sem);
sem_destroy(critical_sem);
for (int i;i<segments;i++){
  sem_destroy(&mutex_array[i]);
}
free(buffer2);



return 0;
}















