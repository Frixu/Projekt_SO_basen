CC = gcc
CFLAGS = -Wall -g -lpthread

# Programy
EXEC_KASJER = kasjer
EXEC_KLIENT = klient


kasjer: kasjer.c
        $(CC) $(CFLAGS) -o kasjer kasjer.c

klient: klient.c
        $(CC) $(CFLAGS) -o klient klient.c

ratownik: ratownik.c
        $(CC) $(CFLAGS) -o ratownik ratownik.c

run: kasjer klient ratownik
        ./$(EXEC_KASJER) &
        sleep 1
        ./$(EXEC_KLIENT)

clean:
        rm -f kasjer klient

