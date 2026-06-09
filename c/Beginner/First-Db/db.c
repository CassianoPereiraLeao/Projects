#include "db.h"

static int current_id = 0;
static Users data[MAX_USERS] = { 0 };

static int valid_name(const char* name) {
    if(strlen(name) < 2) {
        fprintf(stderr, "O nome deve ter no minimo 2 caracteres validos\n");
        return 0;
    }
    for(int i = 0; name[i] != '\0'; ++i) {
        if(isdigit(name[i])) { 
            fprintf(stderr, "O nome nao pode conter numeros\n"); 
            return 0;
        }
    }

    return 1;
}

static int valid_email(const char* email) {
    int has_at_sign = 0;
    int has_dot = 0;
    
    for(int i = 0; email[i] != '\0'; ++i) {
        if(email[i] == '@') has_at_sign = 1;
        if(email[i] == '.' && has_at_sign) has_dot = 1;
    }

    if(!has_at_sign || !has_dot) {
        fprintf(stderr, "O email deve conter @ e .\n");
        return 0;
    }

    return 1;
}

static int add_data(const char* name, const char* email) {
    if(!valid_name(name)) return 0;
    if(!valid_email(email)) return 0;

    data[current_id] = (Users) {
        current_id++,
        name,
        email
    };

    return 1;
}

void clear_db(void) {
    for(int i = 0; i < MAX_USERS; ++i) 
        data[i] = (Users){ 0, " ", " " };
    current_id = 0;
    printf("Todas as linhas do banco de dados foram zeradas\n");
}

ShowData show_all_data(void) {
    ShowData data_exit;

    for(int i = 0; i < current_id; ++i) 
        data_exit.data[i] = data[i];

    data_exit.current_index = current_id;
    return data_exit;
}

void load_db(void) {
    // Test Case: Email Fail
    // (UsersModel){ "Ana", "Ana@gmailcom" },
    // Test Case: Name Fail (Number)
    // (UsersModel){ "A889B", "A889B@gmail.com" },
    // Test Case: Name Fail (1 char)
    // (UsersModel){ "A", "a@gmail.com" },

    int errors = 0;
    const UsersModel fill_data[] = {
        (UsersModel){ "Ana", "Ana@gmailcom" },
        (UsersModel){ "Bill", "bill@company.com" },
        (UsersModel){ "Donna", "donna@home.org" },
        (UsersModel){ "Elise", "elise@hotmail.com" },
        (UsersModel){ "Luegiu", "luegiuogrande@corporation.com" }
    };

    for(int i = 0; i < sizeof(fill_data)/sizeof(fill_data[0]); ++i) {
        if(!add_data(fill_data[i].name, fill_data[i].email))
            errors++;
    }

    if(errors > 0) {
        fprintf(stderr, "Carregamento de dados: Fail");
        return;
    }

    printf("Carregamento de dados: Ok\n");
}
