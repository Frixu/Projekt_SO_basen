#ifndef RATOWNIK_H
#define RATOWNIK_H

#include <signal.h>
#include <sys/sem.h>

#define LICZBA_BASENOW 3
#define SYGNAL_OPUSC_BASEN SIGUSR1
#define SYGNAL_WROC_NA_BASEN SIGUSR2
#define SYGNAL_ZAMKNIJ_OBIEKT SIGTERM

void rozpocznij_prace_ratownikow(int sem_id);
void zamknij_basen(int numer_basen, int sem_id);
void otworz_basen(int numer_basen, int sem_id);
void zamknij_cale_centrum(int sem_id);
void otworz_cale_centrum(int sem_id);
void menu_ratownika(int sem_id);

#endif // RATOWNIK_H
