#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "konfiguracja.h"

int sem_id;

void monitoruj_brodzik(int wiek) {
    if (CZY_BRODZIK(wiek) || wiek > MAX_WIEK_BRODZIK) {
        printf("Ratownik: Brodzik dostępny dla dzieci i opiekunów.\n");
    }
}

void zamknij_basen(int sig) {
    printf("Ratownik: Zamykanie basenów\n");
    semctl(sem_id, SEM_OLIMPIJSKI, SETVAL, 0);
    semctl(sem_id, SEM_REKREACYJNY, SETVAL, 0);
    semctl(sem_id, SEM_BRODZIK, SETVAL, 0);
}

void otworz_basen(int sig) {
    printf("Ratownik: Otwarcie basenów\n");
    semctl(sem_id, SEM_OLIMPIJSKI, SETVAL, MAX_OLIMPIJSKI);
    semctl(sem_id, SEM_REKREACYJNY, SETVAL, MAX_REKREACYJNY);
    semctl(sem_id, SEM_BRODZIK, SETVAL, MAX_BRODZIK);
}

int main() {
    sem_id = semget(SEM_KEY, 0, 0);
    if (sem_id == -1) {
        perror("Nie udało się otworzyć semaforów");
        return 1;
    }

    signal(SIGUSR1, zamknij_basen);
    signal(SIGUSR2, otworz_basen);

    while (1) {
        pause(); // Czekanie na sygnał
    }

    return 0;
}
