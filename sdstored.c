#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
typedef struct Processos* processos;
typedef struct processo processo;
struct processo
{
    int id ;
    int prioridade;
    char** transformacoes; //char* transformacoes[9]
    int n_transformacoes;
    int procfile ;
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
int addFila(processos* fila,struct processo *p);
int removeFila(processos* fila,struct processo p);
int checkFila(processos* fila,processos exec,char* c);
int executa(struct processo p,char* c);
void printLista(processos fila);
//processos fila = NULL;
//processos exec = NULL;

int main (int argc, char *argv[]){
    processos fila = NULL;
    processos exec = NULL;
    /*
    printf("struct 1\n");
    struct processo p ;
    p.prioridade = 3;
    p.n_transformacoes = 0;
    p.id = 1;
    p.procfile = 0;
    printf("struct 2\n");
    struct processo l ;
    l.prioridade = 2;
    l.n_transformacoes = 0;
    l.id = 2;
    l.procfile = 0;
    printf("struct 3\n");
    struct processo g ;
    g.prioridade = 4;
    g.n_transformacoes = 0;
    g.id = 3;
    g.procfile = 0;
    printf("struct 4\n");
    struct processo h ;
    h.prioridade = 3;
    h.n_transformacoes = 0;
    h.id = 4;
    h.procfile = 0;
    addFila(&fila,p);
    printf("print lista11\n");
    printLista(fila);
    addFila(&fila,l);
    printLista(fila);
    addFila(&fila,g);
    printLista(fila);
    addFila(&fila,h);
    printf("print lista2\n");
    printLista(fila);
    printf("print lista3\n");
    removeFila(&fila,g);
    printLista(fila);
    checkFila(&fila,&exec);
    */



    
    int id = 0;
    ler_arquivo(argv[1]);
    //processos *fila = NULL;
    //processos *exec = NULL;


    while (1){
        mkfifo("contacto",0666);
        int fd = open("contacto",O_RDONLY,0666);
        char line[128];
        read(fd,line,128);
        close(fd);
        //unlink("contacto");
        mkfifo("id",0666);

        int fd1 = open("id",O_WRONLY);
        char bb = id + '0';
        char c [2];
        c[0]= bb;
        c[1] = '\0';
        printf("%s\n",c);
        write(fd1,c,sizeof(c));

        //printf("%s\n",line);
        char c1[15] = "contacto";
        strcat(c1,c);
        
        printf("%s\n",c1);

        int fd2 = open(c1,O_WRONLY);
        write(fd2,"Pending\n",9);
        close(fd2);

        processo *p;
        p = malloc(sizeof(processo));
        if (!fork()) {
            //TODO COLOCAR DENTRO DO FORK E PASSAR O ENDERECO DO PROCESSO

            int es = -1;
            for (int aux = 0; line[aux] != '\0'; aux++) {
                if (line[aux] == ' ') es++;
            }
            char **t = malloc(sizeof(char *) * es);

            p->n_transformacoes = es;

            //printf("%d - n.transformacoes\n",p.n_transformacoes);
            es = 0;
            char *resto;
            char *token;
            for (token = strtok_r(line, " ", &resto); token != NULL; token = strtok_r(resto, " ", &resto)) {
                //printf("%s\n",token);
                //printf("%d\n",es);
                if (es == 0) {

                    p->prioridade = atoi(token);
                    //printf("%d - n.prioridade\n",p.prioridade);
                } else if (es == 1) {
                    if (!strcmp(token, "proc-file")) {
                        //printf("%d - n.procfile\n",p.procfile);
                        p->procfile = 1;
                    } else p->procfile = 0;

                } else {
                    t[es - 2] = malloc(sizeof(token));
                    strcpy(t[es - 2], token);
                }
                es++;
            }

            p->id = id;
            p->transformacoes = t;
            //for (int i = 0 ; i < p.n_transformacoes ; i ++){
            //    printf("%s - transformacoes\n",p.transformacoes[i]);
            //}
            //printf("oi\n");
            addFila(&fila, p);
            _exit(1);
            //printf("oiiiiiii\n");
        }
        //}
        //wait(NULL);
        checkFila(&fila,exec,c);
        //printf("executaacabou\n");
        memset(line,0,strlen(line));
        }
      
}


void printLista(processos fila)
{
    if (fila != NULL)
    {
        processos  corre = fila;
        for (; corre != NULL; corre = corre->next) {
            struct processo dados = corre->data;
            printf("%d - n.prioridades\n", dados.prioridade);
            printf("%d - n.transformacoes\n", dados.n_transformacoes);
            printf("%d - n.procfile\n", dados.procfile);
            printf("%d - n.id\n", dados.id);

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

int addFila(processos* fila,processo *p)
{
    if (*fila == NULL){
        processos new = malloc (sizeof (struct Processos));
        new->data = *p;
        new->next = NULL;
        //printf("0i\n");
        (*fila) = new;
    }
    else{
        //printf("Beach\n");
        struct processo dados = (*fila)->data;
        if (p->prioridade >= dados.prioridade)
        {
            processos new = malloc(sizeof(struct Processos));
            new->data = *p;
            new->next = (*fila);
            *fila = new;
            return 1;
        }
        processos aux = (*fila)->next;
        processos ant = *fila;
        for (; aux != NULL; aux = aux->next,ant = aux){
            dados = aux->data;
            if (p->prioridade >= dados.prioridade){
                processos new = malloc(sizeof(struct Processos));
                new->data = *p;
                new->next = aux;
                ant->next = new;
                return 1;
            }
        }
        processos new = malloc(sizeof(struct Processos));
        new->data = *p;
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

int checkFila(processos* fila,processos exec,char* c){
    processos corre = *fila;

    if (*fila == NULL) {
        printf("Hello\n");
        return 1;
    }
    else
    {
        for (;  corre!=NULL ; corre =corre->next)
        {
            struct processo dados = corre->data;
            //printf("%d - n.prioridades\n",dados.prioridade);
            //printf("%d - n.transformacoes\n",dados.n_transformacoes);
            //printf("%d - n.procfile\n",dados.prioridade);
            //for (int i = 0 ; i < dados.n_transformacoes ; i ++){
                //printf("%s - transformacoes\n",dados.transformacoes[i]);
            //}
            
            if(dados.procfile == 1)
            {
                if (permissao(dados.n_transformacoes-2,dados.transformacoes+2))
                {
                    aumentarConf(dados.n_transformacoes-2,dados.transformacoes+2);
                    addFila(&exec,&dados);
                    removeFila(&corre,dados);
                    int fd = open(c,O_WRONLY);
                    write(fd,"Processing\n",12);
                    close(fd);
                    if (!fork())
                    {
                        executa(dados,c);
                        _exit(1);
                    }
                    wait(NULL);
                    
                    //printLista(exec);
                    removeFila(&exec,dados);
                    //printLista(exec);
                    return 1;
                }
            }
            else
            {   
                printLista((*fila));
                printf("oi\n");
                //return 1;
                //status(&exec);
            }
        }
    }
    
    return 0;
}





int executa(struct processo p,char* c)
{

    //printf("pipe open\n");
    //printf("pipe closed\n");

    //int fd1 = open("contacto",O_WRONLY);
    //write(fd1,"Processing\n",11);

    //TODO talvez tenha q concertar o numero de transformcoes e o array das transformacoes
    procfile(p.n_transformacoes,p.transformacoes);
    diminuirConf(p.n_transformacoes-2,p.transformacoes+2);

    int fd1 = open(c,O_WRONLY);
    write(fd1,"Concluded\n",10);
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

int status(processos *exec)
{
    // char arquivo_cliente[13];
    // strcpy(arquivo_cliente,"/tmp/cliente");
    // char numero[1];
    // itoa(ed,numero,10);
    // strcat(arquivo_cliente,numero);

    // int fd = open(arquivo_cliente,O_WRONLY);
    // dup2(fd,STDOUT_FILENO);
    // int temporario = 0;
    // //Precisa fazer as tarefas q estao sendo executadas também mas isso precisa de comunicação com o pai q não esta feita ainda
    // //Ver variaveis globais .
    // printf("transf nop: %d/%d (running/max)",nop_e,nopM);
    // printf("transf bcompress: %d/%d (running/max)",bcompress_e,bcompressM);
    // printf("transf bdecompress: %d/%d (running/max)",bdecompress_e,bdecompressM);
    // printf("transf gcompress: %d/%d (running/max)",gcompress_e,gcompressM);
    // printf("transf gdecompress_e: %d/%d (running/max)",gdecompress_e,gdecompressM);
    // printf("transf encrypt: %d/%d (running/max)",encrypt_e,encryptM);
    // printf("transf decrypt: %d/%d (running/max)",decrypt_e,decryptM);


    // close(fd);
    
    return 0;
}

//Adicionei a variavel ed, q é o id do processo , pois será imporante para comunicar com o cliente q tem o id o status do processo
int procfile(int argc,char *argv[])
{
    
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
        //TODO aqui trocar o coiso
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
        //wait(NULL);
        //printf("ciclo finalizado");
        
        
    }
    wait(NULL);
    return 0;


}



