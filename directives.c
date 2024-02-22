#include "headers.h"

#define SINGLE_QUOTE 39

enum directives {
    ERROR, BYTE, END, RESB, RESW, START, WORD
};

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
int getByteWordValue(int directiveType, char* string) {
    char firstCharInString = string[0],
            intToHexChar[4] = {0},
            output[10] = {0},
            finalHexString[10] = {0};
    int intValueFromChar,
            hexValueFromInt,
            tempNumber;

    char *hexString = strtok(string, "'");
    hexString = strtok(NULL, "'");

    switch (directiveType) {
        case BYTE:
            if (firstCharInString == 'X') {
                return strtol(hexString, NULL, 16);
            } else if (firstCharInString == 'C') {
                for (int i = 0; i < strlen(hexString); ++i) {
                    intValueFromChar = (int) hexString[i];
                    sprintf(output, "%X", intValueFromChar);
                    strcat(finalHexString, output);
                    hexValueFromInt = strtol(output, NULL, 16);
//                    printf("%X", hexValueFromInt);
//                    fflush(stdout);
                }
                return strtol(finalHexString, NULL, 16);
            }
        case WORD:
            tempNumber = strtol(string, NULL, 10);
            sprintf(intToHexChar, "%X", tempNumber);
            return strtol(intToHexChar, NULL, 16);
    }
}

int getMemoryAmount(int directiveType, char* string) {
    char intToHexChar[4] = {0};
    int tempNumber;
    char* tempString = (char*) malloc(sizeof(tempString));
    strcpy(tempString, string);

    switch (directiveType) {
        case BYTE:
            if (string[0] == 'X') {
                tempString[strlen(tempString) - 1] = 0;
                char *hexString = strtok(tempString, "'");
                hexString = strtok(NULL, "'");
                tempNumber = strtol(hexString, NULL, 16);

                if (tempNumber < 0 || tempNumber > 0xFF) {
                    displayError(OUT_OF_RANGE_BYTE, string);
                } else return 0x1;

            } else if (string[0] == 'C') {
//                char *charString = strtok(tempString, "'"); //this changes '/' to '/000'
//                charString = strtok(tempString, "'");
//                int amountCharacters = strlen(charString);
////            printf("size of string is %d\n", amountCharacters);
////            fflush(stdout);
//                sprintf(intToHexChar, "%X", amountCharacters);
//                return strtol(intToHexChar, NULL, 16);
                return strlen(string) - 3;
            }
        case END:
            return 0x0;
        case START:
            return 0x0;
        case RESB:
            tempNumber = strtol(string, NULL, 10);
            sprintf(intToHexChar, "%X", tempNumber);
            return strtol(intToHexChar, NULL, 16);
        case RESW:
            return strtol(string, NULL, 10) * 3;
        case WORD: {
            int tempInt = atoi(string);
            char cast_Int_to_charString[strlen(string)];
            sprintf(cast_Int_to_charString, "%i", tempInt);
            if (tempInt > 16777216) displayError(OUT_OF_RANGE_WORD, cast_Int_to_charString);
            return 0x3;
        }
        default: ;
    }
}
// end getMemoryAmount

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
bool isDataDirective(int directiveType) {
    if (directiveType == BYTE || directiveType == WORD) return true;
    return false;
}

int isDirective(char* string) {
    if (strcmp(string, "BYTE") == 0) return BYTE;
    else if (strcmp(string, "END") == 0) return END;
    else if (strcmp(string, "RESB") == 0) return RESB;
    else if (strcmp(string, "RESW") == 0) return RESW;
    else if (strcmp(string, "START") == 0) return START;
    else if (strcmp(string, "WORD") == 0) return WORD;
    else return ERROR;
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
bool isEndDirective(int directiveType) {
    if (directiveType == END) return true;
    return false;
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
bool isReserveDirective(int directiveType) {
    if (directiveType == RESB || directiveType == RESW) return true;
    return false;
}

bool isStartDirective(int directiveType) {
    if (directiveType == START) return true;
    else return false;
}