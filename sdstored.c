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

int bcompressM = 0;
int bdecompressM = 0;
int decryptM = 0;
int encryptM = 0;
int gcompressM = 0;
int gdecompressM = 0;
int nopM = 0;

int static id = 0;


int main (int argc, char *argv[])
{
    //printf("%s",argv[1]);
    ler_arquivo(argv[1]);
    mkfifo("contacto",0666);
    int fd = open("contacto",O_RDONLY,0666);
    //ACho q essa parte tem q ficar toda dentro de um filho
    char *transformacoes[7];
    int n_transformacoes = 0;
    //Tem q ler as transformações adicionar para o array transformações e atualizar o numero de transformacoes do pedido do cliente
    switch(strcmp(argv[1],"proc-file"))
    {
        case 0:
            if (permissao(n_transformacoes ,transformacoes))
            {
                write(STDOUT_FILENO,"Pending\n",8);
                procfile(n_transformacoes,transformacoes,id);
                write(STDOUT_FILENO,"Concluded\n",10);
            }
            break;
        default:
            status(id);
            break;
    }

    return 0;

}

void alteraglobal(char* var,char* num)
{
    int val = atoi(num);
    if(strcmp(var,"nop")== 0){

        nopM = val;
    }
    else if(strcmp(var,"bcompress")== 0)
    {
        bcompressM = val;
    }
    else if(strcmp(var,"bdecompress")== 0)
    {
        bdecompressM = val;
    }
    else if(strcmp(var,"gdecompress")== 0)
    {
        gdecompressM = val;
    }
    else if(strcmp(var,"encrypt")== 0)
    {
        encryptM = val;
    }
    else if(strcmp(var,"decrypt")== 0)
    {
        decryptM = val;
    }
    else if(strcmp(var,"gcompress")== 0)
    {
        gcompressM = val;
    }




}

int ler_arquivo(char *arquivo){
    
    int fd = open (arquivo, O_RDONLY);
    char buffer[128];
    //char oi[20];
    char delimit[3] = " \n";
    char* resto;
    read(fd,buffer,128);
    char* token;
    char ant[12];
    for(token = strtok_r(buffer, delimit,&resto); token != NULL ; token = strtok_r(resto,delimit,&resto)){
        
        if(strcmp("9",token)>=0){
            
            alteraglobal(ant,token);
            
        }
        else{
            strcpy(ant,token);
            
        }
    }
    return 0;
}

int permissao (int n_transformacoes,char* transformacoes[] )
{
    char var [13];
    for (int i = 0; i < n_transformacoes; i++)
    {
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0)
        {
           if ( nop_e+ 1 > nopM)
            {
                return 0;
            } 
        }
        else if(strcmp(var,"bcompress")== 0)
        {
            if (bcompress_e + 1 > bcompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"bdecompress")== 0)
        {
            if (bdecompress_e + 1 > bdecompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"gdecompress")== 0)
        {
            if (gdecompress_e + 1 > gdecompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"gcompress")== 0)
        {
            if (gcompress_e + 1 > gcompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"encrypt")== 0)
        {
            if (encrypt_e + 1 > encryptM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"decrypt")== 0)
        {
            if (decrypt_e + 1 > decryptM)
            {
                return 0;
            }
        }
        
    }
    return 1;
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
    printf("transf nop: %d/%d (running/max)",nop_e,nopM);
    printf("transf bcompress: %d/%d (running/max)",bcompress_e,bcompressM);
    printf("transf bdecompress: %d/%d (running/max)",bdecompress_e,bdecompressM);
    printf("transf gcompress: %d/%d (running/max)",gcompress_e,gcompressM);
    printf("transf gdecompress_e: %d/%d (running/max)",gdecompress_e,gdecompressM);
    printf("transf encrypt: %d/%d (running/max)",encrypt_e,encryptM);
    printf("transf decrypt: %d/%d (running/max)",decrypt_e,decryptM);


    close(fd);
    
    return 0;
}

*/
//Adicionei a variavel ed, q é o id do processo , pois será imporante para comunicar com o cliente q tem o id o status do processo
int procfile(int argc,char *argv[], int ed){
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
