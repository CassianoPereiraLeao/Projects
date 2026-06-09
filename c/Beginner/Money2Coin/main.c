#include <stdio.h>

#define COINS_LEN 6

const int coins[COINS_LEN] = { 100, 50, 25, 10, 5, 1 };
const char* types[COINS_LEN] = { "1 real", "50 centavos", "25 centavos", "10 centavos", "5 centavos", "1 centavo" };

int main(void) {
    float money;
    printf("Qual valor voce deseja transformar?\n> ");
    scanf("%f", &money);

    int money_integer = money * 100;

    for(int i = 0; i < COINS_LEN; ++i) {
        printf("%d de %s\n", money_integer / coins[i], types[i]);
        money_integer %= coins[i];
    }
}
