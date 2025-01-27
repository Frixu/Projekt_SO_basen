#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>
#include "konfiguracja.h"

#define PATHNAME "konfiguracja.h"

const char *nazwa_basenu[] = {"Olimpijski", "Rekreacyjny", "Brodzik"};

// Struktura do przechowywania danych o średniej wieku
typedef struct {
    int suma_wiekow; // Suma wieku wszystkich osób w basenie rekreacyjnym
    int liczba_osob; // Liczba osób w basenie rekreacyjnym
} DaneRekreacyjny;

// Funkcja obliczająca średnią wieku w basenie rekreacyjnym
double oblicz_srednia(DaneRekreacyjny *dane) {
    if (dane->liczba_osob == 0) {
        return 0.0;
    }
    return (double)dane->suma_wiekow / dane->liczba_osob;
}

void wejdz_na_basen(int wiek, int sem_id, int sem_num, int is_vip, int numer, DaneRekreacyjny *rekreacyjny, int is_opiekun, int *licznik_klientow) {
    int proby = 0;

    struct sembuf lock = {SEM_LOCK, -1, 0}; // Zablokowanie semafora
    struct sembuf unlock = {SEM_LOCK, 1, 0}; // Odblokowanie semafora

    // Zwiększenie licznika klientów
    semop(sem_id, &lock, 1);
    (*licznik_klientow)++;
    semop(sem_id, &unlock, 1);

    while (proby <= MAX_PROBY) {

        // Dzieci poniżej 10 lat mogą korzystać tylko z brodzika lub basenu rekreacyjnego (od 5 lat)
        if (sem_num == SEM_BRODZIK && wiek > 10 && !is_opiekun) {
            printf("Klient #%d (wiek %d): Za stary na brodzik, zmieniam basen.\n", numer, wiek);
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

        // Sprawdzenie średniej wieku dla basenu rekreacyjnego
        if (sem_num == SEM_REKREACYJNY) {
            double srednia = oblicz_srednia(rekreacyjny);
            if ((srednia + wiek) / (rekreacyjny->liczba_osob + 1) > 40.0) {
                printf("Klient #%d: Średnia wieku w basenie rekreacyjnym przekroczyłaby 40 lat, przekierowanie...\n", numer);
                sem_num = (sem_num + 1) % 3; // Przekierowanie na inny basen
                continue;
            }
        }

        if(sem_num == SEM_OLIMPIJSKI && wiek < 18){
            printf("Klient #%d (wiek: %d): Jesteś za młody na basen olimpijski.\n", numer, wiek);
            sem_num = (sem_num + 1) % 3; // Przekierowanie na inny basen
            continue;
        }


        // Próba wejścia na basen
        struct sembuf op = {sem_num, -1, 0};
        if (semop(sem_id, &op, 1) == -1) {
                if (is_vip) {
                        // VIP z komunikatem w kolorze żółtym
                        printf("%sVIP #%d (wiek %d): Brak miejsca na basenie %s, próba przekierowania..%s\"", YELLOW, numer, wiek, nazwa_basenu[sem_num], RESET);
                 }else{
                // Zwykły klient
                         printf("Klient #%d (wiek %d): Brak miejsca na basenie %s, próba przekierowania...\n" ,numer, wiek, nazwa_basenu[sem_num]);
                }
                sem_num = (sem_num + 1) % 3; // Przekierowanie na kolejny basen
                continue;
}

        // Aktualizacja danych o średniej wieku w basenie rekreacyjnym
        if (sem_num == SEM_REKREACYJNY) {
            rekreacyjny->suma_wiekow += wiek;
            rekreacyjny->liczba_osob++;
        }

        // Klient wchodzi na basen
         // Klient wchodzi na basen
        if (is_vip) {
                printf("%sVIP #%d (wiek %d): Korzysta z basenu %s%s\n", YELLOW, numer, wiek, nazwa_basenu[sem_num], RESET);
        } else {
                printf("Klient #%d (wiek %d): Korzysta z basenu %s\n", numer, wiek, nazwa_basenu[sem_num]);
}

        sleep(CZAS_BILETU);

        // Klient opuszcza basen

        if(semop(sem_id, &lock, 1) == -1){
                perror("Blad przy zwalnianiu semafora");
        }else{
                (*licznik_klientow)--;
                if (is_vip) {
                         printf("%sVIP #%d (wiek %d): Opuszcza basen %s%s\n",YELLOW, numer, wiek, nazwa_basenu[sem_num], RESET);
                }else{
                        printf("Klient #%d (wiek %d): Opuszcza basen %s\n", numer, wiek, nazwa_basenu[sem_num]);
                }
                semop(sem_id, &unlock, 1);
        }


        // Aktualizacja danych po opuszczeniu basenu rekreacyjnego
        if (sem_num == SEM_REKREACYJNY) {
            rekreacyjny->suma_wiekow -= wiek;
            rekreacyjny->liczba_osob--;
        }
        break;
        }

        if(proby >= MAX_PROBY){
                if(proby >= MAX_PROBY){
             printf("Klient #%d: Nie mogłem znaleźć miejsca na basenie. Opuszczam pływalnię.\n" , numer);
    }
}
}

int main() {
    srand(getpid());

    //Pamiec wspoldzielona do numeru klienta
    key_t shm_key_numer = ftok(PATHNAME, 'L');
    if(shm_key_numer == -1){
            perror("Klient: Nie udało się wygenerować klucza pamięci współdzielonej");
            exit(1);
    }

    printf("Wygenerowany klucz: %d\n", shm_key_numer);

    int shm_id_numer = shmget(shm_key_numer, MAX_KLIENCI * sizeof(int), 0666 | IPC_CREAT);
    if(shm_id_numer == -1){
            perror("Klient: Blad podczas tworzenia pamieci wspoldzielonej dla klient_numer");
            exit(1);
    }

    int *klient_numer = shmat(shm_id_numer, NULL, 0);
    if(klient_numer == (void *)-1){
            perror("Klient: Blad podczas dolaczania pamieci wspoldzielonej");
            exit(1);
    }
    *klient_numer = 0;

    int sem_id = semget(SEM_KEY, 4, 0 | IPC_CREAT);
    if (sem_id == -1) {
        perror("Błąd semaforów");
        return 1;
    }


    //Pamiec wspoldzielona do licznika klientow
    key_t shm_key_licznik = ftok(PATHNAME, 'K');
    if(shm_key_licznik == -1){
            perror("Klient: Nie udalo sie wygenerowac klucza do pamieci wspoldzielonej-licznik");
            exit(1);
    }

    int shm_id_licznik = shmget(shm_key_licznik, sizeof(int), 0666 | IPC_CREAT);
    if (shm_id_licznik == -1) {
        perror("Klient: Nie udało się utworzyć pamięci współdzielonej dla licznika klientów");
        exit(1);
    }
    int *licznik_klientow = (int *)shmat(shm_id_licznik, NULL, 0);
    if(licznik_klientow == (void *)-1){
            perror("Klient: Nie udalo sie dolaczyc pamieci wspoldzielonej licznika klientow");
            exit(1);
    }

    *licznik_klientow = 0;


    // Pamięć współdzielona dla danych o basenie rekreacyjnym
    key_t rek_key = ftok(PATHNAME, 'R');
    int rek_id = shmget(rek_key, sizeof(DaneRekreacyjny), 0666 | IPC_CREAT);
    DaneRekreacyjny *rekreacyjny = (DaneRekreacyjny *)shmat(rek_id, NULL, 0);
    rekreacyjny->suma_wiekow = 0;
    rekreacyjny->liczba_osob = 0;

    for (int i = 0; i < MAX_KLIENCI; i++) {
        pid_t pid = fork();

        if(pid < 0){
                perror("Blad podczas tworzenia procesu");
                exit(1);
        }

        if (pid == 0) {
            int wiek = rand() % 70 + 1; // Wiek klienta od 1 do 70 lat
            int basen = rand() % 3;     // Losowy wybór basenu
            int is_vip = (wiek >= 18 && rand() % 5 == 0); // VIP tylko dla klientów 18+

            struct sembuf op = {0, -1, 0};
            if(semop(sem_id, &op, 1) == -1){
                    perror("Blad przy blokowaniu semafora");
                    goto cleanup;
            }

            int numer = ++(*klient_numer); // Inkrementacja

            cleanup:
            op.sem_op = 1;
            if(semop(sem_id, &op, 1) == -1){
                    perror("Blad przy odblokowywaniu semafora");
            }

            if (wiek < 10) {
                printf("Dziecko #%d (wiek %d): Potrzebny opiekun!\n", numer, wiek);

                pid_t opiekun_pid = fork();
                if(opiekun_pid < 0){
                        perror("Blad podczas tworzenia procesu opiekuna");
                        exit(1);
                }

                if (opiekun_pid == 0) { // Proces opiekuna
                        int wiek_opiekuna = rand() % 50 + 30; // Opiekun ma wiek 30-70 lat
                        printf("%sOpiekun dla dziecka #%d: Wiek opiekuna %d%s\n", GREEN, numer, wiek_opiekuna, RESET);

                        wejdz_na_basen(wiek_opiekuna, sem_id, SEM_BRODZIK, is_vip, ++(*klient_numer), rekreacyjny, 1, licznik_klientow);
                        exit(0); // Proces opiekuna kończy działanie
            }
        }

            wejdz_na_basen(wiek, sem_id, basen, is_vip, numer, rekreacyjny, 0, licznik_klientow);
            exit(0);
        }
        sleep(rand() % 5);
    }

    for (int i = 0; i < MAX_KLIENCI; i++){
            wait(NULL);
    }

    // Usuwanie pamięci współdzielonej
    if (shmdt(klient_numer) == -1) {
        perror("Blad podczas odlaczania pamieci wspoldzielonej klient_numer");
    }
    if (shmdt(rekreacyjny) == -1) {
        perror("Blad podczas odlaczania pamieci wspoldzielonej rekreacyjny");
    }
    if (shmdt(licznik_klientow) == -1) {
        perror("Blad podczas odlaczania pamieci wspoldzielonej licznik_klientow");
    }

    shmctl(shm_id_numer, IPC_RMID, NULL);
    shmctl(rek_id, IPC_RMID, NULL);
    shmctl(shm_id_licznik, IPC_RMID, NULL);
    return 0;
}
