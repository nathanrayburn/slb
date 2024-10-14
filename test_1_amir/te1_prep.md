# utilisation
two modes:
./te1_motus24 -s <mot à deviner>

./te1_motus24 <filepath du dictionnaire>
=> ./te1_motus24 ./dict_8-10.txt

# code

## main

Initialisation
```c
/*originally in param1, undefined * param2*/
undefined4 main(int argc,char *argv[])

{
  undefined4 uVar1; // valeur de retour de main 
  FILE *__stream;
  int iVar2; // user input (choice 1, 2 or 3 -> play,reset,quit)
  int local_18;
  int local_14;
  
  local_14 = 0; // => round
  local_18 = 0; // => score

```

validation arguments
```c
    /* argc soit 2 soit 3 */
  if ((argc < 2) || (3 < argc)) {
    printf("Usage: %s path_2_dico\n",*(undefined4 *)argv[]);
    uVar1 = 0xffffffff;
  }
  else {
    if (argc == 2) { /* argc 2 -> path to dico*/
      strncpy(dico,*(char **)(argv[] + 4),0x100);
      __stream = fopen(dico,"r");
      if (__stream == (FILE *)0x0) {
        printf("Cannot find dico at %s\n",dico);
        return 0xffffffff;
      }
      fclose(__stream);
    }
    else { /* argc 3 -> -s motàdeviner */
      if ((**(char **)(argv[] + 4) != '-') || (*(char *)(*(int *)(argv[] +  4) + 1) != 's')) {
        printf("Unexpected value passed in argument: %s\n",*(und efined4 *)(argv[] + 4));
        return 0xffffffff;
      }
      strncpy(dico,*(char **)(argv[] + 8),0x14);
      nodico = 1;
    }

``` 

game loop
```c
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
        fprintf(stderr,"ERREUR : option %d non disponible\n\n",iVar 2);
      }
    }
    puts(&DAT_0804a18f);
    uVar1 = 0;
  }
  return uVar1;
}

```

## askChoice()

handles user input for the 1,2,3 choice

```c
int askChoice(void)

{
  char cVar1; //user input as char
  int iVar2; //user input
  int local_10; //return value
  
  local_10 = 0; //initialisation
  puts("\n--------------------");
  while ((local_10 < 1 || (3 < local_10))) {
    puts("[m] Select");
    puts("[1] play");
    puts("[2] reset");
    puts("[3] stop");
    printf("\n[?] Your choice: ");
    iVar2 = getchar();
    cVar1 = (char)iVar2;
    if ((cVar1 < '1') || ('3' < cVar1)) { // validation input
      local_10 = -1;
    }
    else {
      local_10 = cVar1 + -0x30; //convert the char into an int 
    }
    if (local_10 < 0) {
      puts("\n--------------------");
    }
    flushInput();
  }
  return local_10;
}
```

flush the input buffer

```c
void flushInput(void)

{
  int iVar1;
  
  do {
    iVar1 = getchar();
    if ((char)iVar1 == '\n') {
      return;
    }
  } while ((char)iVar1 != -1);
  return;
}


```

## play()

```c
int play(undefined4 param_1) //param1 = round

{
  size_t sVar1; //secret length
  int iVar2; //val for some func return (test val)
  char local_40 [20]; //guess
  char local_2c [20]; //secret
  int local_18; //wordFound
  int local_14; // score for this round
  int local_10; // iteration (nb of guess)
  
  local_10 = 0;
  local_14 = 10;
  initSecret(local_2c);
  sVar1 = strlen(local_2c);
  printf("[i] secret word is %d chars long\n",sVar1);
  do { //game loop
    local_10 = local_10 + 1;
    printf("[i] round %d, iteration %d\n",param_1,local_10);
    getWord(local_40); //user input (guess)
    printf("[i] comparaison : ");
    puts(local_40);
    iVar2 = exist(local_40); //test if word exists in dict (maybe patch here for guaranteed win)
    if (iVar2 == 0) { // patch
LAB_080496d5:      
      local_18 = 0;
    }
    else {
      iVar2 = match(local_40,local_2c); //check if guess matches secret
      if (iVar2 == 0) goto LAB_080496d5;
      local_18 = 1;
    }
    local_14 = local_14 - (uint)(local_18 == 0); //score for this round - 1 if word not found
    if ((9 < local_10) || (local_18 != 0)) { //if 10 guesses or word found, game over
      printf("[i] --> score du jeu = %d\n",local_14);
      return local_14;
    }
  } while( true );
}
```

```asssembly
            080496ab  e8 34 fd ff ff         CALL             exist                                                                              undefined exist(undefined4 guess)
            080496b0  83 c4 10              ADD              ESP,0x10
            080496b3  85 c0                   TEST             secretLen,secretLen
            080496b5  74 1e                   JZ                 LAB_080496d5 // Patch here JMP 0x080496fa ->if word is in dict, guaranteed win
            080496b7  83 ec 08              SUB              ESP,0x8
            080496ba  8d 45 d8              LEA               secretLen=>secret,[EBP + -0x28]

```

## exist()

!!! Utilisation de var globale nodico et dico !!!

If nodico == 0, teste si le mot existe dans le dict (retourne 1 si oui, 0 si non)
If nodico == 1, retourne 1

dico = filepath du dictionnaire

```c
undefined4 exist(char *param_1) //guess

{
  undefined4 uVar1; //ret
  int iVar2; // test value (return from fscanf and strcmp)
  char local_24 [20]; //word from dict
  FILE *local_10; //dict
  
  if (nodico == 0) {
    local_10 = fopen(dico,"r");
    if (local_10 == (FILE *)0x0) { //if can't open file => word does not exist
LAB_08049472:
      puts("[i] ------------> word does not exist!");
      uVar1 = 0;
    }
    else {
      do {
        iVar2 = __isoc99_fscanf(local_10,&DAT_0804a00a,local_24 ); //fetch a word from dict 
        if (iVar2 == -1) { //if return from fscanf is -1, EOF => word does not exist
          fclose(local_10);
          goto LAB_08049472;
        }
        iVar2 = strcmp(param_1,local_24); //compare guess and word from dict
      } while (iVar2 != 0); //if match, stop
      fclose(local_10);
      uVar1 = 1;
    }
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

```

## match()

```c

undefined4 match(char *param_1,char *param_2) // guess, secret

{
  int iVar1;
  undefined4 uVar2;
  int aiStack_70 [20];
  size_t local_20; //secretLen
  size_t local_1c; //guessLen
  uint local_18; //is guess correct
  int local_14;
  int local_10; //iterator
  
  local_1c = strlen(param_1); //guessLen
  local_20 = strlen(param_2); //secretLen
  local_18 = (uint)(local_20 == local_1c); //does len match
  for (local_10 = 0; local_10 < 0x14; local_10 = local_10 + 1) {//init all to 0
    aiStack_70[local_10] = 0; 
  }
  for (local_10 = 0; local_10 < (int)local_20; local_10 = local_10 + 1) { //for i=0 to secretLen
    if (param_1[local_10] == param_2[local_10]) { //if a letter matches, set corresponding index to 1
      aiStack_70[local_10] = 1;
    }
    else { //if a letter doesn't match => guess not correct
      local_18 = 0; 
    }
  }
  if (local_18 == 0) { //if guess not correct, check if there are correct letters but at the wrong spot in the guess
    for (local_14 = 0; local_14 < (int)local_1c; local_14 = local_14  + 1) {
      for (local_10 = 0; local_10 < (int)local_20; local_10 = local_ 10 + 1) {
        if ((aiStack_70[local_14] == 0) && (param_1[local_14] == param_2[local_10])) {
          aiStack_70[local_14] = 2;
        }
      }
    }
    printf("[i] ------------> "); //print letter if it's correctly placed, '.' if not present in word, '!' if present but wrong place
    for (local_14 = 0; local_14 < (int)local_1c; local_14 = local_14  + 1) {
      iVar1 = aiStack_70[local_14];
      if (iVar1 == 2) {
        putchar(0x21);
      }
      else if (iVar1 < 3) {
        if (iVar1 == 0) {
          putchar(0x2e);
        }
        else if (iVar1 == 1) {
          putchar((int)param_1[local_14]);
        }
      }
    }
    putchar(10);
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}
```

## initSecret()

!!! Utilisation de var globale nodico et dico !!!

If nodico == 0, teste si le mot existe dans le dict (retourne 1 si oui, 0 si non)
If nodico == 1, retourne 1

dico = filepath du dictionnaire

```c
void initSecret(char *param_1) // secret (output)

{
  int iVar1;
  int iVar2;
  FILE *__stream;
  int iVar3;
  int local_10;
  
  local_10 = 0;
  if (nodico == 0) {
    *param_1 = '\0';
    iVar1 = countW();
    if (iVar1 != 0) {
      iVar2 = rand();
      __stream = fopen(dico,"r");
      if (__stream != (FILE *)0x0) {
        do {
          iVar3 = __isoc99_fscanf(__stream,&DAT_0804a00a,param _1);
          if (iVar3 == -1) break;
          local_10 = local_10 + 1;
        } while (local_10 < iVar2 % iVar1 + 1);
        fclose(__stream);
      }
    }
  }
  else {
    strcpy(param_1,dico); //if no dict, grab the -s param
  }
  return;
}
```

patch after fscanf call

```
            08049361  e8 fa fc ff ff          CALL             <EXTERNAL>::__isoc99_fscanf                                       undefined __isoc99_fscanf()
            08049366  83 c4 10              ADD              ESP,0x10
            08049369  83 f8 ff                 CMP              randVal,-0x1
             0804936c  74 0c                   JZ                 LAB_0804937a

```

change JZ LAB_0804937a into JMP 0x08049388 (just before return) => will always pick first word of dict.

## countW()

!!! Utilisation de var globale dico !!!

dico = filepath du dictionnaire

```c
int countW(void)

{
  FILE *__stream;
  int iVar1;
  int local_10; // wordCount
  
  local_10 = 0;
  __stream = fopen(dico,"r");
  if (__stream != (FILE *)0x0) {
    while (iVar1 = feof(__stream), iVar1 == 0) { //while not EOF
      iVar1 = fgetc(__stream);
      local_10 = local_10 + (uint)((char)iVar1 == '\n'); //increment every endline char (count words in the dict)
    }
    fclose(__stream);
  }
  return local_10;
}

```



# patch

in play() for guaranteed win

in initSecret() to make secret non random

# analyse dynamique

gdb avec breakpoint après l'appel de initSecret
1. b play
2. r ./dict_8-10.txt
3. when prompted, chose 1 to play
4. disass
5. b *0x08049639
6. c
7. x/s $ebp-0x2c

# stack

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

## Unused function

```c
void eraseWord(int param_1) //pointeur vers un string

{
  int local_8; //iter
  
  for (local_8 = 0; local_8 < 0x14; local_8 = local_8 + 1) { //loop 20 fois (longueur max des mots et guess dans le programme)
    *(undefined *)(param_1 + local_8) = 0x2e; //set tout à '.'
  }
  return;
}
```