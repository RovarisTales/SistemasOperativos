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
// TODO sinal
// TODO diminui conf
// TODO pending bugado

char* itoa(int val, int base);

int main (int argc, char *argv[]){
    char buffer [128];
    pid_t pid = getpid();
    char *aux = NULL;
    aux = itoa(pid,10);
    mkfifo(aux,0666);
    strcpy(buffer,argv[1]);
    strcat(buffer," ");
    strcat(aux,"\0");
    for (int i = 2; i < argc; i++){

        strcat(buffer,argv[i]);
        strcat(buffer," ");
        
    }

    strcat(buffer,aux);
    //printf("buffer %s\n",buffer);
   
    int fd = open("contacto",O_WRONLY);
    write(fd,buffer,sizeof(buffer));
    close(fd);
    
    int fd2 = open(aux,O_RDONLY,0666);
    
    int size;
    
    char line [512];
    
    
    while (1){

        size = read(fd2,line,sizeof (line));
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



char* itoa(int val, int base){

    static char buf[32] = {0};

    int i = 30;

    for(; val && i ; --i, val /= base)

        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i+1];

}























































