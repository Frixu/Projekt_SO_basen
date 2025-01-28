#ifndef KLIENT_H
#define KLIENT_H

#include "konfiguracja.h"

// Struktura do przechowywania danych o średniej wieku w basenie rekreacyjnym
/*typedef struct {
    int suma_wiekow; // Suma wieku wszystkich osób w basenie rekreacyjnym
    int liczba_osob; // Liczba osób w basenie rekreacyjnym
} DaneRekreacyjny;*/

// Funkcje
double oblicz_srednia(DaneRekreacyjny *dane);
void ewakuacja(int sig);
void inicjalizuj_sygnaly();
void wejdz_na_basen(int wiek, int sem_id, int sem_num, int is_vip, int numer, DaneRekreacyjny *rekreacyjny, int is_opiekun);

#endif
