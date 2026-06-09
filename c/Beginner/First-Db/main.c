#include "db.h"

int main(void) {

    while(1) {
        puts("\nEscolha uma das opcoes abaixo:");
        puts("1. Limpar banco de dados");
        puts("2. Carregar dados");
        puts("3. Mostrar todos os dados");
        puts("4. Sair");
        printf("> ");

        int op = 0;
        scanf("%d", &op);

        switch (op)
        {
        case 1:
            clear_db();
            break;
        case 2:
            load_db();
            break;
        case 3:
            ShowData data = show_all_data();

            if(data.current_index == 0) {
                fprintf(stderr, "\nTodos os dados estao limpos.\n");
                break;
            }

            for(int i = 0; i < data.current_index; ++i) {
                printf("\nId: %d\n", i);
                printf("Nome: %s\n", data.data[i].name);
                printf("Email: %s\n", data.data[i].email);
            }

            break;
        case 4:
            printf("Encerrando o programa ...\n");
            return 0;
        default:
            printf("Erro: Por favor digite apenas opções entre 1 e 4\n");
            break;
        }
        
    }
}
