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


    //criar n-2 pipes para os filhos
    int p[argc-2][2];
    
    for (int i = 2 ; i < argc ; i++){
        //pipe 0 leitura
        //pipe 1 escrita
        //cria n-esimo pipe
        if(i != argc-1){
            if(pipe(p[i-2]) < 0){
                perror("pipe");
                _exit(1);
            }
        }
        char transf [17 + strlen(argv[i])];
        strcpy(transf,"./SDStore-transf/");
        strcat(transf ,argv[i]);
        //primeiro ciclo e 1 argumento
        if(i == 2 && (i == (argc -1))){
            
            if(!fork()){
                //close(pipe_fd)
            
           
                //close stdin e programa le o ficheiro fd0
                close(0);
                int fd0 = open(argv[0], O_RDONLY,0666);
                //ficehiro passa a ser stdin
                dup(fd0);
                //close stdout e programa escreve no ficheiro pipe escrita n
                close(1);
                int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                //stdout passa a ser pipe de escrita n-2
                dup(fd1);
                //programa executa
            
                execl(transf ,transf,NULL);
                perror("execl");
                _exit(1);
            
            }
        }
        //primeiro ciclo
        else if(i==2){
            printf("entrei primeiro ciclo\n");
            if(!fork()){
                //close(pipe_fd)
                //close do pipe que nao vamos usar
                close(p[i-2][0]);
                //close stdin e programa le o ficheiro fd0
                close(0);
                int fd0 = open(argv[0], O_RDONLY,0666);
                //ficehiro passa a ser stdin
                dup(fd0);
                //close stdout e programa escreve no ficheiro pipe escrita n
                close(1);
                //int fd1 = write(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                //stdout passa a ser pipe de escrita n-2
                dup(p[i-2][1]);
                //programa executa
            
            
            execl(transf ,transf,NULL);
            perror("execl");
            _exit(1);
            
            }
        }
        //ciclos seguintes
        else if ((argc-1)!= i){
            printf("entrei outros ciclos\n");
            if(!fork()){
                //close(pipe_fd)
                //close do pipe que nao vamos usar
                close(p[i-2][0]);
                //close stdin e programa le o pipe anterior
                close(0);
                dup(p[i-3][0]);
                //ficehiro passa a ser stdin
            
                //close stdout e programa escreve no ficheiro pipe escrita n
                close(1);
                //int fd1 = write(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                //stdout passa a ser pipe de escrita n-2
                dup(p[i-2][1]);
                //programa executa
                
                execl(transf ,transf,NULL);
                perror("execl");
                _exit(1);
            
            }
        }
        //ultimo ciclo
        else{
            printf("entrei ultimo ciclo\n");
            if(!fork()){
                printf("entrei ultimo ciclo       ");
                close(1);
                int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                dup(fd1);
                close(0);
                dup(p[i-3][0]);
                printf("%s executado",transf);
                execl(transf ,transf,NULL);
                
                _exit(1);
            }
            
        }
        //fecha pipe de escrita
        close(p[i-2][1]);
        //fecha pipe de leitura anterior
        if(i!=2)close(p[i-3][0]);
        if(i == (argc-1))close(p[i-2][0]);
        wait(NULL);
        
    }
 
    return 0;


}

int main (int argc, char *argv[]){


    switch(strcmp(argv[1],"proc-file")){
        case 0:

            procfile(argc-2,argv+2);
            break;
        default:
            
            status();
            break;
    }

    return 0;

}


























































