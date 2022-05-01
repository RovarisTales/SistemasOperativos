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

//TODO comunicacao entre servidor e cliente -> Tiago
//TODO status -> Breno
//TODO atualizar permissoes para fazer proc-file -> Breno
//TODO atualizar proc-file() -> Tales
//TODO atualizar argv no servidor para o pipe com nome -> Tales
//TODO Ler arquivo 
int main (int argc, char *argv[])
{
    char buffer[256];

    for (int i = 1; i < argc; i++)
    {
        strcat(buffer,argv[i]);
        strcat(buffer," ");
    }

    printf("%s\n",buffer); //está a imprimir uma merda qualquer no inicio

    int fd = open("contacto",O_WRONLY,0666);
    write(fd,buffer,strlen(buffer));
    //TODO while que espera o concluded e cria um FIFO para o processo.
    
    close(fd);
    

    return 0;

}


























































