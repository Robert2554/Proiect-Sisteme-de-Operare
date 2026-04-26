#include "../include/city_manager.h"
#include "../include/filter_functions.h"

///rxw        rxw       rwx
///MANAGAER INSPECTORS OTHERS
void mode_to_string(mode_t mode , char *str){
    // Conversie a permisiunilor intr-un string
    strcpy(str,"---------");
    if(mode & S_IRUSR) str[0] = 'r'; 
    if(mode & S_IWUSR) str[1] = 'w';  
    if(mode & S_IXUSR) str[2] = 'x';  
    if(mode & S_IRGRP) str[3] = 'r';  
    if(mode & S_IWGRP) str[4] = 'w';  
    if(mode & S_IXGRP) str[5] = 'x';  
    if(mode & S_IROTH) str[6] = 'r';  
    if(mode & S_IWOTH) str[7] = 'w';  
    if(mode & S_IXOTH) str[8] = 'x';
}

int open_and_check_permissions(const char *pathname, int flags, mode_t mode, const char *role, int manager_bit, int inspector_bit) {
    int file_directory;
    
    if (flags & O_CREAT) {
        file_directory = open(pathname, flags | O_EXCL, mode);
        if (file_directory != -1) { // Daca fisierul a fost creat pentru prima data , setam permisiunile cu chmod
            if (chmod(pathname, mode) == -1) {
                perror("Chmod error");
            }
        } else if (errno == EEXIST) {
            file_directory = open(pathname, flags & ~O_CREAT);
        }
    } else {
        file_directory = open(pathname, flags);
    }

    if (file_directory == -1) {
        perror("Error opening file"); 
        return -1;
    }

    struct stat st;
    if (fstat(file_directory, &st) == -1) {
        perror("Stat error");
        close(file_directory);
        return -1;
    }

    if (strcmp(role, "manager") == 0) { // Verificarea permisiunilor pentru fiecare rol
        if (!(st.st_mode & manager_bit) && (manager_bit != 0)) {
            fprintf(stderr, "Permission denied for manager role\n");
            close(file_directory);
            return -1;
        }
    } else if (strcmp(role, "inspector") == 0) {
        if (!(st.st_mode & inspector_bit) && (inspector_bit != 0)) {
            fprintf(stderr, "Permission denied for inspector role\n");
            close(file_directory);
            return -1;
        }
    }

    return file_directory;
}


void create_dir(const char *name) {
    if (mkdir(name, 0750) == -1) { // Creare director
        if (errno != EEXIST) {
            perror("Error creating directory");
            exit(EXIT_FAILURE);
        }
    }
    if(chmod(name, 0750) == -1){
        perror("Chmod error");
        exit(1);
    }
}

void create_district_symlink(const char *district_id) {
    char target[MAX_PATH];
    char linkpath[MAX_PATH];

    sprintf(target, "%s/reports.dat", district_id);
    sprintf(linkpath, "active_reports-%s", district_id);

    if (symlink(target, linkpath) == -1) {
        if (errno == EEXIST) {
            unlink(linkpath); 
            if (symlink(target, linkpath) == -1) {
                perror("Eroare la recrearea symlink-ului");
            }
        } else {
            perror("Eroare la crearea symlink-ului");
        }
    }
}



void check_dangling_link(const char *district_id) {
    char linkpath[MAX_PATH];
    sprintf(linkpath, "active_reports-%s", district_id);

    struct stat link_info;
    struct stat target_info;

    if (lstat(linkpath, &link_info) == 0) {
        
        if (S_ISLNK(link_info.st_mode)) {
            
            if (stat(linkpath, &target_info) == -1) {
                printf("WARNING: Dangling link detected for '%s'!\n", linkpath);
            }
        }
    }
}

void log_district(const char *district_id, const char *username, const char *role, const char *command) {
    char pathname[MAX_PATH];
    sprintf(pathname, "%s/logged_district", district_id);

    int fd = open_and_check_permissions(pathname, O_WRONLY | O_CREAT | O_APPEND, 0644, role, S_IWUSR, 0); 
    
    if (fd == -1) {
        return;
    }

    time_t now = time(NULL);
    char buffer[512];

    int len = sprintf(buffer, "%ld %s %s %s\n", (long)now, username, role, command);

    if (write(fd, buffer, len) == -1) { // Scriem ultima comanda executata impreuna cu informatiile despre ea
        perror("Error writing to log");
        close(fd);
        return;
    }

    close(fd);
}

void add(const char *district_id, const char *username, const char *role) {
    create_dir(district_id); // Creare director pentru fiecare district

    char cfg_path[MAX_PATH];
    sprintf(cfg_path, "%s/district.cfg", district_id);
    if (access(cfg_path, F_OK) == -1) {
        // Cream fisierul cu permisiunile 640 
        int fd_cfg = open_and_check_permissions(cfg_path, O_WRONLY | O_CREAT | O_TRUNC, 0640, role, S_IWUSR, 0);
        if (fd_cfg == -1) return;
    }

    char reportname[MAX_PATH];
    sprintf(reportname, "%s/reports.dat", district_id); // Creare path pentru fisierul nostru

    int file_directory = open_and_check_permissions(reportname, O_WRONLY | O_CREAT | O_APPEND, 0664, role, S_IWUSR, S_IWGRP);

    if(file_directory == -1){
        return;
    }

    struct stat st;
    if(fstat(file_directory, &st) == -1){
        perror("Stat error");
        close(file_directory);
        return;
    }

    Report r; 
    r.id = st.st_size / sizeof(Report); // Citirea informatiilor din raport
    strcpy(r.inspector_name, username);
    r.timestamp = time(NULL);

    printf("X: "),scanf("%f", &r.latitude);
    
    printf("Y: "),scanf("%f", &r.longitude);
    
    printf("Category (road/lighting/flooding/other): "),scanf("%19s", r.category);
    
    printf("Severity level (1/2/3): "),scanf("%d", &r.severity);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Description: "),fgets(r.description, MAX_DESCRIPTION, stdin);
    r.description[strcspn(r.description, "\n")] = 0;

    int write_report = write(file_directory, &r, sizeof(Report)); // adaugam raportul
    if (write_report == -1) {
        perror("Write error");
        close(file_directory);
        return;
    }
    else{
        printf("Report added succesfully\n");
        log_district(district_id,username,role,"add");
    }

    close(file_directory);
}

void list(const char *district_id, const char *username, const char *role){
    char pathname[MAX_PATH];
    sprintf(pathname, "%s/reports.dat", district_id);

    int fd = open_and_check_permissions(pathname, O_RDONLY, 0, role, S_IRUSR, S_IRGRP);
    if(fd == -1) return;

    struct stat st;
    if(fstat(fd, &st) == -1){
        perror("Stat error");
        close(fd);
        return;
    }

    char perms[10];
    mode_to_string(st.st_mode, perms); 
    printf("Listing file %s by user %s with role %s\nActual permissions: %s\nFile size %ld\nDate and hour of last modification%s\n",
        pathname, username, role, perms, st.st_size, ctime(&st.st_mtime)); // Listare informatii generale
    
    Report r;
    while(read(fd, &r, sizeof(Report)) == sizeof(Report)){ // Listare informatii despre fiecare raport din fisier
        printf("Listing report with ID: %d\n", r.id);
        printf("Inspector: %s\n", r.inspector_name);
        printf("Latitude %.2f | Longitude %.2f: | Category: %s | Severity: %d\nAdded on: %s", r.latitude, r.longitude, r.category, r.severity, ctime(&r.timestamp));
        printf("Report description: %s\n\n", r.description);
    }
    log_district(district_id,username,role,"list");

    close(fd);
}

void view(const char *district_id, const char *username, const char *role,int report_id){
    char pathname[MAX_PATH];
    sprintf(pathname,"%s/reports.dat",district_id);

    int fd = open_and_check_permissions(pathname,O_RDONLY,0,role,S_IRUSR,S_IRGRP);
    if(fd == -1) return;


    Report r;
    int exist = 0;
    while(read(fd,&r,sizeof(Report)) == sizeof(Report)){ // Listarea raportului
        if(r.id == report_id){
            printf("Listing report with ID %d by user %s with role %s\n", r.id, username, role);
            printf("Inspector: %s\n", r.inspector_name);
            printf("Latitude %.2f | Longitude %.2f | Category: %s | Severity: %d\nAdded on: %s", r.latitude, r.longitude, r.category, r.severity, ctime(&r.timestamp));
            printf("Report description: %s\n\n", r.description);
            exist = 1;
        }
    }

    if(exist == 0) printf("Non-existent ID\n");
    else{log_district(district_id,username,role,"view");}

    close(fd);
}

void remove_report(const char *district_id, const char *username, const char *role, int report_id) {
    char pathname[MAX_PATH];
    sprintf(pathname, "%s/reports.dat", district_id);

    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Access Denied: Role %s does not have permission to remove reports!\n", role);
        return; 
    }

    int fd = open_and_check_permissions(pathname, O_RDWR, 0, role, S_IWUSR, 0);
    if (fd == -1) return;

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Stat error");
        close(fd);
        return;
    }
    off_t current_size = st.st_size;

    // Veficam sa nu citim in afara fisierului
    if (report_id * sizeof(Report) >= current_size) {
        printf("Error: The report with ID %d doesn't exist.\n", report_id);
        close(fd);
        return;
    }

    Report r;
    off_t read_offset = (report_id + 1) * sizeof(Report);
    off_t write_offset = report_id * sizeof(Report); 

    while (read_offset < current_size) { // Cat timp citim suntem in fisier
        lseek(fd, read_offset, SEEK_SET);   // Cursorul de citire setat la pozitia unde se termina raportul cautat
        read(fd, &r, sizeof(Report)); // Citim urmatorul raport din fisier

        r.id = r.id - 1; // Actualizam id-ul

        lseek(fd, write_offset, SEEK_SET); // Cursorul de scriere setat la pozitia unde incepe raportul cautat
        write(fd, &r, sizeof(Report)); // Mutam raportul cu o pozitie in urma

        read_offset += sizeof(Report);
        write_offset += sizeof(Report);
    }

    if (ftruncate(fd, current_size - sizeof(Report)) == -1) { // Stergem ultimul raport din fisier , pentru ca nu mai avem nevoie de el
        perror("Truncate error");
    } else {
        printf("The report with ID %d was deleted by %s.\n", report_id, username);
        log_district(district_id,username,role,"remove");
    }

    close(fd);
}

void update_threshold(const char *district_id, const char *username,const char *role,int value){
    char pathname[MAX_PATH];
    sprintf(pathname,"%s/district.cfg",district_id);

    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Access Denied: Role %s does not have permission to modify threshold!\n", role);
        return; 
    }


    int fd = open_and_check_permissions(pathname,O_RDWR,0640,role,S_IWUSR,S_IRGRP);
    if(fd == -1) return;

    struct stat st;
    if(fstat(fd,&st) == -1){
        perror("Stat error");
        return;
    }

    if((st.st_mode & 0777) != 0640){
        fprintf(stderr,"Error! Permission bits have been changed\n");
        close(fd);
        return;

    }

    if (ftruncate(fd, 0) == -1) {
        perror("Truncate error");
        return;
    }

    char buffer[20];
    int len = sprintf(buffer,"severity = %d",value);

    if(write(fd,buffer,len) == -1){
        perror("Write error");
        close(fd);
        return;
    }
    else{
        printf("Threshold updated by manager %s",username);
        log_district(district_id,username,role,"update");
    }

    close(fd);
}

void filter_district(const char *district_id, const char *role, int num_conditions, char **conditions) {
    char fields[10][50], ops[10][10], values[10][100]; 
    
    for (int i = 0; i < num_conditions; i++) {
        // Daca o singura conditie din linia de comanda e scrisa gresit, ne oprim din start.
        if (!parse_condition(conditions[i], fields[i], ops[i], values[i])) {
            fprintf(stderr, "Error: invalid format\n");
            return; 
        }
    }

    // 2. Acum ca stim ca filtrele sunt valide, deschidem fisierul
    char pathname[MAX_PATH];
    sprintf(pathname, "%s/reports.dat", district_id);

    int fd = open_and_check_permissions(pathname, O_RDONLY, 0664, role, S_IRUSR, S_IRGRP);
    if (fd == -1) {
        return;
    }

    Report current_report;
    int found_any = 0;

    printf("--- Filtering results ---\n");

    // 3. Citim rapoartele si le comparam cu filtrele deja parsate
    while (read(fd, &current_report, sizeof(Report)) == sizeof(Report)) {
        int matches_all = 1;

        for (int i = 0; i < num_conditions; i++) {
            // Trimitem direct variabilele salvate, fara sa mai parsam string-ul!
            if (!match_condition(&current_report, fields[i], ops[i], values[i])) {
                matches_all = 0;
                break; 
            }
        }

        if (matches_all) {
            printf("ID: %d | Inspector: %s | Category: %s | Severity: %d\n", 
                   current_report.id, current_report.inspector_name, 
                   current_report.category, current_report.severity);
            found_any = 1;
        }
    }

    if (!found_any) {
        printf("No reports matched all conditions.\n");
    }

    close(fd);
}


