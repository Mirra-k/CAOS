#include <assert.h>
//#include <libio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define CELL_SIZE 8
#define CELL_COUNT 8
#define ELEMENTS_COUNT 62

struct Set{
    char cells[CELL_COUNT];
};

void clearSet(struct Set *array){
    memset(array->cells, 0, CELL_COUNT);
}

int getIndex(char c) {

    if (isdigit(c)) {
        return (int)(c - '0');
    }
    else if (isupper(c)) {
        return 10 + (int)(c - 'A');
    }
    else {
        return 36 + (int)(c - 'a');
    }
}

void add(struct Set *set, char c) {
    int global_index = getIndex(c);
    int cell = global_index / CELL_SIZE;
    int local_index = global_index % CELL_SIZE;

    set->cells[cell] = set->cells[cell] | (1 << local_index);
}

void intersection(struct Set *first, struct Set *second) {
    for (int i = 0; i < CELL_COUNT; ++i) {
        first->cells[i] = first->cells[i] & second->cells[i];
    }
    clearSet(second);
}

void unite(struct Set *first, struct Set *second) {
    for (int i = 0; i < CELL_COUNT; ++i) {
        first->cells[i] = first->cells[i] | second->cells[i];
    }
    clearSet(second);
}

void xor(struct Set *first, struct Set *second) {
    for (int i = 0; i < CELL_COUNT; ++i) {
        first->cells[i] = first->cells[i] ^ second->cells[i];
    }
    clearSet(second);
}

void complement(struct Set *set) {
    for (int i = 0; i < CELL_COUNT; ++i) {
        set->cells[i] = ~set->cells[i];
    }
}

char get_char(int index) {
    if (index < 10) {
        return (char)('0' + index);
    }
    else if (index < 36) {
        return (char)('A' + index - 10);
    }
    else {
        return (char)('a' + index - 36);
    }
}

int main() {
    struct Set result_set;
    clearSet(&result_set);
    struct Set current_set;
    clearSet(&current_set);

    char current_char;
    while (scanf("%c", &current_char) != EOF) {
        if (current_char == '&') {
            intersection(&result_set, &current_set);
        }
        else if (current_char == '|') {
            unite(&result_set, &current_set);
        }
        else if (current_char == '^') {
            xor(&result_set, &current_set);
        }
        else if (current_char == '~') {
            complement(&result_set);
        }
        else {
            add(&current_set, current_char);
        }
    }

    for (int i = 0; i < ELEMENTS_COUNT; ++i) {
        int cell = i / CELL_SIZE;
        int local_index = i % CELL_SIZE;

        if ((result_set.cells[cell] & (1 << local_index)) != 0) {
            printf("%c", get_char(i));
        }
    }

    return 0;
.
