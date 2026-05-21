#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h> 


#define MAX_NAME 30
#define MAX_CAT 20
#define MAX_DESCRIPTION 100
#define MAX_PATH 50


typedef struct Report {
    int id;
    char inspector_name[MAX_NAME];
    float latitude;
    float longitude;
    char category[MAX_CAT];
    int severity;
    time_t timestamp;
    char description[MAX_DESCRIPTION];
} Report;


typedef struct {
    char name[MAX_NAME];
    int score;
} InspectorScore;

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc != 2) {
        return 1;
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/reports.dat", argv[1]);

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        printf("District %s: Fisier invalid sau fara rapoarte.\n", argv[1]);
        return 0; 
    }

    InspectorScore scores[100];
    int count = 0;
    Report r;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        int found = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(scores[i].name, r.inspector_name) == 0) {
                scores[i].score += r.severity; 
                found = 1;
                break;
            }
        }
        if (!found && count < 100) {
            strcpy(scores[count].name, r.inspector_name);
            scores[count].score = r.severity;
            count++;
        }
    }
    close(fd);

    printf("--- Suma severitatilor din district: %s ---\n", argv[1]);
    for (int i = 0; i < count; i++) {
        printf("  Inspector %s : Total Severitate = %d\n", scores[i].name, scores[i].score);
    }

    return 0;
}