#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

// check if arg file is .csv
void check_csv_extension(const char* string) {
    size_t i = 1;
    while(string[i] != '.') {
        if(string[i] == '\0') {
            fprintf(stderr, "Extensao do arquivo nao encontrada\n");
            exit(1);
        }
        i++;
    }

    char csv_text[3] = "csv";
    
    for(int j = 0; j < 3; j++) {
        if(string[i + j + 1] != csv_text[j]) {
            fprintf(stderr, "Extensao do arquivo deve ser csv\n");
            exit(1);
        }
    }

    if(string[i+4] != '\0') {
        fprintf(stderr, "Extensao do arquivo deve ser csv\n");
        exit(1);
    }

    printf("Executando arquivo %s\n", string);
}

// convert and generate output.json file
void convert_csv_to_json(const char* buffer) {
    size_t capacity = 1024;
    char* json = (char*)malloc(capacity);
    if(json == NULL) exit(1);
    size_t i = 0;
    size_t j = 0;

    while(buffer[j] != '\n' && buffer[j] != '\0') j++;
    size_t header_size = j;

    char* header = (char*)malloc(header_size + 1);
    if(header == NULL) exit(1);
    for(size_t x = 0; x < header_size; ++x) {
        if(buffer[x] == ',' || buffer[x] == '\r') {
            header[x] = ' ';
            continue;
        }
        header[x] = buffer[x];
    }
    header[header_size] = '\0';

    if(buffer[j] == '\n') j++;

    i = 0;
    json[i++] = '[';
    json[i++] = '\n';

    int first_obj = 1;
    while(1) {
        if(buffer[j] == '\0') break;

        if(i + 7 >= capacity) {
            capacity *= 2;
            char* temp = (char*)realloc(json, capacity);
            if(temp == NULL) exit(1);
            json = temp;
        }

        if(!first_obj) {
            json[i++] = ',';
            json[i++] = '\n';
        }
        first_obj = 0;

        size_t cursor = 0;
        json[i++] = '{';
        json[i++] = '\n';

        while(header[cursor] != '\0') {
            json[i++] = '"';

            if(i + 7 >= capacity) {
                capacity *= 2;
                char* temp = (char*)realloc(json, capacity);
                if(temp == NULL) exit(1);
                json = temp;
            }

            while(header[cursor] != ' ' && header[cursor] != '\0')
                json[i++] = header[cursor++];

            json[i++] = '"';
            json[i++] = ':';
            json[i++] = '"';

            while(buffer[j] != ',' && buffer[j] != '\0' && buffer[j] != '\n' && buffer[j] != '\r')
                json[i++] = buffer[j++];

            json[i++] = '"';

            if(header[cursor + 1] != '\0') {
                json[i++] = ',';
            }
            json[i++] = '\n';

            if(buffer[j] == ',') j++;

            if(header[cursor] == ' ') cursor++;
        }

        json[i++] = '}';

        if(buffer[j] == '\r') j++;
        if(buffer[j] == '\n') j++;
    }

    json[i++] = '\n';
    json[i++] = ']';
    json[i] = '\0';

    FILE* f = fopen("output.json", "w");
    if(f == NULL) exit(1);
    fwrite(json, sizeof(char), i, f);
    fclose(f);
    free(json);
    free(header);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Por favor entre com algum arquivo .csv para a conversao\n");
        exit(1);
    }

    check_csv_extension(argv[1]);

    FILE* f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* csv_file = malloc(len + 1);

    size_t bytes = fread(csv_file, sizeof(char), len, f);
    csv_file[bytes] = '\0';
    fclose(f);

    convert_csv_to_json(csv_file);

    free(csv_file);

    return 0;
}
