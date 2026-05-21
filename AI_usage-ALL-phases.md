


/////////////////////////////////////
Documentatie Utilizare AI - Faza 1
/////////////////////////////////////

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

/////////////////////////////////////
Documentatie Utilizare AI - Faza 2
/////////////////////////////////////

1. Instrumentul Utilizat
Model AI: Gemini (Google).

2. Prompt-urile Oferite

Pentru Faza 2 a proiectului, m-am concentrat pe gestiunea proceselor si comunicarea prin semnale. Am folosit AI-ul punctual pentru a intelege apelurile de sistem noi si pentru a depana modul de testare:

Prompt 1: "De ce trebuie sa folosim un proces copil pentru a sterge un director ? De ce nu putem rula comanda execlp direct dintr-un singur proces?"

Prompt 2: "Daca folosesc kill(pid, SIGUSR1) ca sa trimit un semnal, de ce functia imi returneaza -1? Cum verific daca procesul cu acel PID chiar ruleaza?"

Prompt 3: "Cum testez concret comunicarea intre cele doua programe din terminal, daca vreau sa folosesc un singur terminal, nu doua separate?"

3. Ce a fost generat

AI-ul mi-a explicat cum functioneaza familia de functii exec* in Linux: acestea inlocuiesc complet imaginea procesului curent din memorie cu noul program apelat.Mi-a clarificat faptul ca, daca as fi rulat execlp("rm", ...) direct din programul principal city_manager, programul meu s-ar fi transformat efectiv in comanda rm, si-ar fi terminat executia dupa stergere si s-ar fi inchis definitiv, fara a mai rula restul codului meu.

Mi-a explicat ca kill() returneaza -1 daca procesul tinta nu exista sau nu am permisiuni, sugerandu-mi cum sa interpretez acest rezultat pentru a trata erorile.

Pentru partea de testare, AI-ul a generat instructiunile de shell necesare pentru a rula monitor_reports in background folosind operatorul &, si folosirea comenzii fg pentru a-l aduce in prim-plan cand vreau sa il opresc.

4. Ce am modificat / integrat in cod:

Am instantiat un proces copil cu fork() care sa ruleze execlp (sacrificandu-se pentru a deveni comanda rm), in timp ce in procesul parinte am scris logica de wait(&status) pentru a bloca executia pana cand directorul este sters, abia apoi afisand mesajul de succes.

Pe baza explicatiilor despre kill(), am modificat functia add din city_manager: am adaugat un bloc if care verifica daca kill a returnat -1 si, in caz afirmativ, formateaza un mesaj de eroare pe care il scrie cu write in logged_district, conform cerintei.

Pe partea de utilzare a terminalului Linux, am invatat rularea cu & si recuperarea cu fg, ceea ce mi-a permis sa testez usor comunicarea asincrona dintre city_manager si monitor direct din acelasi terminal.

////////////////////////////////////////
Documentatie Utilizare AI - Faza 3
////////////////////////////////////////

1. Model AI: Gemini(Google)

2. Prompt-uri oferite:

Prompt 1: "In functia mea check_existing_monitor, citesc PID-ul vechi din fisier, dar vreau sa verific daca procesul cu acel PID inca mai ruleaza in sistem, pentru a nu lansa monitorul de doua ori. Cum pot interoga sistemul Linux daca un proces traieste, dar fara sa ii trimit un semnal care sa-l omoare?"

Prompt 2: "Care este motivul pentru care cerinta ne obliga sa folosim pipe si dup2 pentru ca functia calculate_scores sa preia datele de la procesele scorer? Nu ar fi fost mai simplu sa lasam procesele scorer sa printeze direct rezultatul cu un simplu printf pe stdout?"

Prompt 3: "Practic functia start_monitor() din hub-ul meu ruleaza ea insasi programul monitor_reports? Cum face mai exact asta in C, se comporta city_hub practic ca un fel de terminal in terminal?"

3. Ce mi s-a generat:

AI-ul mi-a explicat un "trick" foarte util din standardul POSIX: folosirea functiei kill(existing_pid, 0). Mi s-a clarificat ca trimiterea semnalului 0 nu face absolut nimic procesului (nu il opreste), dar obliga sistemul de operare sa verifice daca PID-ul este valid si daca am permisiunea sa interactionez cu el.

AI-ul mi-a explicat ca daca procesele ar printa direct, iesirile lor s-ar putea amesteca ilizibil pe ecran. Mai important, procesul parinte (city_hub) ar fi complet izolat de datele generate si nu ar putea sa le colecteze pentru a crea un raport unificat.

AI-ul mi-a confirmat intuitia si mi-a detaliat pasii din spate. Mi-a explicat cum city_hub actioneaza exact ca un bash shell: da fork() pentru a crea un clona-copil, care apoi foloseste execlp() pentru a se "distruge" si a incarca in memorie executabilul monitor_reports de pe hard disk, totul legat printr-un pipe.

4. Ce am integrat in cod:

In urma acestor discutii, am construit lansarea proceselor cu fork si execlp (hub-ul actionand ca un mini-terminal), asigurandu-ma prin kill(pid, 0) ca nu rulez monitorul in duplicat. Pentru comunicare, am redirectionat iesirile cu pipe si dup2, permitand procesului parinte sa capteze si sa centralizeze elegant datele de la procesele scorer, evitand haosul afisarilor concurente direct pe ecran.




