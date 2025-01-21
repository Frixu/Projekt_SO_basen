#ifndef KONFIGURACJA_H
#define KONFIGURACJA_H

//ograniczenia pojemności basenu
#define MAX_OLIMPIJSKI 20
#define MAX_REKREACYJNY 15
#define MAX_BRODZIK 10
#define MAX_KLIENCI 45

//klucze 
#define SEM_KEY 1234
#define SHM_KEY 5678

//ograniczenia wiekowe na baseny
#define MIN_WIEK_OLIMPIJSKI 18
#define MAX_WIEK_BRODZIK 5
#define MAX_WIEK_PAMPERS 3

#define CZAS_BILETU 5 // czas biletu w sekundach

#define SEM_OLIMPIJSKI 0
#define SEM_REKREACYJNY 1
#define SEM_BRODZIK 2

//makra dla sprawdzenia zasad
#define CZY_PAMPERS(wiek) ((wiek) <= MAX_WIEK_PAMPERS)
#define CZY_BRODZIK(wiek) ((wiek) <= MAX_WIEK_BRODZIK)
#define CZY_OLIMPIJSKI(wiek) ((wiek) >= MIN_WIEK_OLIMPIJSKI)

#endif // KONFIGURACJA_H
