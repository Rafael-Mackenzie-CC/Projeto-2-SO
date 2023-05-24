#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //habilita funções de thread
#include <semaphore.h> // habilita funções de semáforo
#include <unistd.h> //habilita a função sleep
#include <malloc.h>
#include <sys/types.h>
//número de filósofos/garfos
#define N 100 
struct c {
 int saldo;
};

typedef struct c conta;
conta from, to;
int valor;

sem_t semaforo[2]; // vetor de semáforos para as transferências

void *transacoes(void *arg) 
{  
 // Todas as contas começam com saldo 100
  sem_wait(&semaforo[1]);
 
     
  from.saldo -= 1;
   sem_wait(&semaforo[1]);
  to.saldo += 1;
   sem_wait(&semaforo[2]);
 
  printf("Transferência concluída com sucesso!\n");
  printf("Saldo de c1: %d\n", from.saldo);
  printf("Saldo de c2: %d\n", to.saldo);
 return 0;
}


int main()
{    
  from.saldo = 100;
  to.saldo = 100;
    pthread_t transfer[N]; 
    int i; 
    for(i = 0; i < N; i++) {
        sem_init(&semaforo[i], 1, 1); 
    }


    int ids[N];
    for(i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&transfer[i], NULL, transacoes, &ids[i]);
      
    }

    for(i = 0; i < N; i++) {
        pthread_join(transfer[i], NULL); 
    }

    for(i = 0; i < N; i++) { 
        sem_destroy(&semaforo[i]); 
    }

    return 0;
}