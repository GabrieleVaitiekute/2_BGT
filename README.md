# 2 BGT užduotis: Supaprastintos blokų grandinės (blockchain) kūrimas
## Naudojimosi intrukcija
1. Parsisiųskite Zip failą ir jį išarchivuokite
2. Atidarykite aplankalą norimoje programavimo aplinkoje, pvz.: VS Code
3. Atvėrę terminalą įveskite: * *g++ main.cpp HashFunc.cpp -o main.exe* * 
   Ši komanda sukurs vykdomąjį failą main.exe
4. Programą galite paleisti įvesdami šią komandą terminale: * *./main.exe* * arba tiesiog du kartus paspauskite main.exe failą iš aplanko.
5. Programa pradės generuoti vartotojus ir vėliau transakcijas. Duomenys bus automatiškai išsaugoti į vartotojai.txt ir transakcijos.txt failus.
6. Programa jūsų paklaus, ar norite sukurti bloką:
   * Jei įvesite t, programa sugeneruos bloką naudodama atsitiktines transakcijas ir išsaugos ją į blokai.txt failą. Po sugeneravimo jūsų bus klausiama, ar norite sukurti dar vieną bloką.
   * Jei atsakysite no, programa baigs blokų kūrimą.
7. Baigus generavimą bus duota galimybė atspausdinti pasirinktą informaciją arba baigti darbą:
   * Jei įvesite 1 - galėsite atspausdinti transakiciją, bet bus prašoma įvesti jos ID, tai galite rasti transakcijos.txt faile.
   * Jei įvesite 2 - galėsite atspausdinti  bloką, bet bus prašoma įvesti jos ID, tai galite rasti blokai.tx faile.
   * Jei įvesite bet kokį kitą simbolį programa baigs dirbti.
