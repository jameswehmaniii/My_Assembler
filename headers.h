#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constants defined for Pass 1
#define NAME_SIZE 7
#define SEGMENT_SIZE 9

// Constants defined for Pass 2
#define MAX_RECORD_BYTE_COUNT 30

// Added UNDEFINED_SYMBOL to the error enum for Pass 2
enum errors {
    BLANK_RECORD = 1, DUPLICATE, FILE_NOT_FOUND, ILLEGAL_OPCODE_DIRECTIVE, ILLEGAL_SYMBOL,
    MISSING_COMMAND_LINE_ARGUMENTS, OUT_OF_MEMORY, OUT_OF_RANGE_BYTE, OUT_OF_RANGE_WORD,
    UNDEFINED_SYMBOL
};

// Pass 1 structures
typedef struct address
{
    int start;
    int current;
    int increment;
} address;

typedef struct segment
{
    char first[SEGMENT_SIZE];
    char second[SEGMENT_SIZE];
    char third[SEGMENT_SIZE];
} segment;

typedef struct symbol
{
    char name[NAME_SIZE];
    int address;
} symbol;

// Pass 2 structures
typedef struct recordEntry
{
    int numBytes;
    int value;
} recordEntry;

typedef struct objectFileData
{
    int modificationCount; // Used for extra credit only
    int modificationEntries[60]; // Used for extra credit only
    char programName[NAME_SIZE]; // Used for H and M records only
    int programSize; // Used for H record only
    int recordAddress; // Used for T records only
    int recordByteCount; // Used for T records only
    recordEntry recordEntries[60]; // Used to store T record data
    int recordEntryCount; // Used for T records only
    char recordType; // H, T, E or M
    int startAddress; // Used for H and E records only
} objectFileData;

// Pass 1 function prototypes

// directives.c functions
int getMemoryAmount(int directiveType, char* string);
int isDirective(char* string);
bool isStartDirective(int directiveType);

// errors.c functions
void displayError(int errorType, char* errorInfo);

// opcodes.c functions
int getOpcodeValue(char* opcode);
bool isOpcode(char* string);

// symbols.c functions
void displaySymbolTable(struct symbol* symbolTable[]);
void initializeSymbolTable(struct symbol* symbolTable[]);
bool insertSymbol(struct symbol* symbolTable[], char symbolName[], int symbolAddress);

// Pass 2 function prototypes

// directives.c functions
int getByteWordValue(int directiveType, char* string);
bool isDataDirective(int directiveType);
bool isEndDirective(int directiveType);
bool isReserveDirective(int directiveType);

// symbols.c functions
int getSymbolAddress(struct symbol* symbolArray[], char* string);