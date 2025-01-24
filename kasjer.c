
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "konfiguracja.h"
#define PATHNAME "./konfiguracja.h"

/*typedef struct {
        int licznik_klientow;
}LicznikDanych;*/

void inicjalizuj_semafory(int sem_id) {
    // Inicjalizacja semaforów
    if(semctl(sem_id, SEM_OLIMPIJSKI, SETVAL, MAX_OLIMPIJSKI) == -1){
            perror("Kasjer: Nie udalo sie zainicjowac semafora olimpijskiego");
            exit(1);
    }

    if(semctl(sem_id, SEM_REKREACYJNY, SETVAL, MAX_REKREACYJNY) == -1){
            perror("Kasjer: Nie udalo sie zainicjowac semafora olimpijskiego");
            exit(1);
    }

    if(semctl(sem_id, SEM_BRODZIK, SETVAL, MAX_BRODZIK) == -1){
            perror("Kasjer: Nie udalo sie zainicjowac semafora brodzik");
            exit(1);
    }
    //semctl(sem_id, SEM_LOCK, SETVAL, 1);//Semafor do synchronizacji licznika
    printf("Kasjer: Semafory zostaly zainicjowane.\n");
}

void usun_semafory(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID, 0) == -1) {
        perror("Kasjer: Nie udalo sie usunac semaforow");
        exit(1);
    } else {
        printf("Kasjer: Semafory zostaly usuniete.\n");
    }
}

int main() {
    // Tworzenie semaforów3 semafory: olimpijski, rekreacyjny, brodzik)
    int sem_id = semget(SEM_KEY, LICZBA_SEMAFOROW, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Kasjer: Nie udalo sie utworzyc semaforow");
        return 1;
    }

   key_t shm_key = ftok(PATHNAME, 'L');
   if (shm_key == -1) {
   perror("Kasjer: Nie udało się wygenerować klucza pamięci współdzielonej");
   usun_semafory(sem_id);
   exit(1);
   }

    int shm_id = shmget(shm_key, MAX_KLIENCI * sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Kasjer: Nie udalo sie utworzyc pamieci wspoldzielonej");
        usun_semafory(sem_id);
        return 1;
    }else{
            printf("Kasjer: Pamiec wspoldzielona zainicjalizowana\n");
    }
    /*LicznikDanych *licznik = (LicznikDanych *)shmat(shm_id, NULL, 0);
    if (licznik == (void *)-1) {
        perror("Kasjer: Nie udało się połączyć z pamięcią współdzieloną");
        return 1;
    }

    licznik->licznik_klientow = 0; // Inicjalizacja licznika klientó
    printf("Kasjer: Licznik klientow: %d\n", licznik->licznik_klientow);
*/
    inicjalizuj_semafory(sem_id);

    printf("Kasjer: Oczekiwanie na klientow\n");
    sleep(0);

    // Usunięcie pamięci współdzielonej
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("Kasjer: Nie udało się usunąć pamięci współdzielonej");
    } else {
        printf("Kasjer: Pamięć współdzielona została usunięta.\n");
    }

    usun_semafory(sem_id);

    return 0;
}
