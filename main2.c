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
 int valor;
};

// Utilizando o exemplo e os conceitos apresentados em sala, implemente uma solução para a condição de corrida apresentada no problema.
//A solução utilizada é o uso de semáforos para garantir a integridade das contas em meio à concorrência
typedef struct c conta;
conta c1, c2;


sem_t semaforo[2]; // vetor de semáforos para as transferências

void *transacoes1(void *arg) 
{  
  int id = *(int *) arg; 

  sem_wait(&semaforo[0]);
  if (c1.saldo>0)
  c1.saldo -= c1.valor;
  sem_post(&semaforo[0]);
  sem_wait(&semaforo[1]); 
  c2.saldo += c1.valor;  
  sem_post(&semaforo[1]);
  printf("Transferência %d concluída com sucesso!\n",id+1);
  printf("Saldo de c1: %d\n", c1.saldo);
  printf("Saldo de c2: %d\n", c2.saldo);
}
void *transacoes2(void *arg) 
{  
  int id = *(int *) arg; 

  sem_wait(&semaforo[0]);
  if (c2.saldo>0)
  c2.saldo -= c2.valor;
  sem_post(&semaforo[0]);
  sem_wait(&semaforo[1]); 
  c1.saldo += c2.valor;  
  sem_post(&semaforo[1]);
  printf("Transferência %d concluída com sucesso!\n",id+1);
  printf("Saldo de c1: %d\n", c1.saldo);
  printf("Saldo de c2: %d\n", c2.saldo);
}


int main()
{    
  // Todas as contas começam com saldo 100
  c1.saldo = 100;
  c2.saldo = 100;
  int i;
  pthread_t transfer[N];
  int opcao;
  int ids[N];
  while(opcao!=0){
    printf("Saldo de c1: %d\n", c1.saldo);
    printf("Saldo de c2: %d\n", c2.saldo);
    printf("\n\nEscolha uma opção:");
    printf("\n\n1 - Transferir saldo de conta 1 para conta 2");
    printf("\n\n2 - Transferir saldo de conta 2 para conta 1");
    printf("\n\nOpção:");
    scanf("%d", &opcao);
    while (opcao!=1 && opcao!=2 && opcao!=0){
      printf("\n\nOpção:");
      scanf("%d", &opcao);
    }
    if(opcao==1){
      c1.valor=c1.saldo/N;
      for(i = 0; i < 2; i++) {
        sem_init(&semaforo[i], 1, 1); 
      }
      int ids[N];
      for(i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&transfer[i], NULL, transacoes1, &ids[i]);
      }   
    } else if (opcao==2){
      c2.valor=c2.saldo/N;
      pthread_t transfer[N]; 
      int i; 
      for(i = 0; i < 2; i++) {
        sem_init(&semaforo[i], 1, 1); 
      }
      int ids[N];
      for(i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&transfer[i], NULL, transacoes1, &ids[i]);
      }  
    }

    for(i = 0; i < N; i++) { //aguarda o término da thread. Se algum valor for especificado no segundo campo da chamada, retorna um valor da thread
        pthread_join(transfer[i], NULL); 
    }

    for(i = 0; i < N; i++) { 
        sem_destroy(&semaforo[i]); //destrói os semáforos e libera os recursos para o sistema operacional
    }
  }
    return 0;
}