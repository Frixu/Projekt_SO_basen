#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include "kasjer.h"
#include "konfiguracja.h"

void inicjalizuj_semafory(int sem_id) {
    // Inicjalizacja semaforów
    if(semctl(sem_id, SEM_OLIMPIJSKI, SETVAL, MAX_OLIMPIJSKI) == -1){
            perror("Kasjer: Blad przy ustawianiu wartosci semafora SEM_OLIMPIJSKI");
            exit(1);
    }
    if(semctl(sem_id, SEM_REKREACYJNY, SETVAL, MAX_REKREACYJNY) == -1){
            perror("Kasjer: Blad przy ustawianiu wartosci semafora SEM_REKREACYJNY");
            exit(1);
    }
    if(semctl(sem_id, SEM_BRODZIK, SETVAL, MAX_BRODZIK) == -1){
            perror("Kasjer: Blad przy ustawianiu wartosci semafora SEM_BRODZIK");
            exit(1);
    }

    printf("Kasjer: Semafory zostały zainicjowane.\n");
}

void usun_semafory(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID, 0) == -1) {
        perror("Kasjer: Nie udało się usunąć semaforów");
        exit(1);
    }
    printf("Kasjer: Semafory zostały usunięte.\n");
}

int czy_wszystkie_baseny_puste(int sem_id) {
    int olimpijski = semctl(sem_id, SEM_OLIMPIJSKI, GETVAL);
    if(olimpijski == -1){
            perror("Kasjer: Blad przy odczycie semafora SEM_OLIMPISJKI");
            exit(1);
    }

    int rekreacyjny = semctl(sem_id, SEM_REKREACYJNY, GETVAL);
    if(rekreacyjny == -1){
            perror("Kasjer: Blad przy odczycie semafora SEM_REKREACYJNY");
            exit(1);
    }

    int brodzik = semctl(sem_id, SEM_BRODZIK, GETVAL);
    if(brodzik == -1){
            perror("Kasjer: Blad przy odczycie semafora SEM_BRODZIK");
            exit(1);
    }
    return (olimpijski == MAX_OLIMPIJSKI && rekreacyjny == MAX_REKREACYJNY && brodzik == MAX_BRODZIK);
}
