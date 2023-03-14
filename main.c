#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#define MAX 20
void* Mult_Per_Matrix();
void readfilea(char *file);
void readfileb(char *file);
int Thread_Per_Matrix();
void* Mult_Per_Row(void* id);
void* Mult_Per_Element(void* id);
int Thread_Per_Row();
int Thread_Per_Element();
void write_to_file(char *file,char *phrase);
void resetc();


int a[MAX][MAX],b[MAX][MAX],c[MAX][MAX];
int mata[MAX*MAX],matb[MAX*MAX];
int rowa,rowb,cola,colb,rowc,colc;
typedef struct {
    int row;
    int col;
} thread;

void resetc(){
    for (int i = 0; i <rowc ; ++i) {
        for (int j = 0; j <colc ; ++j) {
            c[i][j]=0;
        }
    }
}



void write_to_file(char *file,char *phrase){
    FILE *ptr;
    char dest[1024]="/home/exception/";
    strncat(dest,file, strlen(file));
    ptr= fopen(dest,"w");
    if(ptr==NULL){
        printf("failed to open file\n");
        return;
    }
    fprintf(ptr, "%s\n",phrase);
    fprintf(ptr,"row=%d col=%d\n",rowc,colc);
    for (int i = 0; i <rowc ; ++i) {
        for (int j = 0; j <colc ; ++j) {
            fprintf(ptr,"%d ",c[i][j]);
        }
        fprintf(ptr,"\n");
    }
    fclose(ptr);
}


void readfilea(char *file){
    char dest[1024]="/home/exception/";
    int i=0;
    const char *delim = " \t\n\r\f\v";  // whitespace characters
    char *token=NULL;
    strncat(dest,file, strlen(file));
    FILE *fp;
    fp = fopen(dest, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        for (token = strtok(buffer, delim); token != NULL; token = strtok(NULL, delim)) {
            if(token[0]=='r'){
                char *value;
                value= strchr(token,'=');
                *value='\0';
                value++;
                rowa= atoi(value);
            }
            else if(token[0]=='c'){
                char *value;
                value= strchr(token,'=');
                *value='\0';
                value++;
                cola= atoi(value);
            }
            else{
                mata[i++]= atoi(token);
            }
        }
    }
    int x=0;
    for (int k = 0; k <rowa ; ++k) {
        for (int j = 0; j <cola ; ++j) {
            a[k][j]=mata[x++];
        }
    }
    fclose(fp);
}
void readfileb(char *file){
    char dest[1024]="/home/exception/";
    int i=0;
    const char *delim = " \t\n\r\f\v";  // whitespace characters
    char *token=NULL;
    strncat(dest,file, strlen(file));
    FILE *fp;
    fp = fopen(dest, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        for (token = strtok(buffer, delim); token != NULL; token = strtok(NULL, delim)) {
            if(token[0]=='r'){
                char *value;
                value= strchr(token,'=');
                *value='\0';
                value++;
                rowb= atoi(value);
            }
            else if(token[0]=='c'){
                char *value;
                value= strchr(token,'=');
                *value='\0';
                value++;
                colb= atoi(value);
            }
            else{
                matb[i++]= atoi(token);
            }
        }
    }
    int x=0;
    for (int k = 0; k <rowb ; ++k) {
        for (int j = 0; j <colb ; ++j) {
            b[k][j]=matb[x++];
        }
    }
    fclose(fp);
}

int Thread_Per_Matrix(){
    pthread_t p;
    if(pthread_create(&p, NULL, &Mult_Per_Matrix, NULL)!=0){
        return -1;
    }
    if(pthread_join(p,NULL)!=0){
        return -1;
    }
    return 0;
}
int Thread_Per_Row(){
    pthread_t p2[rowc];
    for (int i = 0; i <rowa ; ++i) {
        int* index = malloc(sizeof(int));
        *index=i;
        if(pthread_create(p2 + i, NULL, Mult_Per_Row, index) != 0){
            return -1;
        }
    }
    for (int i = 0; i <rowa ; ++i) {
        if(pthread_join(p2[i],NULL)!=0){
            return -1;
        }
    }
    return 0;
}



int Thread_Per_Element(){
    pthread_t p3[rowc*colc];
    int k=0;
    for (int i = 0; i <rowa ; ++i) {
        for (int j = 0; j <colb ; ++j) {
            thread* x= malloc(sizeof (int)*2);
            x->row=i;
            x->col=j;
            if(pthread_create(p3 + (k++), NULL, Mult_Per_Element, x) != 0){
                return -1;
            }
        }
    }

    for (int i = 0; i <rowc*colc ; ++i) {
        if(pthread_join(p3[i],NULL)!=0){
            return -1;
        }
    }
    return 0;
}

void* Mult_Per_Matrix(){
    for (int i = 0; i <rowa ; ++i) {
        for (int j = 0; j <colb ; ++j) {
            for (int k = 0; k <rowb ; ++k) {
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void* Mult_Per_Row(void* id){
    int i= *(int*)id;
    for (int j = 0; j < colb; ++j) {
        for (int k = 0; k <rowb ; ++k) {
            c[i][j]+=a[i][k]*b[k][j];
        }
    }
    free(id);
}

void* Mult_Per_Element(void* id){
    thread index=*(thread*)id;
    int i=index.row;
    int j=index.col;
    for (int k = 0; k <rowb ; ++k) {
        c[i][j]+=a[i][k]*b[k][j];
    }
    free(id);
}
int main(int argc, char* argv[]) {

    if(argc==1){
        readfilea("a.txt");
        readfileb("b.txt");
    }
    else{
        char *filea= strdup(argv[1]);
        strcat(filea,".txt");
        readfilea(filea);
        char *fileb= strdup(argv[2]);
        strcat(fileb,".txt");
        readfileb(fileb);
    }

    rowc=rowa,colc=colb;


    //A thread per matrix
    resetc();
    struct timeval stop, start;

    printf("Method: A thread per matrix\nno of threads = 1\n");

    gettimeofday(&start, NULL); //start checking time
    if(Thread_Per_Matrix()==-1){
        perror("error");
        return 1;
    }

    gettimeofday(&stop, NULL); //end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("------------------------------------------------------------\n");


    if(argc==1){
        write_to_file("c_per_matrix.txt","Method: A thread per matrix");
    }
    else{
        char *filec= strdup(argv[3]);
        strcat(filec,"_per_matrix.txt");
        write_to_file(filec,"Method: A thread per matrix");
    }


    //A thread per row
    resetc();


    struct timeval stop2, start2;
    printf("Method: A thread per row\nno of threads = %d\n",rowc);


    gettimeofday(&start2, NULL); //start checking time
    if(Thread_Per_Row()==-1){
        perror("error");
        return 1;
    }

    gettimeofday(&stop2, NULL); //end checking time

    printf("Seconds taken %lu\n", stop2.tv_sec - start2.tv_sec);
    printf("Microseconds taken: %lu\n", stop2.tv_usec - start2.tv_usec);
    printf("------------------------------------------------------------\n");

    if(argc==1){
        write_to_file("c_per_row.txt","Method: A thread per row");
    }
    else{
        char *filec= strdup(argv[3]);
        strcat(filec,"_per_row.txt");
        write_to_file(filec,"Method: A thread per row");
    }


    //A thread per element

    resetc();
    struct timeval stop3, start3;
    printf("Method: A thread per element\nno of threads = %d\n",rowc*colc);

    gettimeofday(&start3, NULL); //start checking time


    if(Thread_Per_Element()==-1){
        perror("error");
        return 1;
    }

    gettimeofday(&stop3, NULL); //end checking time

    printf("Seconds taken %lu\n", stop3.tv_sec - start3.tv_sec);
    printf("Microseconds taken: %lu\n", stop3.tv_usec - start3.tv_usec);
    printf("------------------------------------------------------------\n");


    if(argc==1){
        write_to_file("c_per_element.txt","Method: A thread per element");

    }
    else{
        char *filec= strdup(argv[3]);
        strcat(filec,"_per_element.txt");
        write_to_file(filec,"Method: A thread per element");
    }


    return 0;
}
