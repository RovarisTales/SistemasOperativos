#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
typedef struct Processos* processos;
typedef struct processo processo;
struct processo
{
    int id ;
    int prioridade;
    char** transformacoes; //char* transformacoes[9]
    int n_transformacoes;
    int procfile ;
    char* pid;
    int tamanho_original ;
    int tamanho_final;
};

struct Processos {
    processo data;
    processos next;
};
//struct node node;
//Parar de usar variaveis globais ?
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

void alteraglobal(char* var,char* num);
int ler_arquivo(char *arquivo);
int procfile(int argc,char *argv[]);
void aumentarConf(int n_transformacoes,char* transformacoes[]);
void diminuirConf(int n_transformacoes,char* transformacoes[]);
int status();
int permissao (int n_transformacoes,char* transformacoes[] );
int addFila(processos* fila,struct processo p);
int removeFila(processos* fila,struct processo p);
int checkFila(processos* fila,processos *exec);
int executa(struct processo p);
void printLista(processos fila);
char* itoa(int val, int base);
//TODO Tem q decidir em relação a o q fazer pois precisamos do fila e exec como variaveis gloabis
void sigterm_handler()
{
    int tem_fila = 1
    while (tem_fila == 1)
    {
        checkFila(fila,exec)
    }
    exit(1)
}
//processos fila = NULL;
//processos exec = NULL;

int main (int argc, char *argv[]){
    processos fila = NULL;
    processos exec = NULL;

    if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
    {
        perror("SIGTERM failed");
    }
    int id = 0;
    ler_arquivo(argv[1]);
    //TODO SE FOR STATUS NAO FUNCIONA
    mkfifo("contacto", 0666);
    while (1)
    {
        int fd = open("contacto", O_RDONLY, 0666);
        char line[128];
        read(fd, line, 128);
        close(fd);
        printf("%s\n",line);
        if(strcmp(line,"done")) {
            //printf("%s\n",line);
            processo p;

            int es = -2;
            for (int aux = 0; line[aux] != '\0'; aux++)
            {
                if (line[aux] == ' ') es++;
            }
            char **t = malloc(sizeof(char *) * es);

            p.n_transformacoes = es;

            es = 0;
            char *resto = NULL;
            char *token = NULL;
            for (token = strtok_r(line, " ", &resto); token != NULL; token = strtok_r(resto, " ", &resto)) {
                if (es == 0) {
                    if (!strcmp(token, "proc-file"))
                    {
                        p.procfile = 1;
                    }
                    else
                    {
                        p.procfile = 0;
                        p.pid = malloc(sizeof (strlen(token)));
                        strcpy(p.pid,token);
                        break;
                    }
                }
                else if (es == 1)
                {
                    p.prioridade = atoi(token);
                    //printf("%d - n.prioridade\n",p.prioridade);

                }
                else if (es == 2)
                {
                    int fd3 = open(token, O_WRONLY | O_CREAT | O_APPEND | S_IRUSR | S_IWUSR,0666);
                    struct stat st;
                    fstat(fd3, &st);
                    off_t filesize = st.st_size;
                    p.tamanho_original = filesize;
                    t[es - 2] = malloc(sizeof(token));
                    strcpy(t[es - 2], token);
                }

                else if (p.n_transformacoes+2 == es )
                {
                    //printf("pid\n");
                    p.pid = malloc(sizeof (strlen(token)));
                    strcpy(p.pid,token);

                }
                else {
                    t[es - 2] = malloc(sizeof(token));
                    strcpy(t[es - 2], token);
                }
                es++;
            }

            p.id = id;
            p.transformacoes = t;
            addFila(&fila, p);
            id++;
        }
        printLista(fila);
        checkFila(&fila, &exec);
        memset(line, 0, strlen(line));
    }
}


void printLista(processos fila)
{
    //printf("NOVALISTA\n");
    if (fila != NULL)
    {
        processos  corre = fila;
        for (; corre != NULL; corre = corre->next) {
            struct processo dados = corre->data;
            printf("%d - n.prioridades\n", dados.prioridade);
            printf("%d - n.transformacoes\n", dados.n_transformacoes);
            printf("%d - n.procfile\n", dados.procfile);
            printf("%d - n.id\n", dados.id);
            printf("%s -n.pid\n",dados.pid);

            for (int i = 0; i < dados.n_transformacoes; i++) {
                printf("%s - transformacoes\n", dados.transformacoes[i]);
            }
        }
    }
    else
    {
        printf("FILA NULL\n");
    }

}

int addFila(processos* fila,processo p){

    if (*fila == NULL)
    {
        //printf("oifil\n");
        processos new = malloc (sizeof (struct Processos));
        new->data = p;
        new->next = NULL;
        int fd1 = open(p.pid,O_WRONLY);
        write(fd1,"Pending\n",8);
        close(fd1);
        //printf("0i\n");
        (*fila) = new;
    }
    else{
        //printf("Beach\n");
        struct processo dados = (*fila)->data;
        if (p.prioridade > dados.prioridade)
        {
            processos new = malloc(sizeof(struct Processos));
            new->data = p;
            new->next = (*fila);
            *fila = new;
            return 1;
        }
        processos aux = (*fila)->next;
        processos ant = *fila;
        for (; aux != NULL; aux = aux->next,ant = aux){
            dados = aux->data;
            if (p.prioridade >= dados.prioridade){
                processos new = malloc(sizeof(struct Processos));
                new->data = p;
                new->next = aux;
                ant->next = new;
                return 1;
            }
        }
        processos new = malloc(sizeof(struct Processos));
        new->data = p;
        new->next = NULL;
        ant->next =new;
        return 1;

    }
    return 0;
}

int removeFila(processos* fila,struct processo p){
    processos aux = *fila;
    processos ant = NULL;
    if(aux->data.id == p.id){
        //printf("removendoprimeiro\n");
        ant = aux;
        *fila = aux -> next;
        free(ant);
        return 1;
    }
    while (aux != NULL){
        ant = aux;
        aux = aux->next;
        if(aux->data.id == p.id){
            ant -> next = aux->next;
            free(aux);
            return 1;
        }
    }
    return 0;
}

int checkFila(processos* fila,processos *exec){
    processos corre = *fila;
    
    if (*fila == NULL)
    {
        return 1;
    }
    else
    {
        for (;  corre!=NULL ; corre =corre->next)
        {
            struct processo dados = corre->data;
            
            if(dados.procfile == 1)
            {
                if (permissao(dados.n_transformacoes-2,dados.transformacoes+2))
                {
                    aumentarConf(dados.n_transformacoes-2,dados.transformacoes+2);
                    addFila(exec,dados);
                    removeFila(fila,dados);

                    
                    
                    if (!fork())
                    {
                        executa(dados);
                        _exit(1);
                    }
                    int fd4 = open(dados.transformacoes[2], O_WRONLY | O_CREAT | O_APPEND | S_IRUSR | S_IWUSR);
                    struct stat st;
                    fstat(fd4, &st);
                    off_t filesize = st.st_size;
                    dados.tamanho_final = filesize;
                    diminuirConf(dados.n_transformacoes-2,dados.transformacoes+2);

                    removeFila(exec,dados);
                    return 1;
                }
            }
            else
            {   
                printLista((*fila));
            }
        }
    }
    
    return 0;
}





int executa(struct processo p)
{


    int fd1 = open(p.pid,O_WRONLY,0666);
    write(fd1,"Processing\n",11);

    procfile(p.n_transformacoes,p.transformacoes);

    write(fd1,"Concluded (bytes-input: ",24);
    char *aux = NULL;
    aux = itoa(p.tamanho_original,10);
    write(fd1,aux,strlen(aux));
    //TODO não ta dando print
    write(fd1,", bytes-output: ",17);
    aux = itoa(p.tamanho_final,10);
    write(fd1,aux,strlen(aux));
    write(fd1,")",1);



    close(fd1);
    return 1;
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

int ler_arquivo(char *arquivo)
{
    
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

void aumentarConf(int n_transformacoes,char* transformacoes[])
{
    char var [13];
    for (int i = 0; i < n_transformacoes; i++)
    {
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0)
        {
            nop_e++;
        }
        else if(strcmp(var,"bcompress")== 0)
        {
            bcompress_e++;
        }
        else if(strcmp(var,"bdecompress")== 0)
        {
            bdecompress_e++;
        }
        else if(strcmp(var,"gdecompress")== 0)
        {
            gdecompress_e++;
        }
        else if(strcmp(var,"gcompress")== 0)
        {
            gcompress_e++;
        }
        else if(strcmp(var,"encrypt")== 0)
        {
            encrypt_e++;
        }
        else if(strcmp(var,"decrypt")== 0)
        {
            decrypt_e++;
        }
        
    }
}

void diminuirConf(int n_transformacoes,char* transformacoes[])
{
    char var [13];
    for (int i = 0; i < n_transformacoes; i++)
    {
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0)
        {
            nop_e--;
        }
        else if(strcmp(var,"bcompress")== 0)
        {
            bcompress_e--;
        }
        else if(strcmp(var,"bdecompress")== 0)
        {
            bdecompress_e--;
        }
        else if(strcmp(var,"gdecompress")== 0)
        {
            gdecompress_e--;
        }
        else if(strcmp(var,"gcompress")== 0)
        {
            gcompress_e--;
        }
        else if(strcmp(var,"encrypt")== 0)
        {
            encrypt_e--;
        }
        else if(strcmp(var,"decrypt")== 0)
        {
            decrypt_e--;
        }

    }
}

int status(processos *exec,processo p)
{
    processos corre = *exec;
    char linha [128];
    int fd = open(p.pid,O_WRONLY,0666);
    if (corre != NULL)
    {
        //char printProc[512];
        for (;  corre!=NULL ; corre =corre->next)
        {
            strcpy(linha,"Processo ");
            for (int i = 0 ; i < p.n_transformacoes; i++)
            {
                strcat(linha,p.transformacoes[i]);
            }
            write(fd,linha,strlen(linha));
            memset(linha,0,strlen(linha));
        }

    }

    char operacoes [1024];
    strcat(operacoes,"transf bcompress: ");
    strcat(operacoes,itoa(bcompress_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(bcompressM,10));
    strcat(operacoes," (running/max)\n");

    strcat(operacoes,"transf bdecompress: ");
    strcat(operacoes,itoa(bdecompress_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(bdecompressM,10));
    strcat(operacoes," (running/max)\n");

    strcat(operacoes,"transf gcompress: ");
    strcat(operacoes,itoa(gcompress_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(gcompressM,10));
    strcat(operacoes," (running/max)\n");

    strcat(operacoes,"transf gdecompress: ");
    strcat(operacoes,itoa(gdecompress_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(gdecompressM,10));
    strcat(operacoes," (running/max)\n");

    strcat(operacoes,"transf encrypt: ");
    strcat(operacoes,itoa(encrypt_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(encryptM,10));
    strcat(operacoes," (running/max)\n");

    strcat(operacoes,"transf decrypt: ");
    strcat(operacoes,itoa(decrypt_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(decryptM,10));
    strcat(operacoes," (running/max)\n");

    strcat(operacoes,"transf nop: ");
    strcat(operacoes,itoa(nop_e,10));
    strcat(operacoes,"/");
    strcat(operacoes,itoa(nopM,10));
    strcat(operacoes," (running/max)\n");
    write(fd,operacoes, strlen(operacoes));


    close(fd);
    
    return 0;
}

int procfile(int argc,char *argv[])
{
    int exitstatus;
    //criar n-2 pipes para os filhos
    int p[argc-2][2];

    for (int i = 2 ; i < argc ; i++)
    {
        //pipe 0 leitura
        //pipe 1 escrita
        //cria n-esimo pipe
        if(i != argc-1){
            if(pipe(p[i-2]) < 0)
            {
                perror("pipe");
                _exit(1);
            }
        }
        //TODO aqui trocar o coiso
        char transf [17 + strlen(argv[i])];
        strcpy(transf,"./SDStore-transf/");
        strcat(transf ,argv[i]);
        //primeiro ciclo e 1 argumento
        if(i == 2 && (i == (argc -1))){
            
            if(!fork())
            {
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
        else
        {

            if(!fork())
            {

                int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                dup2(fd1,STDOUT_FILENO);
                dup2(p[i-3][0],STDIN_FILENO);
                execl(transf ,transf,NULL);
                perror("execl");
                _exit(1);
            }
            
        }
        //fecha pipe de escrita
        close(p[i-2][1]);
        //fecha pipe de leitura anterior
        if(i!=2)close(p[i-3][0]);
        if(i == (argc-1))close(p[i-2][0]);


        
    }
    //LISTA PIDS PENDENTES
    //int i = 2;
    while(wait(&exitstatus)!= -1){
        exitstatus = WEXITSTATUS(exitstatus);

        printf("exitstatus %d\n",exitstatus);
        if(exitstatus==0){
            printf("waitconcluded\n");
            int fd1 = open("contacto",O_WRONLY);
            write(fd1,"done",4);
            close(fd1);

            //i++;
        }
    }
    return 0;


}

char* itoa(int val, int base)
{

    static char buf[32] = {0};

    int i = 30;

    for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }
    return &buf[i+1];

}



