# SLB-2024 – Labo01 – ransTroj

## Auteurs

    - Amir Mouti
    - Ouweis Harun
    - Nathan Rayburn

## First Part

### Manipulation 3.1

On peut déchiffrer en exécutant de nouveau. C'est un simple xor, donc ça réinverse.

### Manipulation 3.2

SEEK_CUR = 1

---

## Second Part: Structure du code

### Manipulation 4.1

![alt text](image.png)
![alt text](image-1.png)

### Question 4.1 : Que fait le code au début de `main` avant l'invocation de la fonction `is_prime` ? Quel est le but du malware, d'après vous ?

Le malware lance l'application calculator de gnome afin de faire passer son véritable attaque malicieuse en arrière-plan sur le main thread. Le thread qui est lancé contient l'application calculator. La synchronisation des threads se fait à la fin de l'attaque du chiffrement du directory.

### Question 4.2 : Que fait la fonction `is_prime` invoquée au début du `main` ? Quel est le but du malware, d'après vous ?

La fonction `is_prime` permet de savoir si les deux valeurs passées en paramètres sont premiers entre elles.

### Question 4.3 : Quelles sont les valeurs passées aux appels à `is_prime` ?

Ce sont deux valeurs premiers entre eux.

### Question 4.4 : Que fait le code C selon les différentes valeurs de retour de la fonction `is_prime` ?

Si la somme de tous les retours de `is_prime` vaut `4`, en récursion, on descend dans l'arbre et au backtrace, on chiffre les fichiers avec `encrypt0`, `encrypt1` et `encrypt2`. Sinon, on join le thread de la calculatrice `gnome-calculator` et le programme se termine.

### Question 4.5 : Que fait la fonction `encrypt_dir` invoquée après l’appel à `is_prime` dans `main` ?

La fonction `encrypt_dir` s'occupe de la récursion pour descendre dans l'arbre de système de fichiers. Lors du backtrace de la récursion, elle compte l'occurrence de la lettre `s` dans le nom du directory et ensuite décide quel chiffrement elle utilise suivant le résultat.

s = 1 -> `encrypt0`  
s = 2 -> `encrypt1`  
s > 2 -> `encrypt2`

### Question 4.6 : Quelles sous-fonctions sont appelées par la fonction `encrypt_dir` et à quelle condition ?

![alt text](image-2.png)

- `strcmp`
- `fclose`
- `strlen`
- `fopen`
- `snprintf`
- `readdir`
- `closedir`
- `opendir`
- `encrypt0`
- `encrypt1`
- `encrypt2`
- `char_occurences`

### Question 4.7 : Quelles exceptions prévoit le code de `encrypt_dir` ? Pourquoi ?

- Si `opendir` rend `nullptr`: ouverture du dossier n'a pas marché.
- Si le nom du dossier est '..' ou '.': le programme ne veut pas ouvrir le dossier courant et il ne veut pas remonter l'arborescence.
- Si le fichier est lui-même: le programme ne veut pas se chiffrer lui-même.

### Manipulation 4.2

Dans cette manipulation, nous avons commenté le code C dans Ghidra pour répondre aux questions 4.1 à 4.7. Nous avons renommé et/ou "retypé" les variables locales selon nécessité.

### Question 4.8 : Analysez le code assembleur correspondant à la fonction `encrypt_dir` :

- **Parties de gestion de la pile** : _En attente d'identification._
- **Instructions de chiffrement** : _En attente d'explication._

---

## Third Part: Fonction `encrypt0`

### Manipulation 5.1

Dans Ghidra, afficher les codes C et assembleur de la fonction `encrypt0`.

### Question 5.1 : Que fait le code C affiché par Ghidra ?

- _En attente de réponse._

### Manipulation 5.2

Commentez le code C dans Ghidra, puis renommer et/ou "retyper" les variables locales en conséquence de la réponse à la question 5.1.

### Question 5.2 : Analysez le code assembleur correspondant à la partie chiffrement :

- Identifiez les parties de gestion de la pile comme telle (sans les expliquer).

fp  = param_1
res = local_10

`Prologue`
```assembly
080492cd 55              PUSH       EBP
080492ce 89 e5           MOV        EBP,ESP
080492d0 83 ec 18        SUB        ESP,0x18
```

`Epilogue`

```assembly
0804933d c9              LEAVE
0804933e c3              RET
```

`Gestion de stack`
```assembly  
080492d5 83 ec 04        SUB        ESP,0x4
080492d8 6a 01           PUSH       0x1
080492da 6a ff           PUSH       -0x1
080492dc ff 75 08        PUSH       dword ptr [EBP + fp]

...


08049313 83 ec 08        SUB        ESP,0x8
08049316 ff 75 08        PUSH       dword ptr [EBP + fp]
08049319 ff 75 f4        PUSH       dword ptr [EBP + res]

...

08049324 83 ec 0c        SUB        ESP,0xc
08049327 ff 75 08        PUSH       dword ptr [EBP + fp]


```
| Base  + Offset   | Comment  |
|-----------|----------|
| `EBP`  +   param_1     | file stream pointer    |
| `EBP`  +   local_10    | changed byte variable    |



- Expliquez les instructions qui implémentent la logique de la fonction `encrypt0`.

**Première partie de la logique est la condition qui décide quel constante on utilise.**

```assembly
    080492ea 83 e0 02        AND        EAX,0x2      @ Calculate second bit
    080492ed 85 c0           TEST       EAX,EAX      @ Test EAX value is 0 without modifying EAX with AND op
    080492ef 74 12           JZ         LAB_08049303 @ Jump if EAX is 0
```
C Equiv.
```c
if ((uVar1 & 2) == 0) {
```

**La seconde partie c'est xor avec les constantes. On fait un saut ou pas suivant si le `deuxième bit` est set à `1`.**

```assemly

@ Simple xor avec la constante et le byte

0804930c 81 75 f4        XOR        dword ptr [EBP + local_10],0xef 

...


@ Même chose mais avec l'autre constante si le 2ème bit vaut 1

080492fa 81 75 f4        XOR        dword ptr [EBP + local_10],0xbe 

```

C Equiv.

```C
local_10 = (int)(char)uVar1 ^ 0xef;

...

local_10 = (int)(char)uVar1 ^ 0xbe;

```

### Manipulation 5.3

Écrivez le code d’un programme capable de déchiffrer les fichiers chiffrés par la fonction `encrypt0`.

---

## Fourth Part: Fonction `encrypt1`

### Manipulation 6.1

Dans Ghidra, afficher les codes C et assembleur de la fonction `encrypt1`.

### Question 6.1 : Que fait le code C affiché par Ghidra ?

Le code de encrypt1 chiffre un fichier byte par byte en faisant quelques manipulations `bitwise operations` avec des `shift`, `or` et ainsi une multiplication avec une `constante`. 

Nous avions une constante qui évolue après chaque itération, donc chaque `byte` contient une addition d'une `constante` calculable.

```c
void encrypt1(FILE *fp)

{
  int byteFromFile;
  undefined var;
  byte res;
  
  var = 7;
  while( true ) {
    byteFromFile = fgetc(fp);
    if (byteFromFile == -1) break;
    fseek(fp,-1,1);
    res = ((byte)byteFromFile << 2 | (byte)byteFromFile >> 6) + var;
    var = res >> 2 | res * '@';
    fputc((uint)res,fp);
  }
  return;
}
```

### Manipulation 6.2

Commentez le code C dans Ghidra, puis renommer et/ou "retyper" les variables locales en conséquence de la réponse à la question 6.1.

### Question 6.2 : Analysez le code assembleur correspondant à la partie chiffrement :

- Identifiez les parties de gestion de la pile comme telle (sans les expliquer).

`Prologue`
```assembly
0804933f 55              PUSH       EBP
08049340 89 e5           MOV        EBP,ESP
08049342 83 ec 28        SUB        ESP,0x28
```

`Epilogue`
```assembly
080493e2 c9              LEAVE
080493e3 c3              RET
```

Préparation des arguments pour le `call` de `fseek`
```assembly
0804934b 83 ec 04        SUB        ESP,0x4                 @ Grandir le stack pointer
0804934e 6a 01           PUSH       0x1                     @ 3ème arg
08049350 6a ff           PUSH       -0x1                    @ 2ème arg
08049352 ff 75 08        PUSH       dword ptr [EBP + fp]    @ 1er arg
08049355 e8 16 fd        CALL       <EXTERNAL>::fseek       @ Call
0804935a 83 c4 10        ADD        ESP,0x10                @ Raccourcir le stack pointer
```

Préparation des arguments pour le `call` de `fputc`
```assembly
080493b6 83 ec 08        SUB        ESP,0x8                 @ Grandir le stack pointer
080493b9 ff 75 08        PUSH       dword ptr [EBP + fp]    @ 2ème arg
080493bc 50              PUSH       EAX                     @ 1er  arg
080493bd e8 4e fd        CALL       <EXTERNAL>::fputc       @ Call
080493c2 83 c4 10        ADD        ESP,0x10                @ Raccourcir le stack pointer

...

@ Même chose pour fgetc

080493c5 83 ec 0c        SUB        ESP,0xc
080493c8 ff 75 08        PUSH       dword ptr [EBP + fp]
080493cb e8 10 fd        CALL       <EXTERNAL>::fgetc 
080493d0 83 c4 10        ADD        ESP,0x10


```

`Gestion de variables dans le stack`
```assembly
08049345 c6 45 f7 07     MOV        byte ptr [EBP + var],0x7
...

08049352 ff 75 08        PUSH       dword ptr [EBP + fp]
...

0804935d 8b 45 f0        MOV        EAX,dword ptr [EBP + local_14]
08049360 0f b6 c0        MOVZX      EAX,AL
08049363 88 45 e7        MOV        byte ptr [EBP + local_1d],AL
08049366 c7 45 e0        MOV        dword ptr [EBP + local_24],0x2
            02 00 00 00
0804936d 8b 55 e0        MOV        EDX,dword ptr [EBP + local_24]
08049370 0f b6 45 e7     MOVZX      EAX,byte ptr [EBP + local_1d]

...

08049378 88 45 e7        MOV        byte ptr [EBP + local_1d],AL
0804937b 0f b6 45 e7     MOVZX      EAX,byte ptr [EBP + local_1d]
0804937f 88 45 ef        MOV        byte ptr [EBP + local_15],AL
08049382 0f b6 45 f7     MOVZX      EAX,byte ptr [EBP + var]
08049386 00 45 ef        ADD        byte ptr [EBP + local_15],AL
08049389 8b 45 f0        MOV        EAX,dword ptr [EBP + local_14]
0804938c 88 45 f7        MOV        byte ptr [EBP + var],AL
0804938f 0f b6 45 ef     MOVZX      EAX,byte ptr [EBP + local_15]
08049393 88 45 ee        MOV        byte ptr [EBP + local_16],AL
08049396 c7 45 e8        MOV        dword ptr [EBP + local_1c],0x2
            02 00 00 00
0804939d 8b 55 e8        MOV        EDX,dword ptr [EBP + local_1c]
080493a0 0f b6 45 ee     MOVZX      EAX,byte ptr [EBP + local_16]

...

080493a8 88 45 ee        MOV        byte ptr [EBP + local_16],AL
080493ab 0f b6 45 ee     MOVZX      EAX,byte ptr [EBP + local_16]
080493af 88 45 f7        MOV        byte ptr [EBP + var],AL
080493b2 0f b6 45 ef     MOVZX      EAX,byte ptr [EBP + local_15]
080493b6 83 ec 08        SUB        ESP,0x8
080493b9 ff 75 08        PUSH       dword ptr [EBP + fp]

...

080493c8 ff 75 08        PUSH       dword ptr [EBP + fp]

...

080493d3 89 45 f0        MOV        dword ptr [EBP + local_14],byteFromFile
080493d6 83 7d f0 ff     CMP        dword ptr [EBP + local_14],-0x1

```
| Base  + Offset   | Comment  |
|-----------|----------|
| `EBP`  +   param_1     | file stream pointer                                   |
| `EBP`  +   local_d     | Modifier constant variable to add with shifted byte   |
| `EBP`  +   local_14    | temp variable ?                                       |
| `EBP`  +   local_15    | to complete...    |
| `EBP`  +   local_16    | to complete...    |
| `EBP`  +   local_1c    | to complete...    |
| `EBP`  +   local_1d    | to complete...    |
| `EBP`  +   local_24    | to complete...    |




- Expliquez les instructions qui implémentent la logique de la fonction `encrypt1`.

Toute la logique du chiffrement se contient de cette boucle while. 

```assembly
            LAB_0804934b
  934b SUB  ESP,0x4
  934e PUSH 0x1
  9350 PUSH -0x1
  9352 PUSH dword ptr [EBP + fp]
  9355 CALL <EXTERNAL>::fseek
  935a ADD  ESP,0x10
  935d MOV  EAX,dword ptr [EBP + local
  9360 MOVZX EAX,AL
  9363 MOV  byte ptr [EBP + local_1d],
  9366 MOV  dword ptr [EBP + local_24]
  936d MOV  EDX,dword ptr [EBP + local
  9370 MOVZX EAX,byte ptr [EBP + local_
  9374 MOV  ECX,EDX
  9376 ROL  AL,CL
  9378 MOV  byte ptr [EBP + local_1d],
  937b MOVZX EAX,byte ptr [EBP + local_
  937f MOV  byte ptr [EBP + local_15],
  9382 MOVZX EAX,byte ptr [EBP + var]
  9386 ADD  byte ptr [EBP + local_15],
  9389 MOV  EAX,dword ptr [EBP + local
  938c MOV  byte ptr [EBP + var],AL
  938f MOVZX EAX,byte ptr [EBP + local_
  9393 MOV  byte ptr [EBP + local_16],
  9396 MOV  dword ptr [EBP + local_1c]
  939d MOV  EDX,dword ptr [EBP + local
  93a0 MOVZX EAX,byte ptr [EBP + local_
  93a4 MOV  ECX,EDX
  93a6 ROR  AL,CL
  93a8 MOV  byte ptr [EBP + local_16],
  93ab MOVZX EAX,byte ptr [EBP + local_
  93af MOV  byte ptr [EBP + var],AL
  93b2 MOVZX EAX,byte ptr [EBP + local_
  93b6 SUB  ESP,0x8
  93b9 PUSH dword ptr [EBP + fp]
  93bc PUSH EAX
  93bd CALL <EXTERNAL>::fputc
  93c2 ADD  ESP,0x10
```

Notre première partie du chiffrement se passe ici où la fonction fait des bitwise operations tél que des `shift` et un `or`.
```c
res = ((byte)byteFromFile << 2 | (byte)byteFromFile >> 6) + var
```
A compléter quelles instructions font quoi par rapport le chiffrement...

```assembly
0804935a 83 c4 10        ADD        ESP,0x10
0804935d 8b 45 f0        MOV        EAX,dword ptr [EBP + local_14]
08049360 0f b6 c0        MOVZX      EAX,AL
08049363 88 45 e7        MOV        byte ptr [EBP + local_1d],AL
08049366 c7 45 e0        MOV        dword ptr [EBP + local_24],0x2
            02 00 00 00
0804936d 8b 55 e0        MOV        EDX,dword ptr [EBP + local_24]
08049370 0f b6 45 e7     MOVZX      EAX,byte ptr [EBP + local_1d]
08049374 89 d1           MOV        ECX,EDX
08049376 d2 c0           ROL        AL,CL  
08049378 88 45 e7        MOV        byte ptr [EBP + local_1d],AL
0804937b 0f b6 45 e7     MOVZX      EAX,byte ptr [EBP + local_1d]
0804937f 88 45 ef        MOV        byte ptr [EBP + local_15],AL
08049382 0f b6 45 f7     MOVZX      EAX,byte ptr [EBP + var]
08049386 00 45 ef        ADD        byte ptr [EBP + local_15],AL
08049389 8b 45 f0        MOV        EAX,dword ptr [EBP + local_14]
0804938c 88 45 f7        MOV        byte ptr [EBP + var],AL
0804938f 0f b6 45 ef     MOVZX      EAX,byte ptr [EBP + local_15]
08049393 88 45 ee        MOV        byte ptr [EBP + local_16],AL
```

Deuxième partie mettre à jour la constante pour l'addition au byte.
A completer aussi...

```c
var = res >> 2 | res * '@';
```

```assembly
08049393 88 45 ee        MOV        byte ptr [EBP + local_16],AL
08049396 c7 45 e8        MOV        dword ptr [EBP + local_1c],0x2
            02 00 00 00
0804939d 8b 55 e8        MOV        EDX,dword ptr [EBP + local_1c]
080493a0 0f b6 45 ee     MOVZX      EAX,byte ptr [EBP + local_16]
080493a4 89 d1           MOV        ECX,EDX
080493a6 d2 c8           ROR        AL,CL
080493a8 88 45 ee        MOV        byte ptr [EBP + local_16],AL
080493ab 0f b6 45 ee     MOVZX      EAX,byte ptr [EBP + local_16]
080493af 88 45 f7        MOV        byte ptr [EBP + var],AL
080493b2 0f b6 45 ef     MOVZX      EAX,byte ptr [EBP + local_15]
080493b6 83 ec 08        SUB        ESP,0x8
080493b9 ff 75 08        PUSH       dword ptr [EBP + fp]
```

### Question 6.3 : Quelle/s instruction/s n’a/ont pas été vue/s en cours ? Que fait/font elle/s ?

ROR, operation de rotation de bits. (Différent du shift extended unsigned or signed)

(A compléter, surement d'autres instructions?)

```assembly
        080493a6 d2 c8           ROR        AL,CL
```

- _En attente de réponse._

### Manipulation 6.3

Écrivez le code d’un programme capable de déchiffrer les fichiers chiffrés par la fonction `encrypt1`.

---

## Fifth Part: Fonction `encrypt2`

### Manipulation 7.1

Dans Ghidra, afficher les codes C et assembleur de la fonction `encrypt2`.

### Question 7.1 : Que fait le code C affiché par Ghidra ?

- _En attente de réponse._

### Manipulation 7.2

Commentez le code C dans Ghidra, puis renommer et/ou "retyper" les variables locales en conséquence de la réponse à la question 7.1.

### Question 7.2 : Vous voulez restaurer les fichiers chiffrés avec `encrypt2`. Pourquoi n’est-il pas nécessaire de faire le reverse de la fonction appelée par `encrypt2` ?

- _En attente de réponse._

### Manipulation 7.3

Écrivez le code d’un programme capable de déchiffrer les fichiers chiffrés par la fonction `encrypt2`.

---

## Sixth Part: Réparation (6 points)

### Manipulation 8.1

En utilisant les programmes que vous avez codés, déchiffrez tous les documents fournis dans le dossier /home.

### Question 8.1 : Expliquez l’algorithme de déchiffrement que vous avez implémenté. Indiquez les formules mathématiques appliquées par chaque méthode de déchiffrement ainsi que la logique du flux d’instruction qui permet d’appliquer l’une ou l’autre.

### Manipulation 8.2

En utilisant Ghidra, patchez le programme `calc` pour qu’il puisse à nouveau être utilisé sans risque.
