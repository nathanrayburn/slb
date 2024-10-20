#include <stdio.h>
#include <sys/types.h>

void encrypt0(FILE* fp) {
    int readCharFromFile;
    char res;
    while (true) {
        readCharFromFile = fgetc(fp);
        if (readCharFromFile == EOF) break;
        fseek(fp, -1, SEEK_CUR);
        if ((readCharFromFile & 2) == 0) {
            res = (char)readCharFromFile ^ 0xef;
        } else {
            res = (char)readCharFromFile ^ 0xeb;
        }
        fputc(res, fp);
    }
}

void decrypt0(FILE* fp) {
    uint readCharFromFile;
    char res;
    if (fp == NULL) return;
    rewind(fp);
    while (true) {
        readCharFromFile = fgetc(fp);
        if (readCharFromFile == EOF) {
            printf("End of file\n");
            break;
        }
        fseek(fp, -1, SEEK_CUR);
        res = (char)readCharFromFile ^ 0xef;
        if ((res & 2) == 0) {
            fputc(res, fp);
            continue;
        }
        res = (char)readCharFromFile ^ 0xbe;
        fputc(res, fp);
    }
}

int main(void) {
    FILE* fp = fopen("/home/nathan/Documents/git/slb/Lab01/reverse_c/salary.txt", "rb+");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }
    decrypt0(fp);
    fclose(fp);

    /*
    fp = fopen("passwords.txt", "rb");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }
    // Call encrypt0 or decrypt0 as needed
    fclose(fp);

    fp = fopen("confidential.txt", "wb+");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }
    // Call encrypt0 or decrypt0 as needed
    fclose(fp);
        */
    return 0;
}