#ifndef KONFIGURACJA_H
#define KONFIGURACJA_H


#define MAX_OLIMPIJSKI 40
#define MAX_REKREACYJNY 30
#define MAX_BRODZIK 15

#define MIN_WIEK_OLIMPIJSKI 18
#define MAX_WIEK_BRODZIK 5
#define MAX_WIEK_PAMPERS 3

#define CZAS_BILETU 5 // w sekundach

#define SEM_KEY 1234
#define SEM_OLIMPIJSKI 0
#define SEM_REKREACYJNY 1
#define SEM_BRODZIK 2

//sciezki do logow
#define KASJER_LOG "log/kasjer_log.txt"

//makra dla sprawdzenia zasad
#define CZY_PAMPERS(wiek) ((wiek) <= MAX_WIEK_PAMPERS)
#define CZY_BRODZIK(wiek) ((wiek) <= MAX_WIEK_BRODZIK)
#define CZY_OLIMPIJSKI(wiek) ((wiek) >= MIN_WIEK_OLIMPIJSKI)

#endif // KONFIGURACJA_H
