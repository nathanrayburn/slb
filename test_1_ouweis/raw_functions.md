## raw functions

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

initSecret.c

```c
void initSecret(char *param_1)

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
          iVar3 = __isoc99_fscanf(__stream,&DAT_0804a00a,param_1);
          if (iVar3 == -1) break;
          local_10 = local_10 + 1;
        } while (local_10 < iVar2 % iVar1 + 1);
        fclose(__stream);
      }
    }
  }
  else {
    strcpy(param_1,dico);
  }
  return;
}
```

flushInput.c

```c
void flushInput(void)

{
  int inputChar;
  
  do {
    inputChar = getchar();
    if ((char)inputChar == '\n') {
      return;
    }
  } while ((char)inputChar != -1);
  return;
}
```

getWord.c

```c
void getWord(undefined4 param_1)

{
  printf("\n[?] proposition : ");
  __isoc99_scanf(&DAT_0804a00a,param_1);
  flushInput();
  return;
}
```

countW.c

```c

int countW(void)

{
  FILE *__stream;
  int iVar1;
  int local_10;
  
  local_10 = 0;
  __stream = fopen(dico,"r");
  if (__stream != (FILE *)0x0) {
    while (iVar1 = feof(__stream), iVar1 == 0) {
      iVar1 = fgetc(__stream);
      local_10 = local_10 + (uint)((char)iVar1 == '\n');
    }
    fclose(__stream);
  }
  return local_10;
}
```

match.c

```c
undefined4 match(char *param_1,char *param_2)

{
  int iVar1;
  undefined4 uVar2;
  int aiStack_70 [20];
  size_t local_20;
  size_t local_1c;
  uint local_18;
  int local_14;
  int local_10;
  
  local_1c = strlen(param_1);
  local_20 = strlen(param_2);
  local_18 = (uint)(local_20 == local_1c);
  for (local_10 = 0; local_10 < 0x14; local_10 = local_10 + 1) {
    aiStack_70[local_10] = 0;
  }
  for (local_10 = 0; local_10 < (int)local_20; local_10 = local_10 + 1) {
    if (param_1[local_10] == param_2[local_10]) {
      aiStack_70[local_10] = 1;
    }
    else {
      local_18 = 0;
    }
  }
  if (local_18 == 0) {
    for (local_14 = 0; local_14 < (int)local_1c; local_14 = local_14 + 1) {
      for (local_10 = 0; local_10 < (int)local_20; local_10 = local_10 + 1) {
        if ((aiStack_70[local_14] == 0) && (param_1[local_14] == param_2[local_10])) {
          aiStack_70[local_14] = 2;
        }
      }
    }
    printf("[i] ------------> ");
    for (local_14 = 0; local_14 < (int)local_1c; local_14 = local_14 + 1) {
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


exist.c

```c
undefined4 exist(char *param_1)

{
  undefined4 uVar1;
  int iVar2;
  char local_24 [20];
  FILE *local_10;
  
  if (nodico == 0) {
    local_10 = fopen(dico,"r");
    if (local_10 == (FILE *)0x0) {
LAB_08049472:
      puts("[i] ------------> word does not exist!");
      uVar1 = 0;
    }
    else {
      do {
        iVar2 = __isoc99_fscanf(local_10,&DAT_0804a00a,local_24);
        if (iVar2 == -1) {
          fclose(local_10);
          goto LAB_08049472;
        }
        iVar2 = strcmp(param_1,local_24);
      } while (iVar2 != 0);
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
