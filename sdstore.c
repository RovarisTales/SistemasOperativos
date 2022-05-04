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
// ./sdstore proc-file <priority> samples/file-a outputs/file-a-output bcompress nop gcompress encrypt nop

//TODO Comunicação servidor para o cliente 
int main (int argc, char *argv[]){
    char buffer[128];
    //sdsdstreijhyase proc-file banan.txt final alt1 alt2
    //argc 6
    //printf("%d aqui\n",argc);
    for (int i = 1; i < argc; i++)
    {
        strcat(buffer,argv[i]);
        //printf("%s %d aqui carai \n",argv[i],i);

        if (i != (argc-1)){
            strcat(buffer," ");
        }
        else strcat(buffer,"\0");
    }
    //printf("%sbuffer\n ",buffer);

    //printf("%s\n",buffer); //está a imprimir uma merda qualquer no inicio
    //printf("pipe open\n");
    int fd = open("contacto",O_WRONLY,0666);
    write(fd,buffer,sizeof(buffer));
    close(fd);
    buffer[0] = '\0';
    //TODO while que espera o concluded e cria um FIFO para o processo.
    //printf("pipe closed\n");
    
    char op[15];
    for(int x = 0;x!= 3;x++){
        
        int fd1 = open("contacto2",O_RDONLY,0666);
        int aux = read(fd1,op,15);
        printf("%s\n",op);
        op[0] = '\0';
        close(fd1);
    }

    return 0;

}


























































