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


Documentatie Utilizare AI - Faza 2

1. Instrumentul Utilizat
Model AI: Gemini (Google).

2. Prompt-urile Oferite

Pentru Faza 2 a proiectului, m-am concentrat pe gestiunea proceselor si comunicarea prin semnale. Am folosit AI-ul punctual pentru a intelege apelurile de sistem noi si pentru a depana modul de testare:

Prompt 1: "De ce trebuie sa folosim un proces copil pentru a sterge un director ? De ce nu putem rula comanda execlp direct dintr-un singur proces?"

Prompt 2: "Care este diferenta dintre sleep() si pause() intr-o bucla infinita a unui proces care actioneaza ca un monitor?"

Prompt 3: "Daca folosesc kill(pid, SIGUSR1) ca sa trimit un semnal, de ce functia imi returneaza -1? Cum verific daca procesul cu acel PID chiar ruleaza?"

Prompt 4: "Cum testez concret comunicarea intre cele doua programe din terminal, daca vreau sa folosesc un singur terminal, nu doua separate?"

3. Ce a fost generat

AI-ul mi-a explicat cum functioneaza familia de functii exec* in Linux: acestea inlocuiesc complet imaginea procesului curent din memorie cu noul program apelat.Mi-a clarificat faptul ca, daca as fi rulat execlp("rm", ...) direct din programul principal city_manager, programul meu s-ar fi transformat efectiv in comanda rm, si-ar fi terminat executia dupa stergere si s-ar fi inchis definitiv, fara a mai rula restul codului meu.

M-a informat despre diferenta de performanta dintre sleep() si pause(), aratand ca pause() suspenda procesul cu 0% consum CPU pana la sosirea unui semnal.

Mi-a explicat ca kill() returneaza -1 daca procesul tinta nu exista sau nu am permisiuni, sugerandu-mi cum sa interpretez acest rezultat pentru a trata erorile.

Pentru partea de testare, AI-ul a generat instructiunile de shell necesare pentru a rula monitor_reports in background folosind operatorul &, si folosirea comenzii fg pentru a-l aduce in prim-plan cand vreau sa il opresc.

4. Ce am modificat / integrat in cod:

Am instantiat un proces copil cu fork() care sa ruleze execlp (sacrificandu-se pentru a deveni comanda rm), in timp ce in procesul parinte am scris logica de wait(&status) pentru a bloca executia pana cand directorul este sters, abia apoi afisand mesajul de succes.

Pe baza explicatiilor despre kill(), am modificat functia add din city_manager: am adaugat un bloc if care verifica daca kill a returnat -1 si, in caz afirmativ, formateaza un mesaj de eroare pe care il scrie cu write in logged_district, conform cerintei.

Am implementat bucla while(1) { pause(); } in monitor pentru eficienta optima.

Pe partea de utilzare a terminalului Linux, am invatat rularea cu & si recuperarea cu fg, ceea ce mi-a permis sa testez usor comunicarea asincrona dintre city_manager si monitor direct din acelasi terminal.



