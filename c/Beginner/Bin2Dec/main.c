#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

int powt(int base, int pow) {
    if(pow == 0)
        return 1;
    if(pow == 1)
        return base;
    return base * powt(base, --pow);
}

int main(void) {
    setlocale(LC_ALL, "");
    int result = 0;
    int op = 0;

    do {
        char choice[2];
        printf(" Selecione uma opcao\n 1 - 8\n 2 - 16\n 3 - 32\n 4 - 64\n> ");
        scanf("%s", choice);
        op = choice[0] - '0';
        if(op < 1 || op > 4) {
            fprintf(stderr, "[Erro]: O numero digitado deve ser de 1 ate 4\n");
            continue;
        }
        break;
    } while(1);

    uint8_t choose;

    switch (op)
    {
    case 1: choose = 8; break;
    case 2: choose = 16; break;
    case 3: choose = 32; break;
    case 4: choose = 64; break;
    }

    char* string = malloc(choose + 1);

    int pos = 0;
    do {
        printf(" Escreva um numero sem espacos com %d numeros\n> ", choose);
        scanf("%s", string);
        if(strlen(string) < 8) {
            fprintf(stderr, "O tamanho do texto digitado deve ser de %d bits\n", choose);
            pos = 0;
            continue;
        }

        if(string[pos] == ' ' || string[pos] == '\n' || string[pos] == '\t' || string[pos] == '\r') {
            fprintf(stderr, "O texto digitado nao pode conter espacos, quebras de linha ou tabulacao\n");
            pos = 0;
            continue;
        }
        pos++;
    } while(pos < choose);
    
    char invert[choose + 1];

    for(uint8_t i = 0; i < choose; i++)
        invert[i] = string[choose - i - 1];

    invert[choose] = '\0';
    printf("%s\n", invert);

    for(int i = 0; i < choose; i++) {
        if(invert[i] < '0' || invert[i] > '1')
            fprintf(stderr, "O caractere digitado na posicao %d deve ser 0 ou 1\n", i);
        result += (invert[i] - '0') * powt(2, i);
        printf("%c na pos %d\n", invert[i], i);
    }

    printf("%d", result);
    free(string);

    return 0;
}
