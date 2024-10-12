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

```c
undefined4 main(int argc, undefined4 *argv)
{
  undefined4 uVar1;
  FILE *filePointer;
  int askChoiceResult;
  int temp_2;
  int temp_1;
  
  temp_1 = 0;
  temp_2 = 0;
                    /* If we execute the program with no arguments or more than 3, the program
                       prints Usage: program.sh path_2_dico
                        */
  if ((argc < 2) || (3 < argc)) {
    printf("Usage: %s path_2_dico\n", *argv);
    uVar1 = 0xffffffff;
  }
  else {
                    /* We enter this else condition :  If argc = 2 or argc = 3 */
    if (argc == 2) {
      strncpy(dico, (char *)argv[1], 0x100);
      filePointer = fopen(dico, "r");
      if (filePointer == (FILE *)0x0) {
        printf("Cannot find dico at %s\n", dico);
        return 0xffffffff;
      }
      fclose(filePointer);
    }
    else {
                    /* If argc = 3 
                       arg parse check. checks for -s
                       ./exec -s value
                       argv[0] = te1_motus24
                       argv[1] =  -s
                       argv[2] =  value ( maybe file path ? ) */
      if ((*(char *)argv[1] != '-') || (*(char *)(argv[1] + 1) != 's')) {
        printf("Unexpected value passed in argument: %s\n", argv[1]);
        return 0xffffffff;
      }
                    /* dest = This is the pointer to the destination array where to copy the content
                       src  = String to be copied
                       n    = The number of characters to be copied from the source
                       char * strncpy(char *dest, const char *src, size_t n) 
                        */
      strncpy(dico, (char *)argv[2], 0x14);
      nodico = 1;
    }
    while (askChoiceResult = askChoice(), askChoiceResult != 3) {
      if (askChoiceResult < 4) {
        if (askChoiceResult == 1) {
          temp_1 = temp_1 + 1;
          askChoiceResult = play(temp_1);
          temp_2 = temp_2 + askChoiceResult;
          printf("[i] --> score total = %d\n", temp_2);
        }
        else {
          if (askChoiceResult != 2) goto LAB_08049988;
          temp_1 = 0;
          temp_2 = 0;
          printf("[i] --> score total = %d\n", 0);
        }
      }
      else {
LAB_08049988:
        fprintf(stderr, "ERREUR : option %d non disponible\n\n", askChoiceResult);
      }
    }
    puts(&DAT_0804a18f);
    uVar1 = 0;
  }
  return uVar1;
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
  char userInputInChar;
  int rawUserInput;
  int gameOption;
  
  gameOption = 0;
  puts("\n--------------------");
                    /* Loop until chosen a valid option. */
  while ((gameOption < 1 || (3 < gameOption))) {
    puts("[m] Select");
    puts("[1] play");
    puts("[2] reset");
    puts("[3] stop");
    printf("\n[?] Your choice: ");
    rawUserInput = getchar();
    userInputInChar = (char)rawUserInput;
    if ((userInputInChar < '1') || ('3' < userInputInChar)) {
      gameOption = -1;
    }
    else {
                    /* Convert the ASCII "1","2","3" into int value 1,2,3 */
      gameOption = userInputInChar + -0x30;
    }
    if (gameOption < 0) {
      puts("\n--------------------");
    }
    flushInput();
  }
  return gameOption;
}
```

- **Variables**:
  - `userInputInChar`: Stores the character input by the user.
  - `rawUserInput`: Holds the raw value returned by `getchar()`.
  - `gameOption`: Stores the selected game option after validation.
- **Flow**: The function loops until the user inputs a valid option (`1`, `2`, or `3`). The value is validated and converted from ASCII to integer for further processing.

## Play Function Deconstruction

The `play` function is responsible for executing the main game logic. To better understand the function, we will track the stack usage and variable assignments throughout its execution.

### Play Function Stack Tracking Table

| Instruction Address | Stack Address | Comments                     |
|---------------------|---------------|------------------------------|
| 0x08049630          | EBP + -0x28   | secret buffer string         |
| 0x08049679          | EBP + -0x3c   | user guess buffer            |

### Play Function Code Deconstruction

```c
int play(undefined4 param_1)
{
  bool doesWordExist;
  size_t secretWordLength;
  undefined3 extraout_var;
  int doesWordsMatch;
  char playerGuessBuffer [20];
  char secretBuffer [20];
  bool flag;
  int score;
  int currentGuessCount;
  
  currentGuessCount = 0;
  score = 10;
  initSecret(secretBuffer);
  secretWordLength = strlen(secretBuffer);
  printf("[i] secret word is %d chars long\n", secretWordLength);
  do {
    currentGuessCount = currentGuessCount + 1;
    printf("[i] round %d, iteration %d\n", param_1, currentGuessCount);
    getWord(playerGuessBuffer);
    printf("[i] comparaison : ");
    puts(playerGuessBuffer);
    doesWordExist = exist(playerGuessBuffer);
    if (CONCAT31(extraout_var, doesWordExist) == 0) {
LAB_080496d5:
      flag = 0;
    }
    else {
      doesWordsMatch = match(playerGuessBuffer, secretBuffer);
      if (doesWordsMatch == 0) goto LAB_080496d5;
      flag = 1;
    }
    score = score - (uint)(flag == 0);
    if ((9 < currentGuessCount) || (flag != 0)) {
      printf("[i] --> score du jeu = %d\n", score);
      return score;
    }
  } while( true );
}
```

- **Variables**:
  - `playerGuessBuffer`: Stores the player's guess.
  - `secretBuffer`: Stores the secret word that the player needs to guess.
  - `score`: The player's current score, decreasing with incorrect guesses.
  - `currentGuessCount`: Keeps track of the number of guesses made.
- **Flow**: The function initializes the secret word, then enters a loop where the player makes guesses until either the correct word is guessed or the number of guesses exceeds 9.

## GetWord Function Deconstruction

The `getWord` function is used to get the player's word guess. This function likely handles input from the user and stores it for further processing.

### Debug Breakpoints Table

| Breakpoint Address | Comments                                   |
|--------------------|-------------------------------------------|
| 0x080493ac         | Before calling the `scanf` in `getWord()` |
| 0x080496c2         | Instruction on call for `match` function (`doesWordsMatch = match(playerGuessBuffer,secretBuffer);`) |
| 0x080494a6         | `secretLength = strlen(secretBuffer);`    |

### GDB Commands

- **Read the secret password from the stack**: `x/s $ebp-0x28`
- **Overwrite the buffer into the stack for the user guess word**: `set {char[20]} $ebp-0x3c = "YOUR_NEW_GUESS"`
