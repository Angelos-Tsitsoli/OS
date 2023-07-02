#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <time.h>

int child(int inte,sem_t*req_sem,sem_t * an_sem,sem_t * critical_sem,sem_t * mtex,int * read_array,int*seg,char* buf,int  lines,int  other_lines,int max_characters,int num_of_requests,int segments,sem_t * read_child,int * application,int all_requests,int children){       
  FILE * file_pointer;        
  printf("child :Mphka synarthsh child  me to i :%d\n",inte);
  int count=0;
  char* ptrbuf;
  char ptrbuf2[max_characters]; 
  ptrbuf=buf;
  int random2=0;
  int next_random2=0;
  char array[60];
  char array2[8];
  srand(time(0)); 
  double beginning;
  double finishing;
  int iterator=0;
  int iterator2=0; //(rand() %  (upper - lower + 1)) + lower;
 
  int requests=(rand() % ((segments-1) - 1 + 1))+1;
  printf("child:to request %d gia to i :%d\n",requests,inte);
  
  for (int i=1;i<=num_of_requests;i++){

   if (requests==segments-1){
   random2=(rand() % ((lines+other_lines) - 1 + 1))+1; 
    printf("child:random2:%d gia to i:%d\n",random2,inte);
  }
  else{

   random2=(rand() % ((lines) - 1 + 1))+1; 
    printf("child:random2:%d gia to i:%d\n",random2,inte);
  }
  
   
  sem_wait(read_child);
  //*application++;  
  sem_post(read_child);

  sem_wait(&mtex[requests]);
  printf("child:katebasa to mtex me to i:%d\n",inte);
  read_array[requests]++;
  if(read_array[requests]==1){


    *seg=requests;
    beginning=clock();
    sem_post(req_sem);
    printf("child:anebasa to req_sem me to i:%d\n",inte);
    

    
    sem_wait(an_sem);
     printf("child:katebasa to an_sem me to i:%d\n",inte);
    finishing=clock();

  
    sem_wait(critical_sem);
    printf("child:katebasa to critical_sem me to i:%d\n",inte);
  }

  sem_post(&mtex[requests]);
  printf("child:anebasa to mtex me to i:%d\n",inte);
  
  count=1;
  iterator=0;
  iterator2=0;

  while(count!=random2){
    if(ptrbuf[iterator]=='\n'){
       count++;
    }
  iterator++;
  }
   


  if (count==random2){
    if(ptrbuf[iterator]=='\n'){
    ptrbuf2[iterator2]='n';
    printf("%s",ptrbuf2);
    }
    else{
    while(ptrbuf[iterator]!='\n'){
      ptrbuf2[iterator2]=ptrbuf[iterator];
      
      iterator++;
      iterator2++;
      }
    }
  }

  sprintf(array, "beginning :%f finish: %f", beginning,finishing);
  file_pointer = fopen("text2.txt", "w");
  for (i = 0; array[i] != '\n'; i++) {
            fputc(array[i], file_pointer);
        }
  fputc('\n', file_pointer);

  sprintf(array2, "< %d > %d", requests,random2);
  for (i = 0; array2[i] != '\n'; i++) {
            fputc(array2[i], file_pointer);
        }
     usleep(20);



  sem_wait(&mtex[requests]);
  printf("child:katebasa to mtex me to i:%d\n",inte);
  read_array[requests]--;
  if(read_array[requests]==0){
    sem_post(critical_sem);
    printf("child:anebasa to critical_sem me to i:%d\n",inte);
  }

  sem_post(&mtex[requests]);
  printf("child:Anebasa to mtex me to i :%d\n",inte);
  





  next_random2=rand()%10;
  if (next_random2<3){
    requests=(rand() % ((segments-1) - 1 + 1))+1;
  }

 }
 
   
  if (inte==children){
   sem_wait(req_sem);
   sem_post(req_sem);

  }



  

  return 0;
}


