    #include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <ctype.h>
#include <signal.h>

// TODO sinal
// TODO diminui conf
// TODO tirar itoa


typedef struct Processos* processos;
typedef struct processo processo;
struct processo{
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

processos fila = NULL;
processos exec = NULL;

int addExec(processo p)
void alteraglobal(char* var,char* num);
int ler_arquivo(char *arquivo);
int procfile(int argc,char *argv[]);
void aumentarConf(int n_transformacoes,char* transformacoes[]);
void diminuirConf(char* transformacoes);
int status();
int permissao (int n_transformacoes,char* transformacoes[] );
int addFila(struct processo p);
int removeFila(struct processo p);
int checkFila();
int executa(struct processo p);
void printLista();
int pode(int n_transformacoes,char* transformacoes[]);
int removeExec(char* p);
//TODO Tem q decidir em relação a o q fazer pois precisamos do fila e exec como variaveis gloabis

void sigterm_handler()
{
    int exitstatus;
    unlink("contacto");
    write(STDOUT_FILENO,"Terminando o Servidor Graciosamente\n", strlen("Terminando o Servidor Graciosamente\n")+1);
    int i = checkFila();
    while (i == 1)
    {   
        printf("Terminando o Servidor Graciosamente\n");
        i = checkFila();
        sleep(1);
    }
    while(wait(&exitstatus)!= -1){
        exitstatus = WEXITSTATUS(exitstatus);
    }
    write(STDOUT_FILENO,"Servidor Finalizado com sucesso\n", strlen("Servidor Finalizado com sucesso\n")+1);
    exit(1);
}


int main (int argc, char *argv[]){
    //processos fila = NULL;
    //processos exec = NULL;
    
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
    {
        perror("SIGTERM failed");
    }
    
    int id = 0;
    ler_arquivo(argv[1]);
    
    mkfifo("contacto", 0666);
    while (1){
        
        int fd = open("contacto", O_RDONLY);
        if(fd == -1){
            perror("open");
        }
        char line[256];
        read(fd, line, sizeof(line));
        close(fd);
       
        
        if(strstr(line,"proc-file")!=NULL) {
            
            processo p;

            int es = -2;
            for (int aux = 0; line[aux] != '\0'; aux++){
                if (line[aux] == ' ') es++;
            }
            char **t = malloc(sizeof(char *) * es);

            p.n_transformacoes = es;
            
            es = 0;
            char *resto = NULL;
            char *token = NULL;
            
            for (token = strtok_r(line, " ", &resto); token != NULL; token = strtok_r(resto, " ", &resto)) {
                if (es == 0) {
                    if (!strcmp(token, "proc-file")){
                        
                        p.procfile = 1;
                    }
                    else{
                        
                        p.procfile = 0;
                        p.pid = strdup(token);
                        break;
                    }
                }
                else if (es == 1){
                    

                    p.prioridade = atoi(token);
                    

                }
                else if (es == 2){
                    
                    int fd3 = open(token, 0666);
                    if(fd3 == -1){
                        perror("open");
                    }
                    struct stat st;
                    fstat(fd3, &st);
                    off_t filesize = st.st_size;
                    p.tamanho_original = filesize;
                    
                    t[es - 2] = strdup(token);
                    
                    
                }
                

                else if (p.n_transformacoes+2 == es ){
                  
                    p.pid = strdup(token);

                }
                else {
                    t[es - 2] = strdup(token);                }
                es++;
            }
            

            p.id = id;
            p.transformacoes = t;
            if(pode(p.n_transformacoes-2,p.transformacoes+2)){
                //printf("PODE ENTRAR\n");
                addFila(p);
            }
            else{
                //printf("NAO PODE ENTRAR\n");
                int fd1 = open(p.pid,O_WRONLY,0666);
                if(fd1 == -1){
                    perror("open");
                }
                write(fd1,"(Excede o maximo de operaçoes)",32);
                close(fd1);
            }
            id++;
        
        }else if(strstr(line,"status")!= NULL){
            
            processo p2;
            int st = 0;
            char *resto = NULL;
            char *token = NULL;
            for (token = strtok_r(line, " ", &resto); token != NULL; token = strtok_r(resto, " ", &resto)) {
                if(st == 0) st++;
                else{
                    p2.pid = malloc(sizeof (strlen(token)));
                    strcpy(p2.pid,token);
                }
            
            }
            p2.id = id;
            id++;
            p2.n_transformacoes = 0;
            p2.prioridade = -1;
            p2.procfile = 0;
            p2.tamanho_final = -1;
            p2.tamanho_original = -1;
            p2.transformacoes = NULL;
            
            status(p2);
        }
        else{
            //printf("linha : %s\n",line);
            if(strchr("1234567890",line[0])!= NULL && strlen(line)>0){
                //printf("entrei\n");
                removeExec(line);
            }
            diminuirConf(line);
        }
       
        //printLista(fila);
        checkFila();
        memset(line, 0, strlen(line));
    }
}


void printLista(){
    
    if (fila != NULL){
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
    else{
        printf("FILA NULL\n");
    }

}

int addFila(processo p){

    int fd1 = open(p.pid,O_WRONLY);
    if (fd1 == -1){
        perror("open");
    }
    int t = write(fd1,"Pending\n",9);
    if (t == -1){
        perror("write");
    }
    close(fd1);
    if (fila == NULL){
        
        processos new = malloc (sizeof (struct Processos));
        new->data = p;
        new->next = NULL;
        
       
        fila = new;
    }
    else{
        
        struct processo dados = fila->data;
        if (p.prioridade > dados.prioridade){
            processos new = malloc(sizeof(struct Processos));
            new->data = p;
            new->next = fila;
            fila = new;
            return 1;
        }
        processos aux = fila->next;
        processos ant = fila;
        for (; aux != NULL; ant = aux,aux = aux->next){
            dados = aux->data;
            if (p.prioridade > dados.prioridade){
                
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

int addExec(processo p)
{

    
    if (exec == NULL)
    {
        
        processos new = malloc (sizeof (struct Processos));
        new->data = p;
        new->next = NULL;
        
       
        exec = new;
    }
    else
    {
        
        struct processo dados = exec->data;
        if (p.prioridade > dados.prioridade){
            processos new = malloc(sizeof(struct Processos));
            new->data = p;
            new->next = exec;
            exec = new;
            return 1;
        }
        processos aux = exec->next;
        processos ant = exec;
        for (; aux != NULL; ant = aux,aux = aux->next){
            dados = aux->data;
            if (p.prioridade > dados.prioridade){
                
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

int removeFila(struct processo p){
    processos aux = fila;
    processos ant = NULL;
    if(aux->data.id == p.id){
        //printf("removendoprimeiro\n");
        ant = aux;
        fila = aux -> next;
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

int removeExec(char* p){
    //printf("oi\n");
    processos aux = exec;
    processos ant = NULL;
    //printf("recebido: %s\n",p);
    if(strcmp(aux->data.pid ,p)==0){
        
        ant = aux;
        exec = aux -> next;
        free(ant);
        return 1;
    }
    while (aux != NULL){
        ant = aux;
        aux = aux->next;
        if(strcmp(aux->data.pid ,p)==0){
            ant -> next = aux->next;
            free(aux);
            return 1;
        }
    }
    return 0;
}

int checkFila()
{
    processos corre = fila;
    
    if (fila == NULL)
    {
        return 0;
    }
    else
    {
        for (;  corre!=NULL ; corre =corre->next)
        {
            struct processo dados = corre->data;
            
            if(dados.procfile == 1)
            {
                if (permissao(dados.n_transformacoes-2,dados.transformacoes+2)){

                    aumentarConf(dados.n_transformacoes-2,dados.transformacoes+2);
                    addExec(dados);
                    removeFila(dados);
                    printf("%d - prioridade ", dados.prioridade);
                    
                    
                    if (!fork()){
                        executa(dados);
                        _exit(1);
                    }
                    
                    //diminuirConf(dados.n_transformacoes-2,dados.transformacoes+2);

                    //removeExec(dados);
                    return 1;
                }
            }
            else{   
                printLista();
            }
        }
    }
    
    return 0;
}





int executa(struct processo p){


    int fd1 = open(p.pid,O_WRONLY);
    if(fd1 == -1){
        perror("open");
    }
    int t = write(fd1,"Processing\n",12);
    if (t == -1){
        perror("write");
    }
    procfile(p.n_transformacoes,p.transformacoes);
    
    



    char final[96];
    strcpy(final,"Concluded (bytes-input: ");
    char aux[20];
    snprintf(aux, sizeof(aux), "%d", p.tamanho_original);
    strcat(final,aux);
    
    int fd4 = open(p.transformacoes[1], 0666);
    if(fd4 == -1){
        perror("open");
    }
    
    struct stat st;
    fstat(fd4, &st);
    off_t filesize = st.st_size;
    //printf("%ld filesize\n",filesize);
    p.tamanho_final = filesize;
    strcat(final,", bytes-output: ");
    snprintf(aux, sizeof(aux), "%ld", filesize);
    strcat(aux,")");
    strcat(final,aux);
    int t2 = write(fd1,final,strlen(final)+1);
    if (t2 == -1){
        perror("write");
    }
    close(fd1);


    //printf("filho %s\n",p.pid);
    int fd2 = open("contacto",O_WRONLY);
    if(fd2 == -1){
        perror("open");
    }
    int t1 = write(fd2,p.pid,strlen(p.pid)+1);
    if (t1 == -1){
        perror("write");
    }
    int c = close(fd2);
    if(c == -1){
        perror("close");
    }

    return 1;
}



void alteraglobal(char* var,char* num){
    int val = atoi(num);
    if(strcmp(var,"nop")== 0){

        nopM = val;
    }
    else if(strcmp(var,"bcompress")== 0){
        bcompressM = val;
    }
    else if(strcmp(var,"bdecompress")== 0){
        bdecompressM = val;
    }
    else if(strcmp(var,"gdecompress")== 0){
        gdecompressM = val;
    }
    else if(strcmp(var,"encrypt")== 0){
        encryptM = val;
    }
    else if(strcmp(var,"decrypt")== 0){
        decryptM = val;
    }
    else if(strcmp(var,"gcompress")== 0){
        gcompressM = val;
    }

}

int ler_arquivo(char *arquivo){
    
    int fd = open (arquivo, O_RDONLY);
    if(fd == -1){
        perror("open");
    }
    char buffer[128];
    //char oi[20];
    char delimit[3] = " \n";
    char* resto = NULL;
    read(fd,buffer,128);
    char* token = NULL;
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

int pode(int n_transformacoes,char* transformacoes[]){
    char var [13];
    int aux[7] = {0};


    for (int i = 0; i < n_transformacoes; i++)
    {
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0){
            aux[0]++; 
        }
        else if(strcmp(var,"bcompress")== 0){
            aux[1]++;
        }
        else if(strcmp(var,"bdecompress")== 0){
            aux[2]++;
        }
        else if(strcmp(var,"gdecompress")== 0){
            aux[3]++;
        }
        else if(strcmp(var,"gcompress")== 0){
            aux[4]++;
        }
        else if(strcmp(var,"encrypt")== 0){
            aux[5]++;
        }
        else if(strcmp(var,"decrypt")== 0){
            aux[6]++;
        }
        
    }

    for (int i = 0; i < n_transformacoes; i++){
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0)
        {
            if ( aux[0] > nopM)
                {
                    return 0;
                } 
        }
        else if(strcmp(var,"bcompress")== 0)
        {
            if (aux[1] > bcompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"bdecompress")== 0)
        {
            if (aux[2] > bdecompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"gdecompress")== 0)
        {
            if (aux[3] > gdecompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"gcompress")== 0)
        {
            if (aux[4] > gcompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"encrypt")== 0)
        {
            if (aux[5] > encryptM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"decrypt")== 0)
        {
            if (aux[6] > decryptM)
            {
                return 0;
            }
        }
        
    }
    return 1;
}
int permissao (int n_transformacoes,char* transformacoes[] ){
    char var [13];
    int aux[7] = {0};


    for (int i = 0; i < n_transformacoes; i++)
    {
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0){
            aux[0]++; 
        }
        else if(strcmp(var,"bcompress")== 0){
            aux[1]++;
        }
        else if(strcmp(var,"bdecompress")== 0){
            aux[2]++;
        }
        else if(strcmp(var,"gdecompress")== 0){
            aux[3]++;
        }
        else if(strcmp(var,"gcompress")== 0){
            aux[4]++;
        }
        else if(strcmp(var,"encrypt")== 0){
            aux[5]++;
        }
        else if(strcmp(var,"decrypt")== 0){
            aux[6]++;
        }
        
    }



    for (int i = 0; i < n_transformacoes; i++){
        strcpy(var, transformacoes[i]);
        if(strcmp(var,"nop")== 0)
        {
            if ( nop_e+ aux[0] > nopM)
                {
                    return 0;
                } 
        }
        else if(strcmp(var,"bcompress")== 0)
        {
            if (bcompress_e + aux[1] > bcompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"bdecompress")== 0)
        {
            if (bdecompress_e + aux[2] > bdecompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"gdecompress")== 0)
        {
            if (gdecompress_e + aux[3] > gdecompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"gcompress")== 0)
        {
            if (gcompress_e + aux[4] > gcompressM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"encrypt")== 0)
        {
            if (encrypt_e + aux[5] > encryptM)
            {
                return 0;
            }
        }
        else if(strcmp(var,"decrypt")== 0)
        {
            if (decrypt_e + aux[6] > decryptM)
            {
                return 0;
            }
        }
        
    }
    return 1;
}

void aumentarConf(int n_transformacoes,char* transformacoes[]){
    char var [13];
    for (int i = 0; i < n_transformacoes; i++){
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

void diminuirConf(char* transformacoes){
    char var [13];
    //for (int i = 0; i < n_transformacoes; i++){
        strcpy(var, transformacoes);
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

    //}
}

int status(processo p){
    
    processos corre = exec;
    char linha [128];
    int t;
    int fd = open(p.pid,O_WRONLY);
    if(fd == -1){
        perror("open");
    }
    if (corre != NULL){
        //char printProc[512];
        for (;  corre!=NULL ; corre =corre->next)
        {
            t = write(fd,"Processo ",10);
            snprintf(linha, sizeof(linha), "%d", corre->data.id);
            for (int i = 0 ; i < corre->data.n_transformacoes; i++)
            {
                strcat(linha," ");
                strcat(linha,corre->data.transformacoes[i]);
            }
            strcat(linha,"\n");
            t = write(fd,linha,strlen(linha));
            if (t == -1){
                perror("write");
            }
            memset(linha,0,strlen(linha));
        }

    }

    char operacoes [512];
    snprintf(operacoes , sizeof(operacoes), "transf bcompress: %d / %d (running/max)\ntransf bdecompress: %d / %d (running/max)\ntransf gcompress: %d / %d (running/max)\ntransf gdecompress: %d / %d (running/max)\ntransf encrypt: %d / %d (running/max)\ntransf decrypt: %d / %d (running/max)\ntransf nop: %d / %d (running/max)",bcompress_e,bcompressM,bdecompress_e,bdecompressM,gcompress_e,gcompressM,gdecompress_e,gdecompressM,encrypt_e,encryptM,decrypt_e,decryptM,nop_e,nopM  );
    //printf("%s\n",operacoes);

    t = write(fd,operacoes, strlen(operacoes));
    if (t == -1){
        perror("write");
    }
    memset(operacoes,0,strlen(operacoes));
    close(fd);
    
    return 0;
}

int procfile(int argc,char *argv[]){
    int exitstatus;
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
                if(fd0 == -1){
                    perror("open");
                }
                //ficehiro passa a ser stdin
                dup2(fd0,STDIN_FILENO);
                //close stdout e programa escreve no ficheiro pipe escrita n
                int fd1 = open(argv[1],O_CREAT|O_WRONLY|O_TRUNC,0666);
                if(fd1 == -1){
                    perror("open");
                }
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
                if(fd0 == -1){
                    perror("open");
                }
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
                if(fd1 == -1){
                    perror("open");
                }
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
    int i = 2;
    while(wait(&exitstatus)!= -1){
        exitstatus = WEXITSTATUS(exitstatus);

        
        if(exitstatus==0){
            //printf("argu %s\n",argv[i]);
            int fd1 = open("contacto",O_WRONLY);
            if(fd1 == -1){
                perror("open");
            }
            int t = write(fd1,argv[i],strlen(argv[i])+1);
            if (t == -1){
                perror("write");
            }
            int c = close(fd1);
            if(c == -1){
                perror("close");
            }
            i++;
        }
    }
   
    return 0;


}




