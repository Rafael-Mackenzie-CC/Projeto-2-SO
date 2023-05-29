Alcir Canella Filho - TIA: 42146534
Heitor J. I. - TIA: 42111013
Rafael Yun - TIA: 32144636

PROJETO: Baseando-se no exemplo e nos conceitos discutidos em sala, nossa implementação propõe uma solução para a condição de corrida.

A estratégia utilizada consiste em aplicar a exclusão mútua para remediar a condição de corrida, usando semáforos binários (com dois estados) para garantir a integridade das contas. Primeiramente, bloqueamos a conta de origem com um semáforo para a retirada do saldo, liberando-o após a retirada ser efetuada. Em seguida, a thread tentará acessar a conta de destino por meio de um segundo semáforo. Se este estiver disponível, a thread o ocupará, aplicará o valor retirado da conta de origem e, por fim, liberará o semáforo após a conclusão da transação.

1 A conta destino pode receber mais de uma transferência simultânea;
Para isso, empregamos a função pthread_create para criar threads de 0 até N, cada uma representando uma transação. Estas threads são alocadas em um pool para execução simultânea. A criação das threads é exemplificada no trecho de código a seguir, realizando transações da conta 1 para a conta 2:

for (i = 0; i < N; i++) { 
    ids[i] = i;
    pthread_create(&transfer[i], NULL, transacoes1, &ids[i]);
}
2 A conta origem pode realizar mais de uma transferência simultânea;
Nesse caso, utilizamos uma lógica similar à do item anterior, mas no sentido inverso, efetuando transações da conta 2 para a conta 1. Veja o código a seguir:

for (i = 0; i < N; i++) {
    ids[i] = i;
    pthread_create(&transfer[i], NULL, transacoes2, &ids[i]);
}

3 A conta origem só pode enviar dinheiro se houver saldo suficiente;
Para assegurar isso, realizamos uma verificação de saldo antes de efetuar a transferência, conforme ilustrado no código abaixo:

sem_wait(&semaforo[0]);
if (c1.saldo > 0)
    c1.saldo -= 2;
sem_post(&semaforo[0]);

4 A conta destino pode tornar-se a conta origem, ou seja, a conta que inicialmente enviava dinheiro pode passar a recebê-lo, e vice-versa;
Para isso, utilizamos os métodos transacoes1 e transacoes2, responsáveis pela lógica de transferência da conta 1 para a conta 2 e vice-versa. A ordem de execução das threads que empregam esses métodos é definida pelo algoritmo do escalonador, que intercala a ordem das threads conforme as duas lógicas, movendo-as da fila de prontos para a fila de execução.

void *transacoes1(void *arg) {
  int id = *(int *)arg;

  sem_wait(&semaforo[0]);
  if (c1.saldo > 0)
    c1.saldo -= 2;
  sem_post(&semaforo[0]);
  sem_wait(&semaforo[1]);
  c2.saldo += 2;
  sem_post(&semaforo[1]);
}

void *transacoes2(void *arg) {
  int id = *(int *)arg;

  sem_wait(&semaforo[1]);
  if (c2.saldo > 0)
    c2.saldo -= 2;
  sem_post(&semaforo[1]);
  sem_wait(&semaforo[0]);
  c1.saldo += 2;
  sem_post(&semaforo[0]);
}

5 Serão permitidas até 100 transações simultâneas de transferência.
No início do código, definimos a constante N como 50. Este valor é utilizado para determinar o limite máximo de threads que serão passadas para cada um dos dois métodos, totalizando um máximo de 100 transações simultâneas (uma por thread). Os trechos de código abaixo exemplificam a criação das threads para transações entre as duas contas:

for (i = 0; i < N; i++) {
    ids[i] = i;
    pthread_create(&transfer[i], NULL, transacoes1, &ids[i]);
}

for (i = 0; i < N; i++) {
    ids[i] = i;
    pthread_create(&transfer[i], NULL, transacoes2, &ids[i]);
}