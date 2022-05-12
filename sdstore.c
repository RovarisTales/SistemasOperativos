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
// ./sdstore proc-file <priority> samples/file-a outputs/file-a-output bcompress nop gcompress encrypt nop

char* itoa(int val, int base);
//TODO Comunicação servidor para o cliente 
int main (int argc, char *argv[]){
    char buffer [128];
    pid_t pid = getpid();
    char *aux = NULL;
    aux = itoa(pid,10);

    strcat(aux,"\0");
    for (int i = 1; i < argc; i++)
    {
        strcat(buffer,argv[i]);
        strcat(buffer," ");
    }

    strcat(buffer,aux);

    int fd = open("contacto",O_WRONLY);
    write(fd,buffer,sizeof(buffer));
    close(fd);
    //buffer[0] = '\0';

    //TODO while que espera o concluded e cria um FIFO para o processo.
    mkfifo(aux,0666);
    char line [128] ;
    int fd2 = open(aux,O_RDONLY,0666);
    int size;

    while (1)
    {
        //fflush(STDOUT_FILENO);
        
        size = read(fd2,line,sizeof (line));
        if(size>0) {
            write(STDOUT_FILENO,line,size);
            
        }
        if(!strcmp(line,"Concluded\n")) break;
        memset(line,0,strlen(line));
    }
    printf("oi\n");

    //int fd2 = open(c1,O_RDONLY);
    close(fd2);
    unlink(aux);

    return 0;

}



char* itoa(int val, int base)
{

    static char buf[32] = {0};

    int i = 30;

    for(; val && i ; --i, val /= base)

        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i+1];

}























































