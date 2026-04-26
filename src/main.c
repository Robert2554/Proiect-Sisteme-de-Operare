#include "../include/city_manager.h"
#include "../include/filter_functions.h"

int main(int argc , char **argv){
    char *role = argv[2];
    char *user = argv[4];
    char *command = argv[5];
    char *district = argv[6];
    if(strcmp(argv[1],"--role") == 0 && strcmp(argv[3],"--user") == 0){
        check_dangling_link(district);
        if(strcmp(command,"--add") == 0 && (argc == 7)) add(district,user,role),create_district_symlink(district);
        else if(strcmp(command,"--list") == 0 && (argc == 7)) list(district,user,role);
        else if(strcmp(command,"--view") == 0 && (argc == 8)) view(district,user,role,atoi(argv[7]));
        else if(strcmp(command,"--remove_report") == 0 && (argc == 8)) remove_report(district,user,role,atoi(argv[7]));
        else if(strcmp(command,"--update_threshold") == 0 && (argc == 8)) update_threshold(district,user,role,atoi(argv[7]));
        else if (strcmp(command, "--filter") == 0 && (argc >= 8)) {
            // Conditiile incep de la indexul 7 si merg pana la final.
            int num_conditions = argc - 7;
            char **conditions = &argv[7]; // Trimitem un pointer catre restul argumentelor din array
            filter_district(district, role, num_conditions, conditions);
            log_district(district,user,role,"filter");
        }
    }


    return 0;
}