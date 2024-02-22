#include "headers.h"

// To implement Pass 2 of the assembler for Project 3,
/// Add the UNDEFINED_SYMBOL error to the displayError() function
void displayError(int errorType, char* errorInfo) {
    switch(errorType) {
        case MISSING_COMMAND_LINE_ARGUMENTS:
            printf("\nERROR: Missing command line argument(s). Must use file via command line... Exiting program.\n");
            exit(1);
        case FILE_NOT_FOUND:
            printf("\nERROR: File not found... Exiting program.\n");
            exit(1);
        case BLANK_RECORD:
            printf("\nERROR: Source File Contains Blank Lines.\n");
            exit(1);
        case DUPLICATE:
            printf("\nERROR: Duplicate Symbol Name (%s) Found.\n", errorInfo);
            exit(1);
        case ILLEGAL_OPCODE_DIRECTIVE:
            printf("\nERROR: Illegal Opcode or Directive (%s) Found in Source File.", errorInfo);
            exit(1);
        case ILLEGAL_SYMBOL:
            printf("\nERROR: Symbol Name (%s) Cannot be a Command or Directive.", errorInfo);
            exit(1);
        case OUT_OF_MEMORY:
            printf("\nERROR: Program Address (0x%s) Exceeds Maximum Memory Address [0x8000].\n", errorInfo);
            exit(1);
        case OUT_OF_RANGE_BYTE:
            printf("\nERROR: Byte Value (%s) Out of Range.", errorInfo);
            exit(1);
        case OUT_OF_RANGE_WORD:
            printf("\nERROR: Word Value (%s) Out Of Range [-16,777,216 to 16,777,215].", errorInfo);
            exit(1);
        case UNDEFINED_SYMBOL:
            printf("\nERROR: Symbol Name (%s) Not Defined in Pass 1.", errorInfo);
            exit(1);
        default: ;
    }
}