Documentatie Utilizare AI - Faza 1


1. Instrumentul Utilizat
Model AI: Gemini (Google).

2. Prompt-urile Oferite
Am utilizat urmatoarele instructiuni pentru a genera logica de filtrare:

Prompt 1: "Am o structura Report (id, inspector, gps, categorie, severitate, timestamp, descriere). Genereaza o functie int parse_condition(const char *input, char *field, char *op, char *value) care sa imparta un sir de tipul field:operator:value in cele trei componente ale sale." 

Prompt 2: "Genereaza functia int match_condition(Report *r, const char *field, const char *op, const char *value) care returneaza 1 daca raportul satisface conditia si 0 altfel. Asigura-te ca valorile numerice (severitate si timestamp) sunt convertite corect pentru comparatie."

3. Ce a fost generat
AI-ul a furnizat codul sursa pentru cele doua functii cerute si inca 2 functii ajutatoare int compare_ints(long a, long b, const char *op) , si int compare_strings(const char *a, const char *b, const char *op) care sa ne ajute sa convertim formatul din linia de comanda intr-un format pe care sa-l putem folosi in functiile noastre.
parse_condition: Utilizeaza sscanf cu formatul %[^:] pentru a extrage campurile fara a modifica sirul de intrare original. 
match_condition: Utilizeaza o structura de decizie (if-else) pentru a compara campurile structurii cu valorile din conditie, folosind atoi/atol pentru conversii numerice si strcmp pentru siruri de caractere.

4. Ce am modificat si de ce:
Am observat ca terminalul interpreteaza operatorii > si < ca redirectionari. Asa ca am folosit ghilimelelor la rulare pentru conditii pentru a proteja integritatea argumentelor.

5. Pe langa functiile de filtrare, am folosit asistentul AI pentru:
Managementul Link-urilor Simbolice: Intelegerea conceptului de "dangling links" si diferenta dintre stat() si lstat().
Organizarea proiectului cu foldere separate pentru src si include si configurarea comenzii gcc.
Ajutor in functia filter_district pentru cazul in care avem mai multe conditii , nu doar una.