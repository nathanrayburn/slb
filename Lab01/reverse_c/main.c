#include <stdint.h>
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

        printf("%ld\n",ftell(fp));

        res = (int)(char)readCharFromFile ^ 0xef;
        if ((res & 2) == 0) {
            fputc(res, fp);
            continue;
        }
        res = (int)(char)readCharFromFile ^ 0xbe;
        fputc(res, fp);
    }
}

void encrypt1(FILE *fp) {
    int byteFromFile;
    int var;
    char res;

    var = 7;
    while(true) {
        byteFromFile = fgetc(fp);
        if (byteFromFile == EOF) break;
        fseek(fp, -1, SEEK_CUR);
        res = ((int)byteFromFile << 2 | (int)byteFromFile >> 6) + var;
        var = res >> 2 | res * '@';
        fputc(var,fp);
    }
}

void decrypt1(FILE *fp) {
    int byteFromFile;
    const uint8_t mult_val = 64;
    const uint8_t shift = 2;
    const uint8_t right_shift = 6;

    uint8_t const_value;
    uint8_t original_byte;

    const_value = 7;

    while((byteFromFile = fgetc(fp) != EOF)) {
        uint8_t temp = byteFromFile;
        original_byte = (temp - const_value) << shift | (temp - const_value) >> right_shift;
        fputc(original_byte, fp);
        const_value = temp >> shift | temp * mult_val;
    }

}

int main(void) {
    /*
    FILE* fp = fopen("/home/nathan/Documents/git/slb/Lab01/reverse_c/confidential.txt", "rb+");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    decrypt0(fp);
    fclose(fp);
    */

    FILE* fp1 = fopen("/home/nathan/Documents/git/slb/Lab01/reverse_c/passwords.txt", "rb+");
    if (fp1 == NULL) {
        perror("Failed to open file");
        return 1;
    }
    decrypt1(fp1);
    fclose(fp1);
    /*
    FILE* f2 = fopen("/home/nathan/Documents/git/slb/Lab01/reverse_c/salary.txt", "rb+");
    if (f2 == NULL) {
        perror("Failed to open file");
        return 1;
    }
    decrypt1(f2);
    fclose(f2); */

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