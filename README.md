# 2 BGT užduotis: Supaprastintos blokų grandinės (blockchain) kūrimas
## Naudojimosi instrukcija
1. Parsisiųskite Zip failą ir jį išarchivuokite
2. Atidarykite aplankalą norimoje programavimo aplinkoje, pvz.: VS Code
3. Atvėrę terminalą įveskite: `g++ main.cpp HashFunc.cpp -o main.exe`  <br> Ši komanda sukurs vykdomąjį failą main.exe
4. Programą galite paleisti įvesdami šią komandą terminale: `./main.exe` arba tiesiog du kartus paspauskite main.exe failą iš aplanko.
5. Programa pradės generuoti vartotojus ir vėliau transakcijas. Duomenys bus automatiškai išsaugoti į vartotojai.txt ir transakcijos.txt failus.
6. Programa jūsų paklaus, ar norite sukurti bloką:
   * Jei įvesite t, programa sugeneruos bloką naudodama atsitiktines transakcijas ir išsaugos ją į blokai.txt failą. Po sugeneravimo jūsų bus klausiama, ar norite sukurti dar vieną bloką.
   * Jei atsakysite no, programa baigs blokų kūrimą.
7. Baigus generavimą bus duota galimybė atspausdinti pasirinktą informaciją arba baigti darbą:
   * Jei įvesite 1 - galėsite atspausdinti transakiciją, bet bus prašoma įvesti jos ID, tai galite rasti transakcijos.txt faile.
   * Jei įvesite 2 - galėsite atspausdinti  bloką, bet bus prašoma įvesti jos ID, tai galite rasti blokai.txt faile.
   * Jei įvesite bet kokį kitą simbolį programa baigs dirbti.
## Užduoties formuluotė
### 1. Vartotojų generavimas
Tam buvo sukurta klasė User (ją sudaro name, public_key, balance). Klasę galite rasti blockchain.h.
Funkcija  generateUsers() sugeneruoja vartotojus ir išveda juos į failą vartotojai.txt.

### 2. Transakcijų generaviams
Buvo sukurta klasė Transaction (ją sudaro transactionID, sender_public_key, recipient_public_key, inputs, outputs) ir struktūra UTXO (utxoID, OwnerPublic_key, amount). Klasę ir struktūrą galite rasti blockchain.h. 
Funkcija  generateTransactions() sugeneruoja transakcijas (tik tas, kurioms įvykdyti užtenkta siuntėjo lėšų), patikrina jų reikšmės maišas ir jas išveda į failą transakcijos.txt. 

### 3. Transakcijų įvedimas į naują bloką
Sukurta klasė Block (sudaryta iš blockID, previous_hash, timestamp, version, nonce, difficulty,  transactions, calculateBlockID(), calculateMerkleRoot(), mineBlockParallel() ). Klasę galite rasti blockchain.h.
Funkcija  generateBlocks() klausia ar naudotojas nori bloką sukurti. Prieš generavimą patikrina, ar yra užtektinai transakcijų (100).

### 4-6. Blokų kasimas ir transakcijų tvarkymas
Bloko mineBlock funkcija realizuoja kasimo procesą naudojant Proof-of-Work (PoW) algoritmą. Šis procesas keičia nonce reikšmę tol, kol blockID atitinka reikalaujamą difficulty lygį. Realizuotas lygiagretus blokų kasimo procesas.  Kai tinkamas nonce surandamas ir blokas yra "iškastas", jis įrašomas į blokai.txt failą kartu su transakcijų informacija, ir bloko blockID pridedamas į blockchain sąrašą kaip patvirtintas blokas. Po kiekvieno bloko kasimo ir patvirtinimo, atnaujinamos vartotojų sąskaitos vartotojai.txt faile (įskaitant jų balansus) bei transakcijų sąrašas transakcijos.txt.

Faile naudojamos kelios gerosios OOP praktikos: 
  + Enkapsuliacija (Privatūs atributai ir vieši metodai klasėse)
  + Konstruktoriai (Klasėse User, Transaction, Block)
  + RAII (Konstruktoriai ir destruktoriai)
