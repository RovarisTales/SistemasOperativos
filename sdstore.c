//
// Created by tiagorodrigues on 23-04-2022.
// ./sdstore proc-file banana.txt bananacomp.txt bcompress nop gcompress encrypt nop
// ./sdstore status
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

int main (int argc, char *argv[]){
    char buffer [256];
    pid_t pid = getpid();
    char aux[10];
    /*aux = itoa(pid,10);*/
    snprintf(aux, sizeof(aux), "%d", pid);
    mkfifo(aux,0666);
    strcpy(buffer,argv[1]);
    strcat(buffer," ");
    for (int i = 2; i < argc; i++){

        strcat(buffer,argv[i]);
        strcat(buffer," ");
        
    }

    strcat(buffer,aux);
    //printf("buffer %s\n",buffer);
   
    int fd = open("contacto",O_WRONLY);
    if(fd  == -1){
        perror("open");
    }
    write(fd,buffer,strlen(buffer)+1);
    close(fd);
    
    int fd2 = open(aux,O_RDONLY);
    
    int size;
    
    char line [512];
    //size = read(fd2,line,sizeof (line));
    //if(size == -1){
    //    perror("read");
    //}
    //write(STDOUT_FILENO,line,size);
    memset(line,0,sizeof(line));
    
    while (1){

        size = read(fd2,line,sizeof (line));
        if(size == -1){
            perror("read");
        }
        if(size>0) {
            write(STDOUT_FILENO,line,size);
        }
         
        if(strstr(line,")")!= NULL)  break;
        memset(line,0,strlen(line));
    }
    printf("\n");
    

    close(fd2);
    unlink(aux);

    return 0;

}

























































