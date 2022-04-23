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

int status(){
    return 0;
}

int procfile(int argc,char *argv[]){
    /*
    char*  entrada_saida [2];
    strcpy(entrada_saida [0],argv[0]);
    strcpy(entrada_saida [1],argv[1]);
    printf("%s",entrada_saida[0]);
    printf("%s",entrada_saida[1]);
    */
    printf("%d\n",argc);
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
        printf("ARGV1 : %s\n",argv[1]);
        char transf [17 + strlen(argv[i])];
        strcpy(transf,"./SDStore-transf/");
        printf("TRANSF %s\n",transf);
        strcat(transf ,argv[i]);
        printf("TRANSF %s\n",transf);
        if (fork() == 0)
        {
            //close(pipe_fd)
            printf("\nOIIIIIIIIII");

            //execv("transf" ,argv -(argc +2));
            _exit(1);
        }
    }
    wait(1);
    return 0;

}

int main (int argc, char *argv[]){

    char * str = (char *) malloc(sizeof (argv[1]));

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


























































