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

//TODO comunicacao entre servidor e cliente -> Tiago
//TODO status -> Breno
//TODO atualizar permissoes para fazer proc-file -> Breno
//TODO atualizar proc-file() -> Tales
//TODO atualizar argv no servidor para o pipe com nome -> Tales

int main (int argc, char *argv[]){
    char buffer[1024];

    for (int i = 1; i < argc-1; i++)
    {
        strcat(buffer,argv[i]);
    }

    int fd = open("contacto_p_servidor",O_RDWR,0666);
    write(fd,buffer,strlen(buffer));
    //TODO while que espera o concluded e cria um FIFO para o processo.
    while ()
    {
        /* code */
    }
    
    close(fd);
    

    return 0;

}


























































