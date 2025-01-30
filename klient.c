#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include "klient.h"
#include "ratownik.h"
#include "konfiguracja.h"

const char *nazwa_basenu[] = {"Olimpijski", "Rekreacyjny", "Brodzik"};

int numer_basen;
int numer_klienta;

void opusc_basen(int sig) {
    printf("Klient %d: Otrzymalem sygnal ewakuacji, opuszczam basen %d!\n", numer_klienta, numer_basen);
    exit(0); // Klient opuszcza program
}

void wroc_na_basen(int sig) {
    printf("Klient %d: Otrzymalem sygnal, moge wrocic na basen %d!\n", numer_klienta, numer_basen);
}

void zamkniecie_obiektu(int sig){
        printf("Klient %d: Centrum zamkniete opuszczam obiekt!\n", numer_klienta);
        exit(0);
}

// Funkcja obliczająca średnią wieku w basenie rekreacyjnym
double oblicz_srednia(DaneRekreacyjny *dane) {
    if (dane->liczba_osob == 0) {
        return 0.0;
    }
    return (double)dane->suma_wiekow / dane->liczba_osob;
}

void wejdz_na_basen(int wiek, int sem_id, int sem_num, int is_vip, int numer, DaneRekreacyjny *rekreacyjny, int is_opiekun) {
    int proby = 0;
    numer_basen = sem_num;//przypisanie basenu do ktorego wchodzi klient
    numer_klienta = numer;
    setpgid(0, -numer_basen - 1);//Klient dolacza do grupy swojego basenu

     // Rejestracja obsługi sygnałów
    signal(SIGUSR1, opusc_basen);
    signal(SIGUSR2, wroc_na_basen);
    signal(SIGTERM, zamkniecie_obiektu);

    while (proby <= 5) {
        proby++;

        // Sprawdzenie czy basen jest otwarty (wartość semafora > 0)
        int sem_val = semctl(sem_id, sem_num, GETVAL);
        if (sem_val == 0){
                if(is_vip){
                        printf("%sVIP #%d (wiek: %d): Basen zamkniety, przekierowanie na inny basen%s\n", YELLOW, numer, wiek, RESET);
                }else{
                        printf("Klient #%d: Basen %s zamknięty, próbuję inny...\n", numer, nazwa_basenu[sem_num]);
                }
                sem_num = (sem_num + 1) % 3; // Przekierowanie na inny basen
                sleep(1);
                proby++;
        }


        // Dzieci poniżej 10 lat mogą korzystać tylko z brodzika lub basenu rekreacyjnego (od 5 lat)
         if (sem_num == SEM_BRODZIK && wiek > 10 && !is_opiekun) {
            if(is_vip){
                    printf("%sVIP #%d (wiek %d): Za stary na brodzik zmieniam basen%s\n", YELLOW, numer, wiek, RESET);
            }else{
            printf("Klient #%d (wiek %d): Za stary na brodzik, zmieniam basen.\n", numer, wiek);
            }
            if(wiek < 18){
                sem_num = 1; // Przekierowanie na basen rekreacyjny
                continue;
            }else{
                sem_num = rand() % 2; // Losowanie pomiędzy rekreacyjnym i olimpijskim
                continue;
            }
        }

        if(sem_num == SEM_BRODZIK && wiek < 3){
                printf("Klient #%d (wiek %d): Korzystam z brodzika i plywam w pampersach.\n", numer, wiek);
        }

        // Dzieci poniżej 10 lat mogą korzystać tylko z brodzika lub basenu rekreacyjnego (od 5 lat)
        if (wiek < 5 && sem_num == SEM_REKREACYJNY) {
            printf("Klient #%d: Jesteś za młody na basen rekreacyjny (wiek: %d).\n", numer, wiek);
            sem_num = SEM_BRODZIK; // Przekierowanie na brodzik
            continue;
        }


        //Osoby ponizej 10 lat nie moga korzystac z basenu olimpijskiego
        if(sem_num == SEM_OLIMPIJSKI && wiek < 18){
                printf("Klient #%d (wiek %d): jestes za mlody na basen olipisjki\n", numer, wiek);
                sem_num = rand() % 2 + 1;
                continue;
        }

        // Sprawdzenie średniej wieku dla basenu rekreacyjnego
        if (sem_num == SEM_REKREACYJNY) {
            double srednia = oblicz_srednia(rekreacyjny);
            if ((srednia + wiek) / (rekreacyjny->liczba_osob + 1) > 40.0) {
                if(is_vip){
                        printf("%sVIP #%d (wiek: %d): Srednia wieku w basenie rekreacyjnym przekroczylaby 40 lat, przekierowanie...%s\n", YELLOW, numer, wiek, RESET);
                }else{
                        printf("Klient #%d: Średnia wieku w basenie rekreacyjnym przekroczyłaby 40 lat, przekierowanie...\n", numer);
                }
                sem_num = (rand() % 2) * 2;  // Przekierowanie na inny basen, losowanie miedzy 0 a 2
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

