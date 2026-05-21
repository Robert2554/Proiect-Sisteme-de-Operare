#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void start_monitor() {
    pid_t hub_mon_pid = fork();
    
    if (hub_mon_pid == 0) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Eroare creare pipe");
            exit(1);
        }

        pid_t monitor_pid = fork();
        if (monitor_pid == 0) {
            close(pipefd[0]); 
            
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            execlp("./monitor_reports", "monitor_reports", NULL);
            perror("Eroare exec monitor");
            exit(1);
        }

        close(pipefd[1]); 
        char buffer[256];
        int bytes_read;

        // Citim ce trimite monitorul prin pipe
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("\n[HUB_MON] %s", buffer);
        }

        close(pipefd[0]);
        waitpid(monitor_pid, NULL, 0); 
        
        printf("\n[HUB_MON] Procesul monitor s-a terminat.\ncity_hub> ");
        exit(0);
    }

    printf("Monitorul (hub_mon) a inceput cu succes.\n");
}

void calculate_scores(char *args) {
    char *districts[20];
    int count = 0;

    char *token = strtok(args, " \n");
    while (token != NULL && count < 20) {
        districts[count++] = token;
        token = strtok(NULL, " \n");
    }

    if (count == 0) {
        printf("Eroare: Trebuie specificat cel putin un district.\n");
        return;
    }

    int pipes[20][2];
    pid_t pids[20];

    // Cream cate un proces pentru fiecare district
    for (int i = 0; i < count; i++) { 
        pipe(pipes[i]);
        pids[i] = fork();

        if (pids[i] == 0) {
            close(pipes[i][0]);
            
            // Redirectam output-ul scorer-ului catre pipe folosind dup2
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][1]);

            execlp("./scorer", "scorer", districts[i], NULL);
            perror("Eroare exec scorer");
            exit(1);
        }
        close(pipes[i][1]); 
    }

    printf("\n==== RAPORT DISTRICTE ====\n");
    
    // Colectam output-ul unificat
    for (int i = 0; i < count; i++) { 
        char buffer[1024];
        int bytes_read;
        
        while ((bytes_read = read(pipes[i][0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer); 
        }
        close(pipes[i][0]);
        waitpid(pids[i], NULL, 0); 
    }
    
    printf("==================================\n");
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    char command[256];
    
    printf("Program City Hub. Comenzi: start_monitor, calculate_scores <d1> <d2>, exit\n");

    while (1) {
        printf("\ncity_hub> ");
        
        if (fgets(command, sizeof(command), stdin) == NULL) break;
        
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } 
        else if (strncmp(command, "calculate_scores", 16) == 0) {
            calculate_scores(command + 17);
        } 
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else if (strlen(command) > 0) {
            printf("Comanda necunoscuta.\n");
        }
    }

    return 0;
}