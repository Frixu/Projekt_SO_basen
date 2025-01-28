#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "utils.h"

// Funkcja do uzyskiwania klucza pamięci współdzielonej
key_t get_shm_key(const char *pathname, int project_id) {
    key_t shm_key = ftok(pathname, project_id);
    if (shm_key == -1) {
        perror("Blad przy generowaniu klucza pamieci wspoldzielonej");
        exit(1);
    }
    return shm_key;
}

// Funkcja do tworzenia lub uzyskiwania semafora
int create_semaphore(int sem_key, int num_semaphores) {
    int sem_id = semget(sem_key, num_semaphores, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        perror("Blad przy tworzeniu semaforów");
        exit(1);
    }
    return sem_id;
}

// Funkcja do tworzenia pamięci współdzielonej
int create_shared_memory(key_t shm_key, size_t size) {
    int shm_id = shmget(shm_key, size, 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("Blad przy tworzeniu pamieci wspoldzielonej");
        exit(1);
    }
    return shm_id;
}

void usun_pamiec_wspoldzielona(int *klient_numer, DaneRekreacyjny *rekreacyjny, int shm_id, int rek_id){
    // Odłączanie pamięci współdzielonej
    if (shmdt(klient_numer) == -1) {
        perror("Blad przy odlczaniu pamieci wspoldzielonej klient_numer");
        exit(1);
    }

    if (shmdt(rekreacyjny) == -1) {
        perror("Blad przy odlaczaniu pamieci wspoldzielonej rekreacyjny");
        exit(1);
    }

    // Usuwanie pamięci współdzielonej
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("Blad przy usuwaniu pamieci wspoldzielonej klient_numer");
        exit(1);
    }

    if (shmctl(rek_id, IPC_RMID, NULL) == -1) {
        perror("Blad przy usuwaniu pamieci wspoldzielonej rekreacyjny");
        exit(1);
    }

    printf("Pamiec wspoldzielona zostala usunieta.\n");
}

