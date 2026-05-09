#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>


// Functia care sterge fisierul la primirea SIGINT
void cleanup_and_exit() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Eroare la fork pentru stergerea fisierului PID");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Copilul ruleaza rm pe fisierul ascuns
        execlp("rm", "rm", ".monitor_pid", NULL);
        perror("Eroare exec rm");
        exit(EXIT_FAILURE);
    } else {
        // Parintele asteapta stergerea si se inchide
        wait(NULL);
        printf("\nAm primit SIGINT. Fisierul .monitor_pid a fost sters.\n");
        exit(0);
    }
}

// Handler-ul care prinde semnalele
void signal_handler(int sig) {
    if (sig == SIGINT) {
        cleanup_and_exit(); 
    } else if (sig == SIGUSR1) {
        printf("A fost adaugat un raport nou in sistem!\n"); 
    }
}

// Functia pentru setarea sigaction
void setup_signals() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Eroare la configurarea SIGINT");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Eroare la configurarea SIGUSR1");
        exit(EXIT_FAILURE);
    }
}

// Functia care scrie PID-ul in fisier
void write_pid_file() {
    pid_t my_pid = getpid();
    
    int fd = open(".monitor_pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Eroare la deschiderea/crearea .monitor_pid");
        exit(EXIT_FAILURE);
    }

    char pid_str[32];
    int len = snprintf(pid_str, sizeof(pid_str), "%d\n", my_pid);
    
    if (write(fd, pid_str, len) == -1) {
        perror("Eroare la scrierea PID-ului");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    close(fd);
    printf("Monitorul a pornit cu PID-ul: %d. Fisierul .monitor_pid a fost salvat.\n", my_pid);
    printf("Astept semnale...\n");
}

int main() {
    setup_signals();
    write_pid_file();

    // Bucla infinita care suspenda procesul pana cand primeste un semnal
    while (1) {
        pause(); 
    }

    return 0;
}