#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    const char* name;
    uint16_t year;
    const char* genre;
    uint8_t note;
} Game;

const Game movies[] = {
    { "Control", 2019, "Action", 82 },
    { "Stardew Valley", 2016, "Indie", 89 },
    { "Graveyard Keeper", 2018, "Indie", 90 },
    { "Call of Duty: Black Ops", 2010, "Action", 90 },
};

int main(void) {
    while(1) {
        uint8_t i = 0;
        for(i = 0; i < sizeof(movies)/sizeof(movies[0]); ++i) 
            printf("%d. %s\n", i + 1, movies[i].name);
        i++;
        printf("%d. Exit\n", i);

        printf("> ");
        char choice = '0';
        scanf(" %c", &choice);

        if(!isdigit(choice)) {
            fprintf(stderr, "Erro, por favor digite um valor entre 1 e %d\n", i);
            continue;
        }

        int op = choice - '0';
        if(op > 0 && op < i) {
            op--;
            printf("Nome: %s\nAno: %d\nGenero: %s\nNota: %.1lf/5.0\n", movies[op].name,
                movies[op].year, movies[op].genre, movies[op].note / 20.0f);
        } else if(op == i)
            break;
        else
            fprintf(stderr, "Erro, por favor digite um valor entre 1 e %d\n", i);
    }
}
