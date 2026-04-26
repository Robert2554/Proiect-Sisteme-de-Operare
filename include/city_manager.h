#ifndef CITY_MANAGER_H
#define CITY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

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

void mode_to_string(mode_t mode, char *str);
int open_and_check_permissions(const char *pathname,int flags,mode_t mode,const char *role,int manager_bit,int inspector_bit);
void create_dir(const char *name);
void create_district_symlink(const char *district_id);
void check_dangling_link(const char *district_id);
void log_district(const char *district_id,const char *username,const char *role,const char *command);
void add(const char *district_id, const char *username, const char *role);
void list(const char *district_id, const char *username, const char *role);
void view(const char *district_id,const char *username,const char *role,int report_id);
void remove_report(const char *district_id,const char *username,const char *role,int report_id);
void update_threshold(const char *district_id,const char *username,const char *role,int value);
void filter_district(const char *district_id,const char *role,int num_conditions,char **conditions);


#endif