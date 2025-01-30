
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "ratownik.h"

pid_t ratownicy[3];

void rozpocznij_prace_ratownikow(int sem_id) {
    for (int i = 0; i < LICZBA_BASENOW; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            setpgid(0, -i -1);
            while(1){
            pause();
            }
        } else if (pid < 0) {
            perror("Blad przy tworzeniu procesu ratownika");
            exit(1);
        }
        ratownicy[i] = pid; // Zapisanie pid ratownika
    }
     printf("Procesy ratowników uruchomione i oczekują na polecenia!\n");
}

void zakoncz_prace_ratownikow(){
        printf("Ratownik: Koniec pracy\n");
        for(int i = 0; i < LICZBA_BASENOW; i++){
                if(ratownicy[i] > 0){
                        kill(ratownicy[i], SIGTERM);
                }
        }
}


void zamknij_basen(int numer_basen, int sem_id) {
    printf("Ratownik: Zamykam basen %d!\n", numer_basen);
    semctl(sem_id, numer_basen, SETVAL, 0);
    killpg(-numer_basen - 1, SYGNAL_OPUSC_BASEN);
}

void otworz_basen(int numer_basen, int sem_id) {
    printf("Otwieram basen %d!\n", numer_basen);
    semctl(sem_id, numer_basen, SETVAL, 1);
    killpg(-numer_basen - 1, SYGNAL_WROC_NA_BASEN);
}

void zamknij_cale_centrum(int sem_id) {
    printf("⚠️ Zamykam całe centrum pływackie!\n");
    for(int i = 0; i < LICZBA_BASENOW; i++){
        semctl(sem_id, i, SETVAL, 0);
    }
    killpg(getpgrp(), SYGNAL_ZAMKNIJ_OBIEKT);
}

void otworz_cale_centrum(int sem_id) {
    printf("Całe centrum pływackie ponownie otwarte!\n");
    for(int i = 0; i < LICZBA_BASENOW; i++){
            semctl(sem_id, i, SETVAL, 1);
    }
    killpg(getpgrp(), SYGNAL_WROC_NA_BASEN);
}

void menu_ratownika(int sem_id) {
    int wybor;
    do{
        printf("\n===== MENU RATOWNIKA =====\n");
        printf("1. Zamknij basen Olimpijski\n");
        printf("2. Zamknij basen Rekreacyjny\n");
        printf("3. Zamknij brodzik\n");
        printf("4. Otwórz basen Olimpijski\n");
        printf("5. Otwórz basen Rekreacyjny\n");
        printf("6. Otwórz brodzik\n");
        printf("7. Zamknij całe centrum\n");
        printf("8. Otwórz całe centrum\n");
        printf("9. Wyjście\n");
        printf("Twój wybór: ");
        scanf("%d", &wybor);

        switch (wybor) {
            case 1:
                    zamknij_basen(0, sem_id);
                    break;
            case 2:
                    zamknij_basen(1, sem_id);
                    break;
            case 3:
                    zamknij_basen(2, sem_id);
                    break;
            case 4:
                    otworz_basen(0, sem_id);
                    break;
            case 5:
                    otworz_basen(1, sem_id);
                    break;
            case 6:
                    otworz_basen(2, sem_id);
                    break;
            case 7:
                    zamknij_cale_centrum(sem_id);
                    break;
            case 8:
                    otworz_cale_centrum(sem_id);
                    break;
            case 9:
                    zakoncz_prace_ratownikow();
                    exit(0);
            default:
                    printf("❌ Niepoprawny wybór!\n");
        }
    }while(wybor != 9);
}

int main() {
    key_t sem_key = ftok("konfiguracja.h", 'A');
    int sem_id = semget(sem_key, LICZBA_BASENOW, 0666);
    if (sem_id == -1) {
        perror("Blad przy pobieraniu semaforó");
        exit(1);
    }

    rozpocznij_prace_ratownikow(sem_id);
    menu_ratownika(sem_id);
        return 0;
}
