#pragma once

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_USERS 1000

typedef struct {
    int id;
    const char* name;
    const char* email;
} Users;

typedef struct {
    const char* name;
    const char* email;
} UsersModel;

typedef struct {
    Users data[MAX_USERS];
    int current_index;
} ShowData;

void clear_db(void);
void load_db(void);
ShowData show_all_data(void);
