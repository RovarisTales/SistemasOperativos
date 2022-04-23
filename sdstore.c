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


    char* entrada_saida [2];
    entrada_saida [0]= (char *) malloc(sizeof (argv[0]));
    //strcpy(entrada_saida[0],"< ");
    //strcat(entrada_saida[0],argv[0]);
    /*
    entrada_saida [0] = (char*) malloc(sizeof ("<"));
    entrada_saida [0] = "<";
    entrada_saida [2] = (char*) malloc(sizeof ("<"));
    entrada_saida [2] = ">";
    entrada_saida [3]= (char *) malloc(sizeof (argv[1]));
     */
    entrada_saida [1]= (char *) malloc(sizeof (argv[1]));
    strcat(entrada_saida [0],argv[0]);
    strcpy(entrada_saida [1],argv[1]);
    //strcat(entrada_saida[0]," >");
    printf("%s",entrada_saida[0]);
    //printf("%s",entrada_saida[1]);

/*
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
            //close(pipe_fd)
            printf("OIII");
            fflush(stdout);
            execv(transf ,entrada_saida);
            _exit(1);
        }
    }
*/

    return 0;


}

int main (int argc, char *argv[]){

    /*
    for (int i = 1 ; i < argc ; i++ )
    {
        printf("%s",argv[i]);
        write(stdout, argv[i] , strlen(argv[i]));
    }
    //execv("./SDStore-transf/bcompress",argv+1);

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


























































