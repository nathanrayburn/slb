## Symbol Table

| Function Name | Location Address | Comments            |
|---------------|-----------------|---------------------|
| flushInput    | 0x08049256      |                     |
| countW        | 0x08049274      |                     |
| initSecret    | 0x080492e4      |                     |
| getWord       | 0x080493b8      |                     |
| eraseWord     | 0x080493bc      |                     |
| exist         | 0x080493e4      |                     |
| match         | 0x08049489      |                     |
| play          | 0x08049619      |                     |
| askChoice     | 0x08049712      |                     |
| main          | 0x080497dd      |                     |

## Play Function Call Table

| Function Called | Location Address | Comments            |
|-----------------|-----------------|---------------------|
| strlen          | N/A             | Standard library function for string length calculation |
| initSecret      | 0x080492e4      |                     |
| getWord         | 0x080493b8      |                     |
| exist           | 0x080493e4      |                     |
| match           | 0x08049489      |                     |

## AskChoice Function Call Table

| Function Called | Location Address | Comments            |
|-----------------|-----------------|---------------------|
| getchar         | N/A             | Standard library function for character input          |
| flushInput      | 0x08049256      |                     |

## Application Launch Instructions

The application can be executed with either 2 or 3 arguments:

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

- **Arguments**:
  - If **argc = 2**: The second argument is expected to be the path to a dictionary file (`path_2_dico`).
  - If **argc = 3**: The second argument must be `-s` and the third argument is expected to be a value, possibly a file path.
- **Error Handling**: If no arguments or more than three arguments are passed, the program prints usage instructions.
- **Loop**: The program enters a loop calling `askChoice()` until a value of `3` is returned.

## AskChoice Function Deconstruction

The `askChoice` function is used to gather input from the user to determine which action to take. It calls the following functions:

- **getchar**: This standard library function is used for reading a character from standard input.
- **flushInput**: This function clears any remaining characters in the input buffer to prevent unintended input handling.

The `askChoice` function plays a critical role in guiding the flow of the application, interacting with the user to decide between different options. It returns an integer value representing the user's choice, which is used to determine further actions in the program.

### AskChoice Function Code Deconstruction

The `askChoice` function is responsible for presenting the user with a menu of options and returning their choice. It ensures that only valid options (`1`, `2`, or `3`) are accepted and converts the character input into an integer value.

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

## Play Function Deconstruction

The `play` function is responsible for executing the main game logic. To better understand the function, we will track the stack usage and variable assignments throughout its execution.

### Play Function Stack Tracking Table

| Instruction Address | Stack Address | Comments                     |
|---------------------|---------------|------------------------------|
| 0x08049630          | EBP + -0x28   | secret buffer string         |
| 0x08049679          | EBP + -0x3c   | user guess buffer            |

### Play Function Code Deconstruction

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

## GetWord Function Deconstruction

The `getWord` function is used to get the player's word guess. This function likely handles input from the user and stores it for further processing.

### Debug Breakpoints Table

| Breakpoint Address | Comments                                   |
|--------------------|-------------------------------------------|
| 0x08049643         | instruction call of strlen, `secretWordLength = strlen(secretBuffer);`|

### GDB Commands

- **Read the secret password from the stack**: `x/s $ebp-0x28`
- **Overwrite the buffer into the stack for the user guess word**: `set {char[20]} $ebp-0x3c = "YOUR_NEW_GUESS"`


### Variable Offset Table

| Variable Name      | Offset       |
|--------------------|--------------|
| secretBuffer       | EBP - 0x28   |
| playerGuessBuffer  | EBP - 0x3c   |


# Stack Documentation at Match Function

## Match Function - Stack Layout

At the breakpoint set in the match function, the stack's current state is as follows:

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
| +004         | EIP                           | -                              | -           |
| +008         | param1 = abaisser             | (mauvais mot existant ds dico) | 0x0         |
| +00c         | param2 = degouter             | (secret_word)                  | 0x0         |
|              |                               |                                |             |



### Details

- **iVar1 (charState), uVar2 (isMatch), sVar1, and iVar2**: These variables are used directly with registers and are not stored in the stack.


# Stack Documentation at Play Function

## Play Function - Stack Layout

At the breakpoint set in the play function, the stack's current state is as follows:

| Comment        | Value                | Offset (based on EBP) | Address     | Conversion Name        |
|----------------|----------------------|-----------------------|-------------|------------------------|
| Local_40       | `0x333231 ('123')`   | `-03c`                | `0xffffcb4c` | `playerGuessBuffer`    |
| Local_2c       | `'degouter'`         | `-028`                | `0xffffcb60` | `secretWordBuffer`     |
| Local_18       | `1`                  | `-014`                | `0xffffcb74` | `isSecretFoundFlag`    |
| Local_14       | `0xa ('\n')`        | `-010`                | `0xffffcb78` | `maxGuesses`           |
| Local_10       | `1`                  | `-00c`                | `0xffffcb7c` | `guessCounter`         |

Stack d'amir

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


### Details

- **sVar1 and iVar2**: These variables are used directly with registers and are not stored in the stack.


## Main Function - Stack Layout
| adresse hexa  | contenu  | valeur typée  | mot hexa  |
|---|---|---|---|
|   |   |   |   |
|-0x10|local_18 => score|0|0x0|
|-0xc|local_14 => round|0|0x0|
|0x0|saved EBP|   |   |
|+0x4|saved EIP|   |   |
|...|...|...|...|
|+0x18|argc|2 ou 3|   |
|+0x1c|argv|adr|   |

=>  adr: ptr vers argv[0] (/home/slb/Downloads/te1_motus24)
    adr+4: ptr vers argv[1] (-s ou ./dict_8-10.txt)
    adr+8: ptr vers argv[2] (secret ou rien)


## Patches 

### First patch

First patch in play function, patch the JZ after test and jump to the flag = 1. Addr 0x080496ce

```assembly
        080496ab e8 34 fd        CALL       exist                                            undefined exist(undefined4 param
                 ff ff
        080496b0 83 c4 10        ADD        ESP,0x10
        080496b3 85 c0           TEST       EAX,EAX
        080496b5 74 1e           JZ         LAB_080496d5
```

### Second patch

Second patch in initSecret function, patch the JZ to jump to the NOP addr : 0x08049388


```assembly
        08049359 68 0a a0        PUSH       DAT_0804a00a                                     = 25h    %
                 04 08
        0804935e ff 75 e8        PUSH       dword ptr [EBP + local_1c]
        08049361 e8 fa fc        CALL       <EXTERNAL>::__isoc99_fscanf                      undefined __isoc99_fscanf()
                 ff ff
        08049366 83 c4 10        ADD        ESP,0x10
        08049369 83 f8 ff        CMP        EAX,-0x1
        0804936c 74 0c           JZ         LAB_0804937a
```


### Third Patch

You can Simply implement NOP operations and change the printf string to %s to print the secret password. 
This works by simply keeping the secret password in EAX which then printf will use it to print later in the console.


originally

```assembly
        08049634 e8 ab fc        CALL       initSecret                                       undefined initSecret(undefined4 
                 ff ff
        08049639 83 c4 10        ADD        ESP,0x10
        0804963c 83 ec 0c        SUB        ESP,0xc
        0804963f 8d 45 d8        LEA        EAX=>local_2c,[EBP + -0x28]
        08049642 50              PUSH       EAX
        08049643 e8 78 fa        CALL       <EXTERNAL>::strlen                               size_t strlen(char * __s)
                 ff ff
```

``àssembly
08049634 e8 ab fc        CALL       initSecret                                       undefined initSecret(undefined4 
                 ff ff
        08049639 83 c4 10        ADD        ESP,0x10
        0804963c 83 ec 0c        SUB        ESP,0xc
        0804963f 8d 45 d8        LEA        EAX=>secretWord,[EBP + -0x28]
        08049642 50              PUSH       EAX
        08049643 90              NOP
        08049644 90              NOP
        08049645 90              NOP
        08049646 90              NOP
        08049647 90              NOP
        08049648 90              NOP
        08049649 90              NOP
        0804964a 90              NOP
        0804964b 90              NOP
        0804964c 90              NOP
        0804964d 90              NOP
        0804964e 90              NOP
        0804964f 90              NOP
        08049650 90              NOP
        08049651 90              NOP
        08049652 90              NOP
        08049653 90              NOP
        08049654 e8 f7 f9        CALL       <EXTERNAL>::printf                               int printf(char * __format, ...)
                 ff ff
        08049659 83 c4 10        ADD        ESP,0x10
```
