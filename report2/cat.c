#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXLENGTH 256 

int main() 
{ 
    char cc; 
    char string[MAXLENGTH], *tmp; 
    FILE *file1;                                 /* ファイルポインタの宣言 */

    char argv[100];
    scanf("%s",argv);
    file1 = fopen(argv, "r");               /* ファイルのオープン */ 

    if(file1 == NULL){                           /* ファイルがオープンできたどうかの確認 */ 
        fprintf(stderr, "%s is not found.\n", argv); 
        exit(1); 
    } 

    cc = getc(file1);                            /* ファイルからの1文字読みだし */ 

    while((cc = getc(file1))!= EOF){                            /* ファイルの終りまで読み込むためのループ */
        printf("%c", cc);                   /* 文字列の出力 */ 
    } 

    fclose(file1);                                /* ファイルのクローズ */ 

    return(0);
}
