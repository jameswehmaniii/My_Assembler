#include "headers.h"

#define MOD_SIZE 10
#define SYMBOL_TABLE_SEGMENTS 10
#define SYMBOL_TABLE_SIZE 100

int computeHash(char* symbolName);

int computeHash(char* symbolName) {
    int length = (int) strnlen(symbolName, NAME_SIZE);
    int hash_value = 0;

    for (int i = 0; i < length; i++) {
        hash_value += symbolName[i];
        hash_value = (hash_value * symbolName[i] % SYMBOL_TABLE_SIZE);
    }
    return hash_value % MOD_SIZE * SYMBOL_TABLE_SEGMENTS;
}

void displaySymbolTable(struct symbol* symbolTable[]) {
    printf("\nSymbol Table Contents\n"
           "---------------------\n");
    printf("%-7s %-6s %s\n", "Index", "Name", "Address"
                                              "\n-----  ------  -------");

    for (int i = 0; i < SYMBOL_TABLE_SIZE; ++i) {
        if (symbolTable[i] != NULL)
            printf("%5d  %-7s 0x%X\n", i, symbolTable[i]->name, symbolTable[i]->address);
    }
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
int getSymbolAddress(struct symbol* symbolArray[], char* string) {
    int hashValue = computeHash(string);
    int i = 0;
    i = hashValue + 1;

    if (symbolArray[hashValue] == NULL) return -1;
    else if (strcmp(symbolArray[hashValue]->name, string) == 0) return symbolArray[hashValue]->address;
    else {
        while(symbolArray[i]!=NULL) {
            if(strcmp(symbolArray[i]->name,string) == 0) {
                return symbolArray[i]->address;
            }
            i++;
        }
        return -1;
    }
}

void initializeSymbolTable(struct symbol* symbolTable[]) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) symbolTable[i] = NULL;
}

bool insertSymbol(struct symbol* symbolTable[], char symbolName[], int symbolAddress) {
    if (symbolName == NULL) return false;
    int index = computeHash(symbolName);

    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        int try = (i + index) % SYMBOL_TABLE_SIZE;

        if (symbolTable[try] != NULL) {
            if (strcmp(symbolTable[try]->name, symbolName) == 0) {
                displayError(DUPLICATE, symbolName);
                return false;
            }
        }
        if (symbolTable[try] == NULL) {
            symbolTable[try] = malloc(sizeof(symbol));
            strcpy(symbolTable[try]->name, symbolName);
            symbolTable[try]->address = symbolAddress;
//            printf("Added %s to Symbol Table at Index %d\n", symbolName, try);
            return true;
        }
    }
    return false;
}