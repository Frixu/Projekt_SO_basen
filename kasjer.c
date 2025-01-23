#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "konfiguracja.h"

int main() {
    int log_fd = open(KASJER_LOG, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (log_fd == -1) {
        perror("Nie udało się otworzyć pliku logu kasjera");
        return 1;
    }
    sleep(100);
    
    while (1) {
        write(log_fd, "Kasjer: Sprzedano bilet\n", 24);
        sleep(2);
    }

   
    return 0;
}
