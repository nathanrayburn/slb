# Préparation du TE1, de SLB

### Structure du Writeup

- [Préparation du TE1, de SLB](#préparation-du-te1-de-slb)
    - [Structure du Writeup](#structure-du-writeup)
    - [Introduction](#introduction)
    - [Symbol Table](#symbol-table)
    - [Description détaillée des fonctions](#description-détaillée-des-fonctions)
      - [Fonction `main`](#fonction-main)
    - [Procédé pour lancer le jeu et but général](#procédé-pour-lancer-le-jeu-et-but-général)
    - [But général du jeu (Motus)](#but-général-du-jeu-motus)
    - [Règles de score](#règles-de-score)
    - [Objectif final](#objectif-final)
      - [Fonction `askChoice`](#fonction-askchoice)
      - [Fonction `play`](#fonction-play)
    - [Vulnérabilités potentielles](#vulnérabilités-potentielles)
  - [Déconstruction de la fonction `getWord`](#déconstruction-de-la-fonction-getword)
    - [Table des points d'arrêt (Breakpoints) dans GDB](#table-des-points-darrêt-breakpoints-dans-gdb)
    - [Commandes GDB](#commandes-gdb)
    - [Table des offsets des variables](#table-des-offsets-des-variables)

fichier originaux :

main.c

```c
undefined4 main(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  FILE *__stream;
  int iVar2;
  int local_18;
  int local_14;
  
  local_14 = 0;
  local_18 = 0;
  if ((param_1 < 2) || (3 < param_1)) {
    printf("Usage: %s path_2_dico\n",*param_2);
    uVar1 = 0xffffffff;
  }
  else {
    if (param_1 == 2) {
      strncpy(dico,(char *)param_2[1],0x100);
      __stream = fopen(dico,"r");
      if (__stream == (FILE *)0x0) {
        printf("Cannot find dico at %s\n",dico);
        return 0xffffffff;
      }
      fclose(__stream);
    }
    else {
      if ((*(char *)param_2[1] != '-') || (*(char *)(param_2[1] + 1) != 's')) {
        printf("Unexpected value passed in argument: %s\n",param_2[1]);
        return 0xffffffff;
      }
      strncpy(dico,(char *)param_2[2],0x14);
      nodico = 1;
    }
    while (iVar2 = askChoice(), iVar2 != 3) {
      if (iVar2 < 4) {
        if (iVar2 == 1) {
          local_14 = local_14 + 1;
          iVar2 = play(local_14);
          local_18 = local_18 + iVar2;
          printf("[i] --> score total = %d\n",local_18);
        }
        else {
          if (iVar2 != 2) goto LAB_08049988;
          local_14 = 0;
          local_18 = 0;
          printf("[i] --> score total = %d\n",0);
        }
      }
      else {
LAB_08049988:
        fprintf(stderr,"ERREUR : option %d non disponible\n\n",iVar2);
      }
    }
    puts(&DAT_0804a18f);
    uVar1 = 0;
  }
  return uVar1;
}
```

askChoice.c

```c

int askChoice(void)

{
  char cVar1;
  int iVar2;
  int local_10;
  
  local_10 = 0;
  puts("\n--------------------");
  while ((local_10 < 1 || (3 < local_10))) {
    puts("[m] Select");
    puts("[1] play");
    puts("[2] reset");
    puts("[3] stop");
    printf("\n[?] Your choice: ");
    iVar2 = getchar();
    cVar1 = (char)iVar2;
    if ((cVar1 < '1') || ('3' < cVar1)) {
      local_10 = -1;
    }
    else {
      local_10 = cVar1 + -0x30;
    }
    if (local_10 < 0) {
      puts("\n--------------------");
    }
    flushInput();
  }
  return local_10;
}
```

play.c

```c

int play(undefined4 param_1)

{
  size_t sVar1;
  int iVar2;
  char local_40 [20];
  char local_2c [20];
  int local_18;
  int local_14;
  int local_10;
  
  local_10 = 0;
  local_14 = 10;
  initSecret(local_2c);
  sVar1 = strlen(local_2c);
  printf("[i] secret word is %d chars long\n",sVar1);
  do {
    local_10 = local_10 + 1;
    printf("[i] round %d, iteration %d\n",param_1,local_10);
    getWord(local_40);
    printf("[i] comparaison : ");
    puts(local_40);
    iVar2 = exist(local_40);
    if (iVar2 == 0) {
LAB_080496d5:
      local_18 = 0;
    }
    else {
      iVar2 = match(local_40,local_2c);
      if (iVar2 == 0) goto LAB_080496d5;
      local_18 = 1;
    }
    local_14 = local_14 - (uint)(local_18 == 0);
    if ((9 < local_10) || (local_18 != 0)) {
      printf("[i] --> score du jeu = %d\n",local_14);
      return local_14;
    }
  } while( true );
}
```

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
    FILE *filePointer;       // filePointer avant modification - Pointeur pour manipuler des fichiers
    int choice;              // choice avant modification - Stocke le choix de l'utilisateur (renvoyé par askChoice)
    int totalScore;          // totalScore avant modification - Variable pour stocker le score total accumulé
    int roundNumber;         // roundNumber avant modification - Stocke le nombre de tours joués
    
    // Initialisation des variables
    roundNumber = 0;         // Le nombre de tours commence à zéro
    totalScore = 0;          // Le score total commence à zéro
    
    // Vérification du nombre d'arguments en ligne de commande
    if ((argc < 2) || (3 < argc)) {  
        // Si le nombre d'arguments est inférieur à 2 ou supérieur à 3, afficher l'utilisation correcte
        printf("Usage: %s path_2_dico\n", *(undefined4 *)argv);
        returnCode = 0xffffffff;  // Retourne -1 pour indiquer une erreur
    }
    else {
        // Si argc est égal à 2 (le programme est exécuté avec un seul argument supplémentaire)
        if (argc == 2) {
            // Copier l'argument (chemin du fichier dictionnaire) dans la variable `dico`
            strncpy(dico, *(char **)(argv + 4), 0x100);
            
            // Tente d'ouvrir le fichier dictionnaire
            filePointer = fopen(dico, "r");
            if (filePointer == (FILE *)0x0) {
                // Si le fichier ne peut pas être ouvert, afficher un message d'erreur
                printf("Cannot find dico at %s\n", dico);
                return 0xffffffff;  // Retourner -1 pour indiquer une erreur
            }
            fclose(filePointer);  // Fermer le fichier une fois ouvert avec succès
        }
        else {
            // Si argc est égal à 3, vérifier que le deuxième argument est bien "-s" pour avoir une entrée qui ressemble à "./program -s ./dico"
            if ((**(char **)(argv + 4) != '-') || (*(char *)(*(int *)(argv + 4) + 1) != 's')) {
                // Si l'argument ne commence pas par "-s", afficher un message d'erreur
                printf("Unexpected value passed in argument: %s\n", *(undefined4 *)(argv + 4));
                return 0xffffffff;  // Retourner -1 pour indiquer une erreur
            }
            // Copier le troisième argument dans `dico` avec une longueur maximale de 20 caractères
            strncpy(dico, *(char **)(argv + 8), 0x14);
            nodico = 1;  // Indiquer qu'aucun fichier dictionnaire n'est utilisé (flag `nodico` à 1)
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

- **Gestion des arguments** : Si le programme reçoit moins de deux ou plus de trois arguments, il affiche un message d'erreur et se termine. Si deux arguments sont fournis, il s'attend à un chemin vers un fichier dictionnaire. Si trois arguments sont fournis, le deuxième argument doit être le flag `-s`, et le troisième argument est interprété comme un autre fichier ou valeur.
  
- **Boucle de jeu** : Le programme entre dans une boucle dans laquelle il demande à l'utilisateur de faire un choix entre trois options : jouer, réinitialiser le score ou quitter.

### Procédé pour lancer le jeu et but général

Le programme est un jeu de type **Motus**, où le joueur doit deviner un mot secret. Le jeu peut être lancé avec deux ou trois arguments en ligne de commande, et le comportement change en fonction de l'usage du flag `-s` :

- **Lancer avec un fichier dictionnaire** :

  ```bash
  ./programme ./chemin_vers_dictionnaire.txt
  ```

  Dans ce cas, le programme utilise le fichier dictionnaire passé en argument pour choisir un mot secret aléatoire. Le dictionnaire peut contenir une liste de mots de n'importe quelle taille.

- **Lancer avec le flag `-s`** :

  ```bash
  ./programme -s ./chemin_vers_fichier.txt
  ```

  Avec ce flag, le programme cherche dans le fichier passé un mot dont la longueur est inférieure à 20 caractères. Il sélectionne le **troisième mot** du fichier qui respecte cette contrainte. Ce mode permet d'utiliser des fichiers plus génériques ou des fichiers contenant des mots de différentes tailles, mais seul un mot de moins de 20 caractères sera utilisé pour le jeu.

---

### But général du jeu (Motus)

Le but du jeu est de **deviner un mot secret** choisi à partir du fichier fourni (dictionnaire ou fichier avec le flag `-s`). À chaque tour, l'utilisateur entre une proposition de mot, et le programme compare ce mot avec le mot secret. L'interface du jeu fournit des **indices visuels** pour aider le joueur à affiner ses essais.

1. **Mode Jouer** (choix 1 dans le menu) :
   - Le joueur entre un mot, par exemple, **"vehicule"**.
   - Le programme affiche une **comparaison visuelle** sous forme d'une chaîne de caractères :
     - Les lettres correctes à la bonne position sont représentées par un **point d'exclamation (!)**.
     - Les lettres qui sont dans le mot secret mais à une mauvaise position sont représentées par un **point (. )**.
     - Les lettres absentes du mot secret ne sont pas indiquées.

   Exemple :

   ```bash
   [?] proposition : vehicule
   [i] comparaison : vehicule
   [i] ------------> .e...!.!
   [i] round 1, iteration 6
   ```

   Ici, la lettre "e" est dans le mot secret mais à la mauvaise position, et les lettres "c" et "l" sont à la bonne position.

   - À chaque tentative, l'utilisateur reçoit ces indices visuels, qui l'aident à affiner ses propositions dans les tours suivants.
   - Si le joueur devine correctement le mot, un message de réussite est affiché, et le score est calculé en fonction du nombre d'essais.

2. **Réinitialiser le score** (choix 2 dans le menu) :
   - Le score total et le nombre de tours sont remis à zéro, ce qui permet au joueur de recommencer une partie.

3. **Quitter** (choix 3 dans le menu) :
   - Le jeu se termine, et un message de fin est affiché.

### Règles de score

- Le score initial est fixé à 10 points, et il diminue à chaque tentative incorrecte.
- Le joueur peut faire plusieurs essais au cours d'un même round, et le score total est mis à jour après chaque itération réussie.

### Objectif final

Le joueur doit **deviner le mot secret** en utilisant les indices visuels fournis à chaque tour. Le jeu se poursuit jusqu'à ce que le mot soit trouvé ou que le joueur décide de quitter.

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
