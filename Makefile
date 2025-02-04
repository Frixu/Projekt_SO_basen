CC = gcc
CFLAGS = -Wall -g

# Program wynikowy
EXEC = main

# Zasada do kompilacji main.c
$(EXEC): main.c kasjer.c klient.c utils.c
        $(CC) $(CFLAGS) -o $(EXEC) main.c kasjer.c klient.c utils.c -pthread

# Uruchamianie procesu
run: $(EXEC)
        ./$(EXEC)

# Czyszczenie plików wynikowych
clean:
        rm -f $(EXEC)
