#include <stdio.h>
#include <sys/types.h>

void encrypt0(FILE* fp) {
    uint readCharFromFile;
    char res;
    while(true) {
        readCharFromFile = fgetc(fp);
        if(readCharFromFile == 0xffffffff) break;
        fseek(fp, -1, 1);
        if((readCharFromFile & 2) == 0) {
            res = (int)(char)readCharFromFile ^ 0xef;
        }else {
            res = (int)(char)readCharFromFile ^ 0xeb;
        }
        fputc(res,fp);
    }

}

void decrypt0(FILE* fp) {
    uint readCharFromFile;
    char res;
    while(true) {
        readCharFromFile = fgetc(fp);

        if(readCharFromFile == 0xffffffff) break;

        fseek(fp, -1, 1);

        res = (int)(char)readCharFromFile ^ 0xef;

        if((res & 2) == 0){
            fputc(res, fp);
            continue;
        }

        res = (int)(char)readCharFromFile ^ 0xbe;
        fputc(res,fp);
    }
}
int main(void) {

    FILE* fp = fopen("salary.txt", "rb+");

    decrypt0(fp);

    fp = fopen("password.txt", "rb+");

    fp = fopen("confdiential.txt", "wb+");

    return 0;
}
