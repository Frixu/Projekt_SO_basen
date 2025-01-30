#ifndef KLIENT_H
#define KLIENT_H

#include "konfiguracja.h"
#include "ratownik.h"

// Funkcja do obliczania sredniej na basenie rekreacyjnym
double oblicz_srednia(DaneRekreacyjny *dane);

// Funkcja symulujaca wchodzenie na basen
void wejdz_na_basen(int wiek, int sem_id, int sem_num, int is_vip, int numer, DaneRekreacyjny *rekreacyjny, int is_opiekun);

// Funkcja wywoływana, gdy klient otrzyma sygnał do opuszczenia basenu
void opusc_basen(int sig);

// Funkcja wywoływana, gdy klient otrzyma sygnał do powrotu na basen
void wroc_na_basen(int sig);

//Funkcja wywoływana gdy będzie wysłany sygnał zamkniecia calego centrum
void zamkniecie_obiektu(int sig);

#endif

