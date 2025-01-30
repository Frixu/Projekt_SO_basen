
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include "klient.h"
#include "konfiguracja.h"

const char *nazwa_basenu[] = {"Olimpijski", "Rekreacyjny", "Brodzik"};

// Funkcja obliczająca średnią wieku w basenie rekreacyjnym
double oblicz_srednia(DaneRekreacyjny *dane) {
    if (dane->liczba_osob == 0) {
        return 0.0;
    }
    return (double)dane->suma_wiekow / dane->liczba_osob;
}

void inicjalizuj_sygnaly() {
    signal(SIGUSR1, ewakuacja);
}

void wejdz_na_basen(int wiek, int sem_id, int sem_num, int is_vip, int numer, DaneRekreacyjny *rekreacyjny, int is_opiekun) {
    inicjalizuj_sygnaly();
    int proby = 0;
    while (proby <= 5) {
        proby++;

        // Sprawdzenie czy basen jest otwarty (wartość semafora > 0)
        int sem_val = semctl(sem_id, sem_num, GETVAL);
        if (sem_val == 0) {
            printf("Klient #%d: Basen %d jest zamknięty, czekam...\n", numer, sem_num);
            sleep(3);
            continue;
        }

        // Dzieci poniżej 10 lat mogą korzystać tylko z brodzika lub basenu rekreacyjnego (od 5 lat)
        if (sem_num == SEM_BRODZIK && wiek > 10 && !is_opiekun) {
            printf("Klient #%d (wiek %d): Za stary na brodzik, zmieniam basen.\n", numer, wiek);
            if (wiek < 18) {
                sem_num = 1;  // Przekierowanie na basen rekreacyjny
                continue;
            } else {
                sem_num = rand() % 2;  // Losowanie pomiędzy rekreacyjnym i olimpijskim
                continue;
            }
        }

        if (sem_num == SEM_BRODZIK && wiek < 3) {
            printf("Klient #%d (wiek %d): Korzystam z brodzika i plywam w pampersach.\n", numer, wiek);
        }

        // Dzieci poniżej 10 lat mogą korzystać tylko z brodzika lub basenu rekreacyjnego (od 5 lat)
        if (wiek < 5 && sem_num == SEM_REKREACYJNY) {
            printf("Klient #%d: Jesteś za młody na basen rekreacyjny (wiek: %d).\n", numer, wiek);
            sem_num = SEM_BRODZIK;  // Przekierowanie na brodzik
            continue;
        }

        // Sprawdzenie średniej wieku dla basenu rekreacyjnego
        if (sem_num == SEM_REKREACYJNY) {
            double srednia = oblicz_srednia(rekreacyjny);
            if ((srednia + wiek) / (rekreacyjny->liczba_osob + 1) > 40.0) {
                printf("Klient #%d: Średnia wieku w basenie rekreacyjnym przekroczyłaby 40 lat, przekierowanie...\n", numer);
                sem_num = (sem_num + 1) % 3;  // Przekierowanie na inny basen
                continue;
            }
        }

        // Próba wejścia na basen
        struct sembuf op = {sem_num, -1, 0};
        if (semop(sem_id, &op, 1) == -1) {
            if (is_vip) {
                // VIP z komunikatem w kolorze żółtym
                printf("%sVIP #%d (wiek %d): Brak miejsca na basenie %s, próba przekierowania..%s\"", YELLOW, numer, wiek, nazwa_basenu[sem_num], RESET);
            } else {
                // Zwykły klient
                printf("Klient #%d (wiek %d): Brak miejsca na basenie %s, próba przekierowania...\n", numer, wiek, nazwa_basenu[sem_num]);
            }
            sem_num = (sem_num + 1) % 3;  // Przekierowanie na kolejny basen
            continue;
        }

        // Aktualizacja danych o średniej wieku w basenie rekreacyjnym
        if (sem_num == SEM_REKREACYJNY) {
            rekreacyjny->suma_wiekow += wiek;
            rekreacyjny->liczba_osob++;
        }

        // Klient wchodzi na basen
        if (is_vip) {
            printf("%sVIP #%d (wiek %d): Korzysta z basenu %s%s\n", YELLOW, numer, wiek, nazwa_basenu[sem_num], RESET);
        } else {
            printf("Klient #%d (wiek %d): Korzysta z basenu %s\n", numer, wiek, nazwa_basenu[sem_num]);
        }

        sleep(CZAS_BILETU);

        // Klient opuszcza basen
        op.sem_op = 1;
        semop(sem_id, &op, 1);
        if (is_vip) {
            printf("%sVIP #%d (wiek %d): Opuszcza basen %s%s\n", YELLOW, numer, wiek, nazwa_basenu[sem_num], RESET);
        } else {
            printf("Klient #%d (wiek %d): Opuszcza basen %s\n", numer, wiek, nazwa_basenu[sem_num]);
        }

        // Aktualizacja danych po opuszczeniu basenu rekreacyjnego
        if (sem_num == SEM_REKREACYJNY) {
            rekreacyjny->suma_wiekow -= wiek;
            rekreacyjny->liczba_osob--;
        }
        break;
    }
}
