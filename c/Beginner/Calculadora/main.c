#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NODE_NUM,
    NODE_BIN
} NodeKind;

typedef struct {
    const char* string;
    size_t cursor;
} CalcEntry;

typedef struct Node {
    NodeKind kind;

    union {
        int num;

        struct {
            char op;
            struct Node* left;
            struct Node* right;
        } bin;
    } ast;
} Node;

static Node* number(int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->kind = NODE_NUM;
    node->ast.num = value;
    return node;
}

static void skip_space(CalcEntry* entry) {
    while(entry->string[entry->cursor] == ' ') entry->cursor++;
}

static Node* parse_number(CalcEntry* entry) {
    skip_space(entry);

    int value = 0;
    while(entry->string[entry->cursor] >= '0' && entry->string[entry->cursor] <= '9') {
        value = value * 10 + (entry->string[entry->cursor] - '0');
        entry->cursor++;
    }

    return number(value);
}

static Node* parse_fact(CalcEntry* entry){ return parse_number(entry); }

static Node* binary(char op, Node *left, Node *right) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->kind = NODE_BIN;
    node->ast.bin.op = op;
    node->ast.bin.left = left;
    node->ast.bin.right = right;
    return node;
}

static Node* parse_term(CalcEntry* entry) {
    Node* left = parse_fact(entry);

    while(1) {
        skip_space(entry);

        char op = entry->string[entry->cursor];

        if(op != 'x' && op != '/') break;

        entry->cursor++;

        Node* right = parse_fact(entry);

        left = binary(op, left, right);
    }

    return left;
}

static Node* parse_expr(CalcEntry* entry) {
    Node* left = parse_term(entry);

    while(1) {
        skip_space(entry);

        char op = entry->string[entry->cursor];

        if(op != '+' && op != '-') break;

        entry->cursor++;

        Node* right = parse_term(entry);

        left = binary(op, left, right);
    }

    return left;
}

static int evaluate(Node* node) {
    if(node->kind == NODE_NUM) return node->ast.num;

    int left = evaluate(node->ast.bin.left);
    int right = evaluate(node->ast.bin.right);

    switch (node->ast.bin.op)
    {
    case '+': return left + right;
    case '-': return left - right;
    case 'x': return left * right;
    case '/': return left / right;
    }

    return 0;
}

int main(void) {
    CalcEntry entry;
    char string[15];
    scanf("%s", string);
    entry.string = string;
    entry.cursor = 0;

    Node* ast = parse_expr(&entry);

    printf("Resultado: %d\n", evaluate(ast));

    free(ast);
}
