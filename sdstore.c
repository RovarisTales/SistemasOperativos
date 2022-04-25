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

int status(){
    return 0;
}

int procfile(int argc,char *argv[]){


    char *entrada_saida[] = {"./SDStore-transf/bcompress",NULL};


    //printf("%s",entrada_saida[1]);

//	Necessário??
//    close(0);
//  int fd0 = open(argv[0], O_RDONLY,0666);
//    dup(fd0);
//    close(1);
//    int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
//    dup(fd1);
    for (int i = 2 ; i < argc ; i++)
    {
        /*
        int pipe_fd[2];
        if(pipe(pipe_fd[0]) < 0)
        {
            perror("pipe");
            _exit(1);
        }
        */
        char transf [17 + strlen(argv[i])];
        strcpy(transf,"./SDStore-transf/");
        strcat(transf ,argv[i]);
        if (!fork())
        {
            int fd1 = open(argv[0],O_RDONLY,0666);
            int fd = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666); dup2(fd,STDOUT_FILENO);
            dup2(fd1,0);
            execvp(argv[2],NULL); 
            close(fd);
            /*
            //close(pipe_fd)
            close(0);
            int fd0 = open(argv[0], O_RDONLY,0666);
            dup(fd0);
            close(1);
            int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
            dup(fd1);
            execv(transf ,entrada_saida);
            _exit(1);
            */
        }
        wait(NULL);
    }
    close(fd1);
    close(fd0);

    return 0;


}

int main (int argc, char *argv[]){


    //execv("./SDStore-transf/encrypt",argv+1);

    //printf("%s %s",argv[1],argv[2]);


    //execv("./SDStore-transf/encrypt",argv+1);

    /*
    char* entrada_saida [4];
    entrada_saida [0] = (char *) malloc(sizeof ("./SDStore-transf/encrypt"));
    strcpy(entrada_saida[0],"./SDStore-transf/encrypt");
    entrada_saida [1]= (char *) malloc(sizeof (argv[0])+4);
    entrada_saida [2]= (char *) malloc(sizeof (argv[1]));
    strcpy(entrada_saida[1],argv[1]);
    //strcat(entrada_saida [0],argv[1]);
    strcpy(entrada_saida [2],argv[2]);
    //strcat(entrada_saida[0]," >");
    entrada_saida[3] = NULL;
    for (int i = 0 ; i < 3; i++)
    {
        printf("%s\n",entrada_saida[i]);
        write(stdout, argv[i] , strlen(argv[i]));
    }
    fflush(stdout);
    if(!fork())
    {
        execv("./SDStore-transf/encrypt",entrada_saida);
    }

    */

    switch(strcmp(argv[1],"proc-file"))
    {
        case 0:

            procfile(argc-2,argv+2);
            break;
        default:
            //printf("OIiii");
            status();
            break;
    }
    //printf("OIiii");


    return 0;

}


























































