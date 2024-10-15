# Préparation du TE1, de SLB

### Structure du Writeup

- [Préparation du TE1, de SLB](#préparation-du-te1-de-slb)
    - [Structure du Writeup](#structure-du-writeup)
    - [Introduction](#introduction)
    - [Symbol Table](#symbol-table)
    - [Description détaillée des fonctions](#description-détaillée-des-fonctions)
      - [Fonction `main`](#fonction-main)
    - [Gestion des arguments](#gestion-des-arguments)
    - [Boucle de jeu](#boucle-de-jeu)
    - [Procédé pour lancer le jeu et but général](#procédé-pour-lancer-le-jeu-et-but-général)
    - [But général du jeu](#but-général-du-jeu)
    - [Exemples de jeu](#exemples-de-jeu)
    - [Règles de score](#règles-de-score)
    - [Objectif final](#objectif-final)
      - [Fonction `askChoice`](#fonction-askchoice)
      - [Fonction `play`](#fonction-play)
    - [Vulnérabilités potentielles](#vulnérabilités-potentielles)
  - [Déconstruction de la fonction `getWord`](#déconstruction-de-la-fonction-getword)
    - [Déconstruction de la fonction `match`](#déconstruction-de-la-fonction-match)
    - [Explication rapide](#explication-rapide)
    - [Déconstruction de la fonction `exist`](#déconstruction-de-la-fonction-exist)
    - [Explication rapide exist](#explication-rapide-exist)
    - [Code commenté et renaming des variables de la fonction `initSecret`](#code-commenté-et-renaming-des-variables-de-la-fonction-initsecret)
    - [Explication des blocs de code](#explication-des-blocs-de-code)
    - [Déconstruction de la fonction `initSecret`](#déconstruction-de-la-fonction-initsecret)
    - [Analyse et vulnérabilités potentielles](#analyse-et-vulnérabilités-potentielles)
    - [Explication rapide de la fonction `flushInput`](#explication-rapide-de-la-fonction-flushinput)
    - [Code commenté et renaming des variables](#code-commenté-et-renaming-des-variables)
    - [Explication détaillée du code](#explication-détaillée-du-code)
    - [Fonctionnalité et usage dans le programme](#fonctionnalité-et-usage-dans-le-programme)
    - [Déconstruction de la fonction `countW`](#déconstruction-de-la-fonction-countw)
    - [Explication rapide countW](#explication-rapide-countw)
    - [Déconstruction de la fonction `eraseWord`](#déconstruction-de-la-fonction-eraseword)
    - [Explication rapide eraseWord](#explication-rapide-eraseword)
    - [Exemple d'utilisation dans ton programme](#exemple-dutilisation-dans-ton-programme)
      - [Exemple d'utilisation](#exemple-dutilisation)
    - [Analyse dynamique pour avoir toujours le mot de passe](#analyse-dynamique-pour-avoir-toujours-le-mot-de-passe)
    - [Table des points d'arrêt (Breakpoints) dans GDB](#table-des-points-darrêt-breakpoints-dans-gdb)
    - [Commandes GDB](#commandes-gdb)
    - [Table des offsets des variables](#table-des-offsets-des-variables)
  - [stack](#stack)
  - [main](#main)
  - [play](#play)
  - [match](#match)

### Introduction

Dans ce writeup, nous analysons un programme binaire conçu pour un jeu interactif basé sur un dictionnaire de mots. Le programme prend deux ou trois arguments en ligne de commande, et effectue diverses actions en fonction des choix faits par l'utilisateur. L'objectif de cette analyse est d'explorer le comportement interne du programme, de comprendre sa logique, et d'identifier des vulnérabilités potentielles, notamment liées à la gestion des entrées et de la mémoire.

### Symbol Table

Voici une table des symboles répertoriant les principales fonctions du programme et leurs adresses respectives en mémoire.

| Function Name | Location Address | Description |
|---------------|------------------|-------------|
| flushInput    | 0x08049256        | Vide le buffer d'entrée pour éviter les lectures intempestives. |
| countW        | 0x08049274        | Compte les mots, possiblement utilisé pour gérer le dictionnaire. |
| initSecret    | 0x080492e4        | Initialise un mot secret que le joueur doit deviner. |
| getWord       | 0x080493b8        | Récupère une entrée de l'utilisateur pour son essai de mot. |
| eraseWord     | 0x080493bc        | Efface un mot du dictionnaire. |
| exist         | 0x080493e4        | Vérifie si le mot donné existe dans le dictionnaire. |
| match         | 0x08049489        | Compare le mot donné par le joueur avec le mot secret. |
| play          | 0x08049619        | Logique principale du jeu, gère le score et les tours de jeu. |
| askChoice     | 0x08049712        | Demande à l'utilisateur de faire un choix dans le menu. |
| main          | 0x080497dd        | Point d'entrée du programme. Gère les arguments en ligne de commande. |

### Description détaillée des fonctions

#### Fonction `main`

La fonction **main** est le point d'entrée du programme. Elle prend deux ou trois arguments via la ligne de commande et gère la logique principale. Si un fichier dictionnaire est fourni, le programme l'ouvre et l'utilise pour jouer.

Voici une explication détaillée des blocs de code :

```c
undefined4 main(int argc, char *argv) { // param_1 avant modifcation argc : nombre d'arguments, param_2 avant modification :  argv : tableau d'arguments
    undefined4 returnCode;   // returnCode avant modification - Variable pour stocker le code de retour du programme
    FILE *filePointer;       // __stream avant modification - Pointeur pour manipuler des fichiers
    int choice;              // iVar12 avant modification - Stocke le choix de l'utilisateur (renvoyé par askChoice)
    int totalScore;          // local_18 avant modification - Variable pour stocker le score total accumulé
    int roundNumber;         // local_14 avant modification - Stocke le nombre de tours joués
    
     // Initialisation des variables
    roundNumber = 0;         // Le nombre de tours commence à zéro
    totalScore = 0;          // Le score total commence à zéro
    
    // Vérification du nombre d'arguments en ligne de commande
    if ((argc < 2) || (3 < argc)) {  
        // Si le nombre d'arguments est inférieur à 2 ou supérieur à 3, afficher l'utilisation correcte
        printf("Usage: %s path_2_dico\n", argv[0]);
        returnCode = 0xffffffff;  // Retourne -1 pour indiquer une erreur
    }
    else {
        // Si argc est égal à 2 (le programme est exécuté avec un seul argument supplémentaire)
        if (argc == 2) {
            // Copier l'argument (chemin du fichier dictionnaire) dans la variable `dico`
            strncpy(dico, argv[1], 0x100);
            
            // Tente d'ouvrir le fichier dictionnaire
            filePointer = fopen(dico, "r");
            if (filePointer == NULL) {
                // Si le fichier ne peut pas être ouvert, afficher un message d'erreur
                printf("Cannot find dico at %s\n", dico);
                return 0xffffffff;  // Retourner -1 pour indiquer une erreur
            }
            fclose(filePointer);  // Fermer le fichier une fois ouvert avec succès
        }
        else {
            // Si argc est égal à 3, vérifier que le deuxième argument est bien "-s"
            if ((argv[1][0] != '-') || (argv[1][1] != 's')) {
                // Si l'argument ne commence pas par "-s", afficher un message d'erreur
                printf("Unexpected value passed in argument: %s\n", argv[1]);
                return 0xffffffff;  // Retourner -1 pour indiquer une erreur
            }
            // Copier le troisième argument dans `dico` avec une longueur maximale de 20 caractères
            strncpy(dico, argv[2], 0x14);
            nodico = 1;  // Indiquer que le mot est défini manuellement (flag `nodico` à 1)
        }
        
        // Boucle principale du programme (jeu interactif)
        while (choice = askChoice(), choice != 3) {  // Continue tant que le choix n'est pas 3 (arrêt)
            if (choice < 4) {  // Si le choix est valide (entre 1 et 3)
                if (choice == 1) {
                    // Si l'utilisateur choisit de jouer (1), incrémenter le nombre de tours
                    roundNumber = roundNumber + 1;
                    
                    // Appeler la fonction `play` pour jouer le tour et mettre à jour le score
                    choice = play(roundNumber);
                    totalScore = totalScore + choice;  // Ajouter le résultat au score total
                    
                    // Afficher le score total à l'utilisateur
                    printf("[i] --> score total = %d\n", totalScore);
                }
                else {
                    if (choice != 2) goto LAB_08049988;  // Gérer les erreurs de choix
                    // Si l'utilisateur choisit de réinitialiser le score (2)
                    roundNumber = 0;   // Réinitialiser le nombre de tours
                    totalScore = 0;    // Réinitialiser le score total
                    printf("[i] --> score total = %d\n", 0);  // Afficher que le score est réinitialisé
                }
            }
            else {
                // Si un choix non valide est fait (supérieur à 3), afficher une erreur
LAB_08049988:
                fprintf(stderr, "ERREUR : option %d non disponible\n\n", choice);
            }
        }
        // Une fois la boucle terminée, afficher un message de fin
        puts(&DAT_0804a18f);  // Afficher un message de fin du jeu
        returnCode = 0;  // Retourner 0 pour indiquer que le programme s'est terminé correctement
    }
    return returnCode;  // Retourner le code de retour
}
```

### Gestion des arguments

- **Deux arguments** : Si le programme reçoit deux arguments, le deuxième argument est interprété comme le chemin vers un fichier dictionnaire. Le programme va tenter de lire ce fichier pour choisir un mot secret aléatoire à deviner.
  
- **Trois arguments avec `-s`** : Si le programme reçoit trois arguments, le deuxième argument doit être **`-s`**, ce qui signifie que le joueur choisit manuellement le mot secret. Le troisième argument sera le mot secret choisi par le joueur, qui sera utilisé dans le jeu. Ce mot est limité à 20 caractères.

### Boucle de jeu

Le programme entre dans une boucle où l'utilisateur doit choisir parmi trois options : **jouer**, **réinitialiser le score**, ou **quitter**. Le joueur peut continuer à jouer ou réinitialiser le score à chaque tour.

### Procédé pour lancer le jeu et but général

Le programme est un jeu de type **Motus**, où l'utilisateur doit deviner un mot secret. Le comportement du programme dépend du nombre d'arguments fournis à l'exécution :

1. **Lancer avec un fichier dictionnaire** :

   ```bash
   ./programme ./chemin_vers_dictionnaire.txt
   ```

   Dans ce cas, le programme choisit un mot secret aléatoire dans le fichier dictionnaire fourni.

2. **Lancer avec le flag `-s`** :

   ```bash
   ./programme -s "votremot"
   ```

   Avec l'option **`-s`**, l'utilisateur définit lui-même le mot secret. Ce mot reste fixe et utilisé dans toutes les parties jusqu'à ce qu'un nouveau mot soit défini.

---

### But général du jeu

Le but du jeu est de **deviner un mot secret**. À chaque tour, l'utilisateur entre un mot, et le programme le compare avec le mot secret. L'interface du jeu fournit des **indices visuels** pour aider le joueur à affiner ses suppositions :

- Les lettres correctes et bien placées sont marquées par un **!**.
- Les lettres présentes dans le mot secret, mais mal placées, sont marquées par un **.**.

### Exemples de jeu

1. **Mode Jouer** :  
   L'utilisateur entre un mot, par exemple, **"vehicule"**, et le programme affiche une comparaison visuelle indiquant les lettres bien placées et celles mal placées :

   ```bash
   [?] proposition : vehicule
   [i] comparaison : vehicule
   [i] ------------> .e...!.!
   [i] round 1, iteration 6
   ```

2. **Réinitialiser le score** :  
   Le joueur peut réinitialiser le score à zéro à tout moment en choisissant l'option 2 du menu.

3. **Quitter** :  
   Le jeu se termine lorsque l'utilisateur choisit l'option 3.

---

### Règles de score

- Le score initial est fixé à 10 points et diminue à chaque tentative incorrecte.
- Le score total est mis à jour après chaque partie, et le joueur peut faire plusieurs essais dans chaque round.

### Objectif final

Le joueur doit **deviner le mot secret** en utilisant les indices fournis à chaque tour. Le jeu se poursuit jusqu'à ce que le mot soit trouvé ou que le joueur décide de quitter.

#### Fonction `askChoice`

Cette fonction est utilisée pour recueillir l'entrée de l'utilisateur et déterminer quelle action doit être exécutée. Elle se base sur les choix offerts à l'utilisateur dans un menu, et la validation de l'entrée se fait par conversion de caractères en valeurs numériques.

**Table des appels pour `askChoice`** :

| Function Called | Location Address | Description |
|-----------------|------------------|-------------|
| getchar         | N/A              | Fonction standard de la bibliothèque pour la capture d'un caractère d'entrée utilisateur |
| flushInput      | 0x08049256        | Fonction pour vider le buffer d'entrée et éviter des erreurs de traitement ultérieures |

```c
int askChoice(void)
{
  char userInputChar;    // cVar1 avant modification - Représente le caractère saisi par l'utilisateur
  int rawUserInput;      // iVar2 avant modification - Stocke la valeur brute retournée par `getchar()` (ASCII)
  int gameOption;        // local_10 avant modification - Représente le choix de jeu final de l'utilisateur
  
  gameOption = 0;        // Initialisation de l'option de jeu à zéro
  puts("\n--------------------");   // Affichage de la ligne de séparation pour le menu
  
  // Boucle jusqu'à ce qu'un choix valide (1 à 3) soit fait
  while ((gameOption < 1 || (3 < gameOption))) {
    puts("[m] Select");              // Affichage des options du menu
    puts("[1] play");                // Option 1 : Jouer
    puts("[2] reset");               // Option 2 : Réinitialiser le score
    puts("[3] stop");                // Option 3 : Arrêter le jeu
    printf("\n[?] Your choice: ");
    
    rawUserInput = getchar();        // Lecture de l'entrée utilisateur
    userInputChar = (char)rawUserInput;  // Conversion en caractère (de ASCII à char)
    
    // Vérification si l'entrée est valide (doit être entre '1' et '3')
    if ((userInputChar < '1') || ('3' < userInputChar)) {
      gameOption = -1;   // Si entrée invalide, on attribue -1 pour indiquer une erreur
    }
    else {
      gameOption = userInputChar - '0';  // Conversion du caractère ASCII en entier
    }
    
    if (gameOption < 0) {  // Si le choix est invalide, réafficher le menu
      puts("\n--------------------");
    }
    
    flushInput();  // Vider le buffer d'entrée pour éviter d'avoir des caractères résiduels
  }
  
  return gameOption;  // Retourne le choix valide de l'utilisateur (1, 2 ou 3)
}

```

- **Fonctionnalité** : La fonction **askChoice** permet de capturer le choix de l'utilisateur à partir d'un menu et retourne une valeur valide entre 1 et 3. Toute autre entrée est rejetée, et le menu est réaffiché jusqu'à ce qu'une entrée valide soit saisie.
- **Sécurité** : Un risque potentiel ici est que, bien que les caractères hors de la plage '1'-'3' soient rejetés, il n'y a pas de gestion pour des entrées non numériques, ce qui pourrait introduire des comportements imprévus.

#### Fonction `play`

La fonction **play** contient la logique principale du jeu, où l'utilisateur tente de deviner un mot secret. Le nombre d'essais est limité à 10, et le score diminue à chaque tentative incorrecte.

**Table des appels pour `play`** :

| Function Called | Location Address | Description |
|-----------------|------------------|-------------|
| strlen          | N/A              | Fonction standard pour calculer la longueur de la chaîne (ici, celle du mot secret) |
| initSecret      | 0x080492e4        | Initialise le mot secret que le joueur doit deviner |
| getWord         | 0x080493b8        | Récupère le mot deviné par le joueur |
| exist           | 0x080493e4        | Vérifie si le mot donné par le joueur existe dans le dictionnaire |
| match           | 0x08049489        | Compare le mot donné avec le mot secret pour voir s'ils correspondent |

```c
int play(undefined param_1)   // param_1 indique le numéro du tour
{
  size_t secretWordLength;  // sVar1 avant modification - Stocke la longueur du mot secret
  int doesWordExist;        // iVar2 avant modification - Indique si le mot deviné existe dans le dictionnaire
  char playerGuessBuffer[20]; // local_40 avant modification - Buffer pour stocker l'essai du joueur
  char secretWord[20];      // local_2c avant modification - Contient le mot secret que le joueur doit deviner
  int isMatch;              // local_18 avant modification - Indique si le mot deviné correspond au mot secret
  int score;                // local_14 avant modification - Stocke le score actuel du joueur
  int attempts;             // local_10 avant modification - Compteur pour le nombre d'essais du joueur
  
  attempts = 0;  // Initialisation du nombre d'essais
  score = 10;    // Le score initial du joueur est 10
  initSecret(secretWord);  // Initialise le mot secret à deviner
  
  secretWordLength = strlen(secretWord);  // Calculer la longueur du mot secret
  printf("[i] secret word is %d chars long\n", secretWordLength);  // Affiche la longueur du mot
  
  do {
    attempts++;  // Incrémente le nombre d'essais du joueur
    printf("[i] round %d, iteration %d\n", roundNumber, attempts);  // Affiche le numéro du tour et de l'essai
    
    getWord(playerGuessBuffer);  // Récupère le mot deviné par le joueur
    printf("[i] comparaison : ");
    puts(playerGuessBuffer);  // Affiche le mot deviné par le joueur
    
    doesWordExist = exist(playerGuessBuffer);  // Vérifie si le mot existe dans le dictionnaire
    if (doesWordExist == 0) {   // Si le mot n'existe pas
LAB_080496d5:
      isMatch = 0;   // Le mot ne correspond pas
    }
    else {
      doesWordExist = match(playerGuessBuffer, secretWord);  // Compare le mot deviné au mot secret
      if (doesWordExist == 0) goto LAB_080496d5;   // Si le mot ne correspond pas, aller au label LAB_080496d5
      isMatch = 1;   // Le mot correspond
    }
    
    score = score - (uint)(isMatch == 0);   // Si le mot ne correspond pas, réduire le score
    if ((9 < attempts) || (isMatch != 0)) {  // Si 10 essais sont faits ou si le mot est deviné correctement
      printf("[i] --> score du jeu = %d\n", score);  // Affiche le score final du jeu
      return score;  // Retourner le score final
    }
  } while (true);   // Boucle infinie tant que les conditions ne sont pas remplies
}

```

- **Logique** : Le joueur a jusqu'à 10 tentatives pour deviner le mot secret. À chaque tentative incorrecte, le score diminue. Si le joueur devine correctement ou atteint le nombre maximum de tentatives, la fonction retourne le score.
- **Sécurité** : Cette fonction présente un potentiel **buffer overflow**. Les buffers pour les mots devinés et secrets (chaque buffer ayant une taille de 20 caractères) peuvent être débordés si les entrées ne sont pas correctement limitées par des vérifications adéquates.

### Vulnérabilités potentielles

- **Débordement de buffer (Buffer Overflow)** : L'utilisation de `strncpy` et des buffers statiques de 20 caractères (comme `secretWord` et `playerGuessBuffer`) pourrait entraîner un débordement de buffer si les chaînes manipulées dépassent cette taille. Cela pourrait corrompre les données ou permettre l'exécution de code malveillant.
  
- **Entrée non sécurisée** : L'utilisation de **getchar()** et l'absence de validation de la longueur des entrées utilisateur dans **askChoice** et **play** peuvent permettre à un utilisateur d'entrer des données non valides, ce qui peut entraîner des comportements imprévisibles.

## Déconstruction de la fonction `getWord`

La fonction **getWord** est utilisée pour capturer l'essai de mot du joueur. Elle permet d'obtenir l'entrée utilisateur (le mot que le joueur devine) et de la stocker dans un buffer pour un traitement ultérieur. Cela se produit lors de chaque tentative du joueur pendant le déroulement du jeu.

Cette fonction est cruciale pour le bon déroulement de la partie, car elle interagit directement avec les autres parties du programme, notamment les fonctions de comparaison comme **exist** et **match** qui valident l'essai du joueur.

```c
void getWord(char *userGuess) {  // Renommé param_1 en userGuess, car il s'agit du mot que l'utilisateur propose
    
    // Affiche un message à l'utilisateur pour qu'il entre une proposition de mot
    printf("\n[?] proposition : ");
    
    // Lit l'entrée utilisateur et stocke le mot dans le buffer userGuess (limité à 19 caractères)
    __isoc99_scanf("%19s", userGuess);
    
    // Vide le buffer d'entrée pour éviter tout caractère résiduel, comme un retour à la ligne
    flushInput();
    
    return;
}
```

### Déconstruction de la fonction `match`

```c
undefined4 match(char *userGuess, char *secretWord) {  // Renommé param_1 en userGuess, param_2 en secretWord
    int matchStatus;          // Renommé iVar1 - Indique le statut de la comparaison pour chaque lettre
    undefined4 wordMatch;     // Renommé uVar2 - Indique si les mots correspondent (1) ou non (0)
    int letterStatus[20];     // Renommé aiStack_70 - Tableau pour stocker le statut de chaque lettre du mot deviné
    size_t userGuessLength;   // Renommé local_1c - Longueur du mot deviné par l'utilisateur
    size_t secretWordLength;  // Renommé local_20 - Longueur du mot secret
    uint isLengthMatch;       // Renommé local_18 - Indique si les longueurs des deux mots sont égales (1 ou 0)
    int i, j;                 // local_10 et local_14 - Variables d'index pour les boucles for
    
    // Calculer les longueurs du mot deviné et du mot secret
    userGuessLength = strlen(userGuess);
    secretWordLength = strlen(secretWord);
    
    // Vérifie si les deux mots ont la même longueur
    isLengthMatch = (uint)(secretWordLength == userGuessLength);
    
    // Initialiser le tableau letterStatus à 0 (pas de correspondance initiale pour les lettres)
    for (i = 0; i < 20; i++) {
        letterStatus[i] = 0;
    }
    
    // Comparer chaque lettre des deux mots, lettre par lettre
    for (i = 0; i < (int)secretWordLength; i++) {
        // Si la lettre à la position i dans userGuess correspond à celle de secretWord
        if (userGuess[i] == secretWord[i]) {
            letterStatus[i] = 1;  // Lettre correctement placée
        } else {
            isLengthMatch = 0;    // Le mot ne correspond pas complètement
        }
    }
    
    // Si les mots ne sont pas identiques, chercher les lettres mal placées
    if (isLengthMatch == 0) {
        // Comparer chaque lettre du mot deviné avec chaque lettre du mot secret
        for (j = 0; j < (int)userGuessLength; j++) {
            for (i = 0; i < (int)secretWordLength; i++) {
                // Si une lettre de userGuess existe dans secretWord mais n'est pas déjà bien placée
                if ((letterStatus[j] == 0) && (userGuess[j] == secretWord[i])) {
                    letterStatus[j] = 2;  // Lettre présente mais mal placée
                }
            }
        }
        
        // Afficher le résultat de la comparaison
        printf("[i] ------------> ");
        for (j = 0; j < (int)userGuessLength; j++) {
            matchStatus = letterStatus[j];
            if (matchStatus == 2) {
                putchar('!');  // Lettre présente mais mal placée
            } else if (matchStatus == 0) {
                putchar('.');  // Lettre absente du mot secret
            } else if (matchStatus == 1) {
                putchar(userGuess[j]);  // Lettre bien placée
            }
        }
        putchar(10);  // Nouvelle ligne
        wordMatch = 0;  // Les mots ne sont pas identiques
    } else {
        wordMatch = 1;  // Les mots sont identiques
    }
    
    return wordMatch;  // Retourne 1 si les mots correspondent, 0 sinon
}
```

### Explication rapide

1. **Paramètres `userGuess` et `secretWord` :**  
   - **`userGuess`** : Le mot proposé par le joueur.
   - **`secretWord`** : Le mot secret à deviner.

2. **Longueur des mots :**  
   Les longueurs des deux mots sont calculées avec **`strlen()`** et comparées pour vérifier si elles sont égales.

3. **Comparaison des lettres :**  
   - La première boucle **`for`** compare chaque lettre du mot deviné avec celle du mot secret à la même position. Si une lettre est bien placée, elle est marquée dans le tableau **`letterStatus`** avec un **1**.
   - Si les longueurs des mots sont différentes ou si des lettres ne correspondent pas, **`isLengthMatch`** est mis à **0**.

4. **Recherche des lettres mal placées :**  
   - La seconde boucle **`for`** vérifie si des lettres du mot deviné existent dans le mot secret, mais à des positions différentes. Ces lettres sont marquées avec un **2** dans **`letterStatus`**.

5. **Affichage des résultats :**  
   - Les lettres bien placées sont affichées telles quelles.
   - Les lettres mal placées sont signalées par **`!`**.
   - Les lettres absentes sont marquées par un **`.`**.

6. **Retour de la fonction :**  
   - La fonction retourne **`1`** si les mots sont identiques, sinon **`0`**.

---

Cette fonction est utilisée pour comparer le mot deviné avec le mot secret et fournir un retour visuel (correct, mal placé ou absent) afin d'aider le joueur à deviner correctement le mot.

### Déconstruction de la fonction `exist`

```c
undefined4 exist(char *userGuess) {  // Renommé param_1 en userGuess, car c'est le mot que l'utilisateur a proposé
    undefined4 wordExists;           // Renommé uVar1 - Indique si le mot existe (1) ou non (0)
    int comparisonResult;            // Renommé iVar2 - Résultat de la comparaison entre les mots
    char dictionaryWord[20];         // Renommé local_24 - Stocke un mot du dictionnaire
    FILE *dictionaryFile;            // Renommé local_10 - Pointeur pour ouvrir le fichier dictionnaire
    
    // Si le mot est tiré d'un dictionnaire (nodico == 0)
    if (nodico == 0) {
        // Ouvre le fichier dictionnaire
        dictionaryFile = fopen(dico, "r");
        
        // Si le fichier ne peut pas être ouvert
        if (dictionaryFile == NULL) {
            // Indiquer que le mot n'existe pas
            puts("[i] ------------> word does not exist!");
            wordExists = 0;
        }
        else {
            // Lire chaque mot du dictionnaire et comparer avec userGuess
            do {
                comparisonResult = __isoc99_fscanf(dictionaryFile, "%19s", dictionaryWord);
                
                // Si fin du fichier atteinte ou erreur
                if (comparisonResult == -1) {
                    fclose(dictionaryFile);
                    puts("[i] ------------> word does not exist!");
                    wordExists = 0;
                    return wordExists;
                }
                
                // Comparer le mot proposé avec le mot du dictionnaire
                comparisonResult = strcmp(userGuess, dictionaryWord);
            } while (comparisonResult != 0);  // Répéter jusqu'à trouver une correspondance
            
            // Fermer le fichier et indiquer que le mot existe
            fclose(dictionaryFile);
            wordExists = 1;
        }
    }
    else {
        // Si nodico == 1, le mot est défini manuellement, il est donc supposé exister
        wordExists = 1;
    }
    
    return wordExists;
}
```

### Explication rapide exist

1. **Paramètre `userGuess`** : C'est le mot proposé par l'utilisateur.
2. **Ouverture du dictionnaire** : Si le jeu utilise un fichier dictionnaire (**`nodico == 0`**), le fichier est ouvert avec **`fopen()`**.
3. **Lecture et comparaison** : Chaque mot du dictionnaire est lu et comparé au mot proposé par l'utilisateur avec **`strcmp()`**. Si une correspondance est trouvée, la fonction renvoie **1** pour indiquer que le mot existe.
4. **Gestion de la fin de fichier** : Si la fin du fichier est atteinte sans correspondance, la fonction affiche un message indiquant que le mot n'existe pas et renvoie **0**.
5. **Mode manuel `-s`** : Si le mode manuel (**`nodico == 1`**) est activé, le mot est supposé exister par défaut, et la fonction renvoie **1**.

Cette fonction permet de vérifier si un mot proposé existe dans le dictionnaire. Elle assure que les mots proposés sont valides dans le contexte du jeu.

### Code commenté et renaming des variables de la fonction `initSecret`

```c
void initSecret(char *secretWord) {  // Renommé param_1 en secretWord car il s'agit du mot secret à deviner
    int wordCount;         // iVar1 avant modification - Nombre total de mots dans le dictionnaire (via countW)
    int randomIndex;       // iVar2 avant modification - Index aléatoire pour sélectionner un mot dans le dictionnaire
    FILE *dictionaryFile;  // __stream avant modification - Pointeur de fichier pour lire le dictionnaire
    int result;            // iVar3 avant modification - Stocke le résultat de fscanf lors de la lecture du dictionnaire
    int currentWord = 0;   // local_10 avant modification - Compteur du nombre de mots lus dans le fichier
    
    // Vérifie si un dictionnaire est utilisé (nodico == 0) ou si le mot secret est défini manuellement (nodico == 1)
    if (nodico == 0) {
        *secretWord = '\0';  // Initialise le mot secret à une chaîne vide
        
        // Appelle countW pour obtenir le nombre de mots dans le dictionnaire
        wordCount = countW();  
        
        // Si le dictionnaire n'est pas vide
        if (wordCount != 0) {
            randomIndex = rand();  // Génère un index aléatoire basé sur rand()

            // Ouvre le fichier dictionnaire
            dictionaryFile = fopen(dico, "r");
            
            // Si le fichier dictionnaire est ouvert correctement
            if (dictionaryFile != NULL) {
                // Boucle qui lit le fichier dictionnaire mot par mot
                do {
                    // Lit un mot depuis le fichier dictionnaire et le stocke dans secretWord
                    result = __isoc99_fscanf(dictionaryFile, "%19s", secretWord);
                    
                    // Si fscanf renvoie -1, cela signifie que la fin du fichier est atteinte ou qu'il y a une erreur
                    if (result == -1) break;
                    
                    // Incrémente le compteur de mots lus
                    currentWord = currentWord + 1;

                // Continue jusqu'à atteindre le mot aléatoire (basé sur randomIndex)
                } while (currentWord < randomIndex % wordCount + 1);
                
                // Ferme le fichier dictionnaire après la lecture
                fclose(dictionaryFile);
            }
        }
    } 
    else {
        // Si nodico == 1, copie simplement le mot défini manuellement dans secretWord
        strcpy(secretWord, dico);
    }
    
    return;
}
```

### Explication des blocs de code

1. **Initialisation du mot secret** :
   - Si **`nodico == 0`**, cela signifie qu'un fichier dictionnaire est utilisé. Dans ce cas, la fonction initialise **`secretWord`** avec une chaîne vide (`*secretWord = '\0'`) avant de choisir un mot du dictionnaire.

2. **Choix d'un mot aléatoire dans le dictionnaire** :
   - La fonction appelle **`countW()`** pour obtenir le nombre total de mots dans le fichier dictionnaire.
   - Si le dictionnaire contient au moins un mot, un index aléatoire est généré via **`rand()`**. Cet index sert à choisir un mot au hasard parmi ceux présents dans le fichier dictionnaire.
   - La fonction ouvre ensuite le fichier dictionnaire avec **`fopen()`** et parcourt les mots un par un jusqu'à atteindre l'index aléatoire calculé.

3. **Cas où `-s` est utilisé** :
   - Si **`nodico == 1`**, cela signifie que le joueur a défini manuellement le mot secret (via l'argument **`-s`** en ligne de commande). La fonction utilise alors **`strcpy()`** pour copier le mot fourni dans **`secretWord`**.

---

### Déconstruction de la fonction `initSecret`

La fonction **`initSecret`** a pour but d'initialiser le mot secret que le joueur doit deviner. Selon la valeur de la variable globale **`nodico`**, le mot peut être choisi aléatoirement dans un fichier dictionnaire ou être défini manuellement par le joueur via l'option **`-s`**. Voici une déconstruction détaillée :

1. **Paramètre d'entrée :**
   - La fonction prend un seul paramètre, **`char *secretWord`**, qui représente le mot secret à deviner. Il s'agit d'un pointeur vers une chaîne de caractères où le mot sera stocké.

2. **Variable globale `nodico` :**
   - Cette variable détermine la méthode utilisée pour choisir le mot secret :
     - **`nodico == 0`** : Le mot secret est choisi aléatoirement depuis un fichier dictionnaire.
     - **`nodico == 1`** : Le joueur a fourni un mot via l'option **`-s`**, et ce mot est copié directement dans **`secretWord`**.

3. **Choix du mot dans le dictionnaire :**
   - Si **`nodico == 0`**, la fonction commence par initialiser **`secretWord`** à une chaîne vide, puis elle compte le nombre de mots dans le fichier dictionnaire via **`countW()`**.
   - Si le dictionnaire n'est pas vide, un index aléatoire est généré par **`rand()`** pour sélectionner un mot.
   - La fonction ouvre ensuite le fichier dictionnaire et lit les mots un par un en utilisant **`fscanf()`**. Une boucle s'exécute jusqu'à ce que le mot correspondant à l'index aléatoire soit trouvé.
   - Une fois le mot trouvé, il est stocké dans **`secretWord`** et le fichier dictionnaire est fermé.

4. **Cas du mot défini manuellement :**
   - Si **`nodico == 1`**, cela signifie que l'utilisateur a passé un mot directement via la ligne de commande (avec **`-s`**). Dans ce cas, la fonction utilise **`strcpy()`** pour copier ce mot dans **`secretWord`** sans chercher dans le dictionnaire.

---

### Analyse et vulnérabilités potentielles

1. **Overflow potentiel :**
   - La fonction utilise **`strncpy()`** pour limiter la copie du mot du dictionnaire à 20 caractères, ce qui est une bonne pratique pour éviter des dépassements de tampon. Cependant, l'utilisation de **`strcpy()`** pour copier un mot défini manuellement dans **`secretWord`** pourrait entraîner un dépassement de tampon si la longueur du mot fourni dépasse la capacité du buffer.

2. **Vulnérabilité liée au fichier dictionnaire :**
   - La fonction ne vérifie pas si le fichier dictionnaire contient des mots valides. Si le fichier est vide ou corrompu, la boucle de sélection pourrait ne pas fonctionner comme prévu.

3. **Randomisation faible :**
   - L'utilisation de **`rand()`** pour choisir un mot aléatoire dans le dictionnaire pourrait ne pas être suffisamment aléatoire si le générateur de nombres aléatoires n'est pas initialisé correctement avec **`srand()`**. Cela pourrait entraîner une faible variation dans le choix des mots.

### Explication rapide de la fonction `flushInput`

La fonction **`flushInput`** vide le buffer d'entrée après une lecture utilisateur, évitant que des caractères résiduels (comme **`\n`**) n'interfèrent avec les prochaines entrées.

- **`getchar()`** lit chaque caractère un par un dans le buffer.
- La lecture continue jusqu'à ce qu'un **`\n`** (nouvelle ligne) soit rencontré, signalant que l'entrée utilisateur est terminée.
- Si **`getchar()`** renvoie **`-1`** (EOF ou erreur), la fonction s'arrête.

### Code commenté et renaming des variables

Voici le code avec des noms de variables plus clairs :

```c
void flushInput(void) {
    int inputChar;  // Stocke le caractère lu depuis l'entrée utilisateur
    
    do {
        inputChar = getchar();  // Lire un caractère dans le buffer
        
        // Si un saut de ligne ('\n') est trouvé, la fonction termine
        if ((char)inputChar == '\n') {
            return;
        }
    } 
    // Continue jusqu'à rencontrer EOF (-1)
    while ((char)inputChar != -1);
    
    return;
}
```

### Explication détaillée du code

1. **Variable `inputChar` :**  
   **`inputChar`** stocke le caractère lu par **`getchar()`**, et est vérifié pour détecter **`\n`** ou **`-1`** (EOF).

2. **Boucle `do-while` :**  
   - **`getchar()`** lit chaque caractère.
   - La boucle s'arrête dès que **`\n`** ou **`-1`** est lu.

3. **Condition de sortie :**  
   La fonction se termine dès qu'un **`\n`** ou **`-1`** est rencontré.

### Fonctionnalité et usage dans le programme

**`flushInput`** assure qu'aucun caractère indésirable (comme **`\n`**) ne reste dans le buffer après une entrée utilisateur, évitant des perturbations lors de futures lectures.

### Déconstruction de la fonction `countW`

```c
int countW(void) {
    FILE *dictionaryFile;  // Renommé __stream en dictionaryFile - Fichier du dictionnaire à ouvrir
    int charRead;          // Renommé iVar1 en charRead - Stocke le caractère lu
    int wordCount = 0;     // Renommé local_10 en wordCount - Compteur de mots (basé sur le nombre de sauts de ligne)
    
    // Ouvre le fichier dictionnaire en mode lecture
    dictionaryFile = fopen(dico, "r");
    
    // Si le fichier a été ouvert avec succès
    if (dictionaryFile != NULL) {
        // Lire le fichier caractère par caractère jusqu'à la fin du fichier
        while ((charRead = feof(dictionaryFile)) == 0) {
            charRead = fgetc(dictionaryFile);  // Lire un caractère du fichier
            
            // Incrémenter le compteur de mots pour chaque saut de ligne '\n'
            wordCount += (charRead == '\n');
        }
        
        // Fermer le fichier dictionnaire après avoir fini de lire
        fclose(dictionaryFile);
    }
    
    // Retourner le nombre de mots (ou lignes) trouvés dans le fichier
    return wordCount;
}
```

### Explication rapide countW

1. **Paramètre de sortie :**
   - La fonction **`countW`** renvoie le nombre de mots (ou plutôt le nombre de lignes) dans un fichier dictionnaire, car chaque mot est séparé par un saut de ligne **`\n`**.

2. **Ouverture du fichier :**
   - **`fopen(dico, "r")`** ouvre le fichier contenant les mots. Si le fichier n'est pas trouvé, la fonction retourne 0 (car **`wordCount`** reste à 0).

3. **Lecture caractère par caractère :**
   - La fonction utilise **`fgetc()`** pour lire chaque caractère du fichier dictionnaire jusqu'à atteindre la fin du fichier. Elle incrémente le compteur **`wordCount`** chaque fois qu'un saut de ligne **`\n`** est rencontré, ce qui correspond à un mot dans le dictionnaire.

4. **Fermeture du fichier :**
   - Une fois la lecture terminée, **`fclose()`** est appelé pour fermer le fichier.

5. **Retour du nombre de mots :**
   - La fonction retourne **`wordCount`**, qui correspond au nombre de mots dans le dictionnaire (en réalité, au nombre de lignes).

Voici la fonction **`eraseWord`** avec des noms de variables plus explicites, des commentaires ajoutés, et un exemple d'utilisation possible dans ton programme.

### Déconstruction de la fonction `eraseWord`

```c
void eraseWord(int wordAddress) {  // Renommé param_1 en wordAddress - Adresse du mot à effacer
    undefined4 index;              // Renommé local_8 en index - Index pour parcourir les caractères du mot
    
    // Parcourt chaque caractère du mot (jusqu'à 20 caractères, soit 0x14 en hexadécimal)
    for (index = 0; index < 0x14; index = index + 1) {
        // Remplace chaque caractère du mot par un point '.' (0x2e en ASCII)
        *(char *)(wordAddress + index) = '.';
    }
    
    return;
}
```

### Explication rapide eraseWord

1. **Paramètre `wordAddress` :**
   - **`wordAddress`** est l'adresse du mot à effacer. Il s'agit d'un pointeur vers la mémoire où le mot est stocké.

2. **Boucle `for` :**
   - La boucle parcourt chaque caractère du mot (jusqu'à 20 caractères, car la limite est définie à **0x14**, soit 20 en décimal).
   - À chaque itération, le caractère à l'adresse donnée par **`wordAddress + index`** est remplacé par un point **`.`** (dont le code ASCII est **0x2e**).

3. **Effacement du mot :**
   - Le mot est "effacé" en remplaçant chacun de ses caractères par des points. Le programme ne supprime pas réellement le mot, mais le masque visuellement en affichant des points à la place des lettres.

---

### Exemple d'utilisation dans ton programme

La fonction **`eraseWord`** pourrait être utilisée dans un contexte où il est nécessaire de masquer ou de réinitialiser visuellement un mot, par exemple :

#### Exemple d'utilisation

Supposons que tu veuilles effacer le mot proposé par l'utilisateur après chaque tour dans le jeu, pour éviter qu'il soit affiché sur l'écran après une nouvelle tentative. Après chaque proposition du joueur, **`eraseWord`** pourrait être appelée pour masquer le mot proposé, en le remplaçant par des points.

```c
char userGuess[20];

// Supposons que l'utilisateur entre une proposition
getWord(userGuess);

// Comparaison du mot avec le mot secret
if (match(userGuess, secretWord) == 0) {
    // Si le mot ne correspond pas, effacer (ou masquer) le mot proposé
    eraseWord((int)userGuess);
}
```

Dans cet exemple, après avoir comparé le mot proposé avec le mot secret, si le joueur a échoué, le mot deviné est effacé (remplacé par des points) pour éviter qu'il ne soit réutilisé visuellement. Cela peut aider à rendre le jeu plus interactif en masquant les tentatives précédentes.

---

### Analyse dynamique pour avoir toujours le mot de passe

gdb avec breakpoint après l'appel de initSecret

1. b play
2. r ./dict_8-10.txt
3. when prompted, chose 1 to play
4. disass
5. b *0x08049643  
6. c
7. x/s $ebp-0x28

### Table des points d'arrêt (Breakpoints) dans GDB

Cette table montre où placer les points d'arrêt dans la fonction pour analyser le comportement de la fonction **getWord** pendant son exécution.

| Breakpoint Address | Commentaires |
|--------------------|--------------|
| 0x08049643         | Appel de la fonction **strlen**, qui calcule la longueur du mot secret avec `secretWordLength = strlen(secretBuffer);` |

### Commandes GDB

Voici quelques commandes utiles pour manipuler le contenu des buffers pendant le débogage avec GDB :

- **Lire le mot secret depuis la pile (stack)** : Cette commande permet de lire le contenu du buffer contenant le mot secret à partir de l'adresse EBP - 0x28, où est stocké le buffer **secretBuffer**.
  
  ```gdb
  x/s $ebp-0x28
  ```

- **Écraser le buffer contenant le mot deviné par le joueur** : Cette commande permet d'injecter un nouveau mot deviné directement dans la pile à l'adresse EBP - 0x3c, où est stocké le buffer **playerGuessBuffer** parce que à la ligne `0x08049679` nous trouvons    LEA        secretWordLength=>playerGuessBuffer,[EBP + -0x3c] on peut voir cela dans instruction info en cliquant dessus.
.

  ```gdb
  set {char[20]} $ebp-0x3c = "YOUR_NEW_GUESS"
  ```

Ces commandes sont particulièrement utiles pour tester la fonction **getWord** et pour analyser la façon dont les données sont stockées et manipulées au cours de l'exécution du programme.

### Table des offsets des variables

Voici une table des décalages mémoire des principales variables utilisées dans la fonction **play** et liées à **getWord**. Ces informations sont essentielles pour comprendre comment les buffers sont gérés dans la pile.

| Nom de la variable      | Décalage (Offset) |
|-------------------------|-------------------|
| secretBuffer            | EBP - 0x28        |
| playerGuessBuffer       | EBP - 0x3c        |

Ces offsets indiquent où dans la pile se trouvent les buffers **secretBuffer** (mot secret à deviner) et **playerGuessBuffer** (mot deviné par le joueur), et peuvent être utilisés pour déboguer et analyser les interactions entre les différentes parties du programme.

## stack

## main

| adresse hexa  | contenu  | valeur typée  | mot hexa  |
|---|---|---|---|
|   |   |   |   |
|-0xc|local_18 => score|0|0x0|
|-0x8|local_14 => round|0|0x0|
|0x0|saved EBP|   |   |
|+0x4|saved EIP|   |   |
|...|...|...|...|
|+0x18|argc|2 ou 3|   |
|+0x1c|argv|adr|   |

=>  adr: ptr vers argv[0] (/home/slb/Downloads/te1_motus24)
    adr+4: ptr vers argv[1] (-s ou ./dict_8-10.txt)
    adr+8: ptr vers argv[2] (secret ou rien)

## play

| adresse hexa  | contenu  | valeur typée  | mot hexa  |
|---|---|---|---|
|   |   |   |   |
|-0x3c|local_40 => guess|init after getWord()|   |
|-0x28|local_2c => secret|init after initSecret|   |
|-0x14|local_18 => wordFound|0 or 1 or undefined at the start|0x1 0x0 or undef|
|-0x10|local_14 => score|10 after init (decrement each wrong guess)|0xa|
|-0xc|local_10 => iteration / nbGuess|0 (incremented at start of loop)|0x0|
|0x0|saved EBP|   |   |
|+0x4|saved EIP|   |   |
|+0x8|param_1 => round|1 (for first exec)|0x1|
|...|...|...|...|

![trouver argc et argv](image.png)

## match

Si le mot est **pas** le bon au retour de la fonction (ligne 52 / 0x08049612)

| addr hexa    | contenu                       | valeur typée                   | mot hexa    |
| ------------ | ----------------------------- | ------------------------------ | ----------- |
| -074         | uvar2 = is_match              | 0                              | 0x0         |
| -06c  - 0x20 | aiStack_70 = buffer_result    | 1 / 0 / 2 selon règle motus    | 0x1 / 2 / 0 |
| -01c         | local_20 = secret_word_lenght | 8    (valeur du mot)           | 0x8         |
| -018         | local_1c = user_guess_lenght  | 8 (valeur du mot)              | 0x8         |
| -014         | local_18 = has_same_lenght    | 0                              | 0x0         |
| -010         | local_14 = j                  | 8   (val mot)                  | 0x8         |
| -00c         | local_10 = i                  | 8 (val mot)                    | 0x8         |
| 0xffffcb28   | EBP                           | -                              | -           |
| +008         | param1 = abaisser             | (mauvais mot existant ds dico) | 0x0         |
| +00c         | param2 = degouter             | (secret_word)                  | 0x0         |
|              |                               |                                |             |
