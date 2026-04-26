#include "../include/filter_functions.h"

int parse_condition(const char *input, char *field, char *op, char *value) {
    // Sscanf cu regex-like format:
    // %[^:] citeste toate caracterele pana da de ':'
    // Apoi sare peste ':', citeste iar pana la ':', sare peste el, 
    // si %s citeste restul (valoarea).
    int scanned = sscanf(input, "%[^:]:%[^:]:%s", field, op, value);
    
    if (scanned == 3) {
        return 1; // Succes
    }
    return 0; // Format invalid
}

// Functie ajutatoare pentru compararea numerelor intregi (folosita pt severity si timestamp)
int compare_ints(long a, long b, const char *op) {
    if (strcmp(op, "==") == 0) return a == b;
    if (strcmp(op, "!=") == 0) return a != b;
    if (strcmp(op, "<")  == 0) return a < b;
    if (strcmp(op, "<=") == 0) return a <= b;
    if (strcmp(op, ">")  == 0) return a > b;
    if (strcmp(op, ">=") == 0) return a >= b;
    return 0;
}

// Functie ajutatoare pentru compararea sirurilor de caractere (folosita pt category si inspector)
int compare_strings(const char *a, const char *b, const char *op) {
    int cmp = strcmp(a, b);
    if (strcmp(op, "==") == 0) return cmp == 0;
    if (strcmp(op, "!=") == 0) return cmp != 0;
    return 0; // Pentru string-uri logica principala este egal / diferit
}


int match_condition(Report *r, const char *field, const char *op, const char *value) {
    if (strcmp(field, "severity") == 0) {
        int val_cautata = atoi(value); // convertim string-ul in int
        return compare_ints(r->severity, val_cautata, op);
    } 
    else if (strcmp(field, "timestamp") == 0) {
        long val_cautata = atol(value); // convertim string-ul in long
        return compare_ints((long)r->timestamp, val_cautata, op);
    } 
    else if (strcmp(field, "category") == 0) {
        return compare_strings(r->category, value, op);
    } 
    else if (strcmp(field, "inspector") == 0) {
        return compare_strings(r->inspector_name, value, op);
    }
    
    return 0; 
}