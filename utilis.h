#ifndef UTILS_H
#define UTILS_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "konfiguracja.h"

// Funkcja do uzyskiwania klucza pamięci współdzielonej
key_t get_shm_key(const char *pathname, int project_id);

// Funkcja do tworzenia lub uzyskiwania semafora
int create_semaphore(int sem_key, int num_semaphores);

// Funkcja do tworzenia pamięci współdzielonej
int create_shared_memory(key_t shm_key, size_t size);

// Funkcja do usuwania pamieci wspoldzielonej
void usun_pamiec_wspoldzielona(int *klient_numer, DaneRekreacyjny *rekreacyjny, int shm_id, int rek_id);


#endif
