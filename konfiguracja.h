#ifndef KONFIGURACJA_H
#define KONFIGURACJA_H


#define MAX_OLIMPIJSKI 20
#define MAX_REKREACYJNY 15
#define MAX_BRODZIK 10
#define MAX_KLIENCI 45

#define MIN_WIEK_OLIMPIJSKI 18
#define MAX_WIEK_BRODZIK 5
#define MAX_WIEK_PAMPERS 3

#define LICZBA_BASENOW 3
#define LICZBA_SEMAFOROW 4
#define CZAS_BILETU 10 // w sekundach
#define SEM_OLIMPIJSKI 0
#define SEM_REKREACYJNY 1
#define SEM_BRODZIK 2
#define MAX_PROBY 5

#define GREEN   "\033[32m" // Zielony
#define RED     "\033[31m" //Czerwony
#define YELLOW "\033[33m" //Zolty
#define RESET   "\033[0m" // Resetuje kolor


// Struktura do przechowywania danych o średniej wieku w basenie rekreacyjnym
typedef struct {
    int suma_wiekow; // Suma wieku wszystkich osób w basenie rekreacyjnym
    int liczba_osob; // Liczba osób w basenie rekreacyjnym
} DaneRekreacyjny;

#endif
