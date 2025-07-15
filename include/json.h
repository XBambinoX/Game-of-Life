#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct jsonPair{
    char* key;
    char* value;
} jsonPair;

typedef struct returnPair{
    jsonPair* data;
    int size;
} returnPair;

char* ReadJson(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* str = (char*)malloc(size + 1);
    if (str == NULL) {
        fclose(file);
        return NULL;
    }
    size_t read = fread(str, 1, size, file);
    str[read] = '\0';

    fclose(file);
    return str;
}

returnPair ParseJson(char* data){
    int size = 0;
    char* str = data;

    while (*str != '\0'){
        if(*str == '"'){
            size++;
        }
        str++;
    }
    size /= 4; //array size

    char word[10];
    jsonPair* arr = (jsonPair*)malloc(size * sizeof(jsonPair));

    int index = 0;
    while (*data != '\0') {
        while (*data != '"' && *data != '\0') data++;
        if (*data == '\0') break;
        data++;

        char word[10];
        int i = 0;
        while (*data != '"' && *data != '\0') {
            word[i++] = *data++;
        }
        word[i] = '\0';
        data++;

        arr[index].key = strdup(word);

        while (*data != '"' && *data != '\0') data++;
        if (*data == '\0') break;
        data++;

        i = 0;
        while (*data != '"' && *data != '\0') {
            word[i++] = *data++;
        }
        word[i] = '\0';
        data++;

        arr[index].value = strdup(word);
        index++;
    }

    returnPair pair;
    pair.data = arr;
    pair.size = size;
    return pair;
}