#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int bcompress_e = 0;
int bdecompress_e = 0;
int decrypt_e = 0;
int encrypt_e = 0;
int gcompress_e = 0;
int gdecompress_e = 0;
int nop_e = 0;

int bcompress = 0;
int bdecompress = 0;
int decrypt = 0;
int encrypt = 0;
int gcompress = 0;
int gdecompress = 0;
int nop = 0;

int static id = 0;


int main (int argc, char *argv[])
{

    mkfifo("contacto",0666);
    ler_arquivo(argv[1]);
    int fd = open("contacto",O_RDONLY,0666);

    switch(strcmp(argv[1],"proc-file"))
    {
        case 0:
            if (permissao());
            write(STDOUT_FILENO,"Pending\n",8);
            procfile(argc-2,argv+2,id);
            write(STDOUT_FILENO,"Concluded\n",10);
            break;
        default:
            status(id);
            break;
    }

    return 0;

}

int ler_arquivo(char *arquivo)
{
    int fd = open (arquivo, O_RDONLY);
    char buffer[128];
    read(fd,buffer,128);
    
}

int permissao ()
{
    return bcompress_e <= bcompress && bdecompress_e <= bdecompress && nop_e <= nop && gcompress_e <= gcompress 
    && gdecompress_e <= gdecompress &&  encrypt_e <= encrypt && decrypt_e <= decrypt ;
}

int status(int ed)
{
    char arquivo_cliente[13];
    strcpy(arquivo_cliente,"/tmp/cliente");
    char numero[1];
    itoa(ed,numero,10);
    strcat(arquivo_cliente,numero);

    int fd = open(arquivo_cliente,O_WRONLY);
    dup2(fd,STDOUT_FILENO);
    int temporario = 0;
    //Precisa fazer as tarefas q estao sendo executadas também mas isso precisa de comunicação com o pai q não esta feita ainda
    //Ver variaveis globais .
    printf("transf nop: %d/%d (running/max)",nop_e,nop);
    printf("transf bcompress: %d/%d (running/max)",bcompress_e,bcompress);
    printf("transf bdecompress: %d/%d (running/max)",bdecompress_e,bdecompress);
    printf("transf gcompress: %d/%d (running/max)",gcompress_e,gcompress);
    printf("transf gdecompress_e: %d/%d (running/max)",gdecompress_e,gdecompress);
    printf("transf encrypt: %d/%d (running/max)",encrypt_e,encrypt);
    printf("transf decrypt: %d/%d (running/max)",decrypt_e,decrypt);


    close(fd);
    
    return 0;
}

//Adicionei a variavel ed, q é o id do processo , pois será imporante para comunicar com o cliente q tem o id o status do processo
int procfile(int argc,char *argv[], int ed)
{
    write(STDIN_FILENO,"Procesing\n",10);

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
                int fd0 = open(argv[0], O_RDONLY,0666);
                //ficehiro passa a ser stdin
                dup2(fd0,0);
                //close stdout e programa escreve no ficheiro pipe escrita n
                int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                //stdout passa a ser pipe de escrita n-2
                dup2(fd1,STDOUT_FILENO);
                //programa executa
            
                execl(transf ,transf,NULL);
                perror("execl");
                _exit(1);
            
            }
        }
        //primeiro ciclo
        else if(i==2){

            if(!fork()){
                //close(pipe_fd)
                //close do pipe que nao vamos usar
                close(p[i-2][0]);
                //close stdin e programa le o ficheiro fd0
                int fd0 = open(argv[0], O_RDONLY,0666);
                //ficehiro passa a ser stdin
                dup2(fd0,STDIN_FILENO);
                //close stdout e programa escreve no ficheiro pipe escrita n
                //int fd1 = write(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                //stdout passa a ser pipe de escrita n-2
                dup2(p[i-2][1],STDOUT_FILENO);
                //programa executa
            
            
            execl(transf ,transf,NULL);
            perror("execl");
            _exit(1);
            
            }
        }
        //ciclos seguintes
        else if ((argc-1)!= i){

            if(!fork()){
                //close do pipe que nao vamos usar
                close(p[i-2][0]);
                //close stdin e programa le o pipe anterior
                dup2(p[i-3][0],STDIN_FILENO);
                //ficehiro passa a ser stdin
            
                //close stdout e programa escreve no ficheiro pipe escrita n
                //int fd1 = write(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                //stdout passa a ser pipe de escrita n-2
                dup2(p[i-2][1],STDOUT_FILENO);
                //programa executa
                
                execl(transf ,transf,NULL);
                perror("execl");
                _exit(1);
            
            }
        }
        //ultimo ciclo
        else{

            if(!fork()){

                int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                dup2(fd1,STDOUT_FILENO);
                dup2(p[i-3][0],STDIN_FILENO);
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