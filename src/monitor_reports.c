#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>


// Functia care sterge fisierul la primirea SIGINT
void cleanup_and_exit() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Eroare la fork pentru stergerea fisierului PID");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp("rm", "rm", ".monitor_pid", NULL);
        perror("Eroare exec rm");
        exit(EXIT_FAILURE);
    } else {
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

int check_existing_monitor() {
    int pid_fd = open(".monitor_pid", O_RDONLY);
    if (pid_fd != -1) {
        char buf[32];
        int n = read(pid_fd, buf, sizeof(buf) - 1);
        close(pid_fd);
        if (n > 0) {
            buf[n] = '\0';
            pid_t existing_pid = atoi(buf);
            
            if (kill(existing_pid, 0) == 0) {
                printf("[ERROR] Monitor is already running with ID: %d\n", existing_pid);
                return 1;
            }
        }
    }
    return 0;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    if (check_existing_monitor() == 1) {
        return 1; 
    }

    setup_signals();
    write_pid_file();

    sigset_t empty_mask;
    sigemptyset(&empty_mask);


    // Bucla infinita care suspenda procesul
    while (1) {
        sigsuspend(&empty_mask); 
    }

    return 0;
}