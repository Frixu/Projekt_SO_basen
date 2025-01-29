include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include "konfiguracja.h"
#include "utils.h"

#define PATHNAME "./konfiguracja.h"

void zamknij_basen(int sem_id, int basen) {
    struct sembuf op = {basen, -MAX_KLIENCI, 0};
    semop(sem_id, &op, 1);
    printf("Basen %d został ZAMKNIĘTY!\n", basen);
}

void otworz_basen(int sem_id, int basen) {
    struct sembuf op = {basen, MAX_KLIENCI, 0};
    semop(sem_id, &op, 1);
    printf("Basen %d został OTWARTY!\n", basen);
}

void zamknij_cale_centrum(int sem_id) {
    printf("🚨 Zamykam całe centrum basenowe na czas wymiany wody!\n");
    for (int i = 0; i < 3; i++) {
        zamknij_basen(sem_id, i);
    }
}

void otworz_cale_centrum(int sem_id) {
    printf("Otwieram całe centrum basenowe!\n");
    for (int i = 0; i < 3; i++) {
        otworz_basen(sem_id, i);
    }
}

int main() {
        key_t sem_key = get_sem_key(PATHNAME, 'A');
    int sem_id = semget(sem_key, 3, 0);
    if (sem_id == -1) {
        perror("❌ Błąd przy pobieraniu semaforów");
        exit(1);
    }

    while (1) {
        int wybor;
        printf("\n---  MENU RATOWNIKA ---\n");
        printf("1 - Zamknij basen Olimpijski\n");
        printf("2 - Zamknij basen Rekreacyjny\n");
        printf("3 - Zamknij Brodzik\n");
        printf("4 - Otwórz basen Olimpijski\n");
        printf("5 - Otwórz basen Rekreacyjny\n");
        printf("6 - Otwórz Brodzik\n");
        printf("7 - Zamknij całe centrum\n");
        printf("8 - Otwórz całe centrum\n");
        printf("9 - Wyjdź\n");
        printf("Wybór: ");
        fflush(stdout);

        if(scanf("%d", &wybor) != 1){
                printf("Niepoprawny wybór, spróbuj ponownie!\n");
            while (getchar() != '\n');  // Czyści bufor wejściowy (np. w przypadku liter)
            continue;
        }

         while (getchar() != '\n');  // Usuwa nadmiarowe znaki (np. Enter)

        switch (wybor) {
            case 1:
                    zamknij_basen(sem_id, SEM_OLIMPIJSKI);
                    break;
            case 2:
                    zamknij_basen(sem_id, SEM_REKREACYJNY);
                    break;
            case 3:
                    zamknij_basen(sem_id, SEM_BRODZIK);
                    break;
            case 4:
                    otworz_basen(sem_id, SEM_OLIMPIJSKI);
                    break;
            case 5:
                    otworz_basen(sem_id, SEM_REKREACYJNY);
                    break;
            case 6:
                    otworz_basen(sem_id, SEM_BRODZIK);
                    break;
            case 7:
                    zamknij_cale_centrum(sem_id);
                    break;
            case 8:
                    otworz_cale_centrum(sem_id);
                    break;
            case 9:
                    printf("Kończę pracę ratownika!\n");
                    exit(0);
            default:
                    printf("Niepoprawny wybór, spróbuj ponownie!\n");
        }
    }
}
