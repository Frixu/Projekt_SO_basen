#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "klient.h"
#include "kasjer.h"
#include "ratownik.h"
#include "utils.h"
#include "konfiguracja.h"

#define PATHNAME "./konfiguracja.h"

int sem_id, shm_id, rek_id;
int *klient_numer;
DaneRekreacyjny *rekreacyjny;

void *czyszczenie_zombie(void *arg) {
    while (1) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        while (pid > 0) {
            pid = waitpid(-1, &status, WNOHANG); // Sprawdzenie kolejnych zombie
        }
        sleep(2); // Co 2 sekundy sprawdzamy, czy są zombie
    }
    return NULL;
}

int main() {
        srand(getpid());//Inicjalizacja generatora liczb losowych

        //Rejestracja sygnałów
        signal(SIGTERM, zamkniecie_obiektu);
        signal(SIGINT, handler_sigint);

        //Uruchomienie wątku do czyszczenia procesów zombie
        pthread_t zombie_thread;
        if (pthread_create(&zombie_thread, NULL, czyszczenie_zombie, NULL) != 0) {
                perror("Błąd przy tworzeniu wątku do czyszczenia procesów zombie");
                exit(1);
        }

        //Tworznie kluczy
        key_t sem_key = get_sem_key(PATHNAME, 'A');
        key_t shm_key = get_shm_key(PATHNAME, 'B');

        // Tworzenie semaforów
        int sem_id = create_semaphore(sem_key, 3);

        // Inicjalizacja semaforów
        inicjalizuj_semafory(sem_id);

         // Tworzenie pamięci współdzielonej dla numeru klienta
    int shm_id = shmget(shm_key, MAX_KLIENCI * sizeof(int), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("Klient: Błąd przy tworzeniu pamięci współdzielonej dla numeru klienta");
        exit(1);
    }else{
            printf("Pamiec wspoldzielona zaincjowana\n");
    }

    int *klient_numer = shmat(shm_id, NULL, 0);
    if (klient_numer == (void *)-1) {
        perror("Klient: Błąd przy łączeniu pamięci współdzielonej dla numeru klienta");
        exit(1);
    }
    *klient_numer = 0; // Inicjalizacja numeru klienta

    // Pamięć współdzielona dla danych o basenie rekreacyjnym
    key_t rek_key = ftok(PATHNAME, 'R');
    int rek_id = shmget(rek_key, sizeof(DaneRekreacyjny), 0666 | IPC_CREAT);
    if (rek_id == -1) {
        perror("Klient: Błąd przy tworzeniu pamięci współdzielonej dla basenu rekreacyjnego");
        exit(1);
    }
    DaneRekreacyjny *rekreacyjny = (DaneRekreacyjny *)shmat(rek_id, NULL, 0);
    if (rekreacyjny == (void *)-1) {
        perror("Klient: Błąd przy łączeniu pamięci współdzielonej dla basenu rekreacyjnego");
        exit(1);
    }

    rekreacyjny->suma_wiekow = 0; // Inicjalizacja danych o basenie
    rekreacyjny->liczba_osob = 0;

    // Tworzenie procesów dla klientów
    for (int i = 0; i < MAX_KLIENCI; i++) {
        srand(getpid());
        pid_t pid = fork();
        if (pid < 0) {
            perror("Klient: Błąd przy tworzeniu procesu");
            exit(1);
        }

        if (pid == 0) { // Dziecko - klient
            int wiek = rand() % 70 + 1; // Losowy wiek klienta (1-70 lat)
            int basen = rand() % 3;     // Losowy wybór basenu
            int is_vip = (wiek >= 18 && rand() % 5 == 0); // VIP tylko dla osób 18+

            struct sembuf op = {0, -1, 0};
            if (semop(sem_id, &op, 1) == -1) {
                perror("Klient: Błąd przy blokowaniu semafora");
                killpg(getpgrp(), SIGTERM);
                exit(1);
            }

            int numer = ++(*klient_numer); // Inkrementacja numeru klienta

            op.sem_op = 1;
            if (semop(sem_id, &op, 1) == -1) {
                perror("Klient: Błąd przy odblokowywaniu semafora");
                exit(1);
            }

            if (wiek < 10) { // Dzieci potrzebują opiekuna
                printf("Klient(dziecko)#%d (wiek: %d): Potrzebny opiekun!\n", numer, wiek);
                pid_t opiekun_pid = fork();
                if (opiekun_pid < 0) {
                    perror("Klient: Błąd podczas tworzenia procesu opiekuna");
                    exit(1);
                }

                if (opiekun_pid == 0) {
                    int wiek_opiekuna = rand() % 40 + 30; // Losowy wiek opiekuna
                    printf("%sOpiekun dla dziecka #%d: Wiek opiekuna: %d%s\n", GREEN, numer, wiek_opiekuna, RESET);
                    wejdz_na_basen(wiek_opiekuna, sem_id, SEM_BRODZIK, is_vip, ++(*klient_numer), rekreacyjny, 1);
                    exit(0);
                }
            }

            wejdz_na_basen(wiek, sem_id, basen, is_vip, numer, rekreacyjny, 0);
            exit(0);
        }
        sleep(rand() % 3); // Losowy czas przed utworzeniem kolejnego procesu klienta
    }

    // Kasjer sprawdzający co jakiś czas, czy baseny są puste
    pid_t kasjer_pid = fork();
    if (kasjer_pid < 0) {
        perror("Kasjer: Błąd przy tworzeniu procesu kasjera");
        exit(1);
    }

    if (kasjer_pid == 0) { // Dziecko - kasjer
        while (1) {
            sleep(10); // Sprawdzanie co 10 sekund
            if (czy_wszystkie_baseny_puste(sem_id)) {
                printf("Kasjer: Wszystkie baseny są puste. Kończę pracę.\n");
                break;
            }
        }
        exit(0);
    }

    // Czekamy na zakończenie wszystkich procesów klientów
    for (int i = 0; i < MAX_KLIENCI; i++) {
        wait(NULL);
    }

     // Czekamy na zakończenie procesu kasjera
    waitpid(kasjer_pid, NULL, 0);

    // Zatrzymanie wątku czyszczenia zombie
    pthread_cancel(zombie_thread);
    pthread_join(zombie_thread, NULL);

         // Usuwanie pamięci współdzielonej
        usun_pamiec_wspoldzielona(klient_numer, rekreacyjny, shm_id, rek_id);

        // Usuwanie semaforów po zakończeniu pracy
        usun_semafory(sem_id);

        return 0;
}
