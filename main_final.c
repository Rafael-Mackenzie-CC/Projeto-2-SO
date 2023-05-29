/* Alcir Canella Filho
   Heitor Jody
   Rafael Yun
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //habilita funções de thread
#include <semaphore.h> // habilita funções de semáforo
#include <unistd.h> //habilita a função sleep
#include <malloc.h>
#include <sys/types.h>
//número de filósofos/garfos
#define N 50 
struct c {
 int saldo;
 int valor;
};

//Problema: Utilizando o exemplo e os conceitos apresentados em sala, implemente uma solução para a condição de corrida apresentada no problema.

//Solução encontrada: Utilizar exclusão mútua para solucionar a condição de corrida a partir do uso de semáforos binários(com dois estados) para garantir a integridade das contas, travando a conta de origem através de um semáforo  para retirar saldo e liberando o mesmo após a retirada do saldo. Depois, a thread tentará acesso à conta de destino, através de um segundo semáforo. Se ele estiver livre, a thread torna o semáforo ocupado e aplica o valor retirado da conta de origem, liberando o semáforo após concluir a transação 

/*Colaborações de cada membro:
Alcir Canella Filho: Uso de Pthreads e desenvolvimento da lógica inicial (Função que transfere dinheiro da conta 1 para conta 2)
Heitor Jody: Desenvolvimento da lógica reversa em conjunto com Rafael (conta 2 para conta 1) e validador da transação.
Rafael Yun: Fórmula principal (uso de 50 threads para cada lógica de transferência, sendo 50 da conta 1 para conta 2, e 50 da conta 2 para a conta 1, totalizando 100)  em conjunto com Heitor.
*/
typedef struct c conta;
conta c1, c2;

sem_t semaforo[2]; // vetor de semáforos para as contas 1 e 2

void *transacoes1(void *arg) {   //Transfere da conta 1 para conta 2
  int id = *(int *)arg;
  int transacao_valida=0; //flag para verificar se o dinheiro foi retirado da conta de origem
  sem_wait(&semaforo[0]); 
  if (c1.saldo > 0){//verifica se há saldo para retirada
    c1.saldo -= 2;
    transacao_valida=1;} //o dinheiro foi retirado da conta de origem, logo a flag foi atualizada para 1
  sem_post(&semaforo[0]); 
  sem_wait(&semaforo[1]);
  if (transacao_valida==1){ //se a flag for igual a 1, faz o depósito na conta de destino
      c2.saldo += 2;
      printf("Transferência %d concluída com sucesso!\n", id + 1);
      printf("Saldo de c1: %d\n", c1.saldo);
      printf("Saldo de c2: %d\n", c2.saldo);}
  sem_post(&semaforo[1]); // libera o semáforo
  
}
void *transacoes2(void *arg) { //Transfere da conta 2 para conta 1
  int id = *(int *)arg;
  int transacao_valida=0; //flag para verificar se o dinheiro foi retirado da conta de origem
  sem_wait(&semaforo[1]);
  if (c2.saldo > 0){ //verifica se há saldo para retirada
    c2.saldo -= 2;
    transacao_valida=1;} //o dinheiro foi retirado da conta de origem, logo a flag foi atualizada para 1
  sem_post(&semaforo[1]);
  sem_wait(&semaforo[0]);
  if (transacao_valida==1){//se a flag for igual a 1, faz o depósito na conta de destino
      c1.saldo += 2;
      printf("Transferência %d concluída com sucesso!\n", id + 1);
      printf("Saldo de c1: %d\n", c1.saldo);
      printf("Saldo de c2: %d\n", c2.saldo);}
  sem_post(&semaforo[0]); // libera o semáforo
  
}

int main() {
  // Todas as contas começam com saldo 100
  c1.saldo = 100;
  c2.saldo = 100;
  pthread_t transfer[N];
  int i;
  int opcao;
  int ids[N];
  // c1.valor=c1.saldo/N;
  for (i = 0; i < 2; i++) {
    sem_init(&semaforo[i], 1, 1);
  }

  for (i = 0; i < N; i++) { //criação de threads para as transações da conta 1 para a conta 2
    ids[i] = i;
    pthread_create(&transfer[i], NULL, transacoes1, &ids[i]);
  }

  for (i = 0; i < N; i++) { //criação de threads para as transações da conta 2 para a conta 1
    ids[i] = i;
    pthread_create(&transfer[i], NULL, transacoes2, &ids[i]);
  }

  for (i = 0; i < N;
       i++) { // aguarda o término da thread. Se algum valor for especificado no
              // segundo campo da chamada, retorna um valor da thread
    pthread_join(transfer[i], NULL);
  }

  for (i = 0; i < N; i++) {
    sem_destroy(&semaforo[i]); // destrói os semáforos e libera os recursos para
                               // o sistema operacional
  }
  return 0;
}