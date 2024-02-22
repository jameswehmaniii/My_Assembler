#include "headers.h"

#define COMMENT 35
#define INDEX_STRING ",X"
#define INPUT_BUF_SIZE 60
#define NEW_LINE 10
#define SPACE 32
#define SYMBOL_TABLE_SIZE 100

// Pass 2 constants
#define BLANK_INSTRUCTION 0x000000
#define OPCODE_MULTIPLIER 0x10000
#define OUTPUT_BUF_SIZE 70
#define RSUB_INSTRUCTION 0x4C0000
#define X_MULTIPLER 0x8000


void performPass1(struct symbol* symbolArray[], char* filename, address* addresses);
struct segment* prepareSegments(char* line);
void trim(char string[]);

// Pass 2 function prototypes
char* createFilename(char* filename, const char* extension);
void performPass2(struct symbol* symbolTable[], char* filename, address* addresses);
void resetObjectFileData(objectFileData* objectData, address* addresses);
void writeToLstFile(FILE* file, int address, segment* segments, int opcode);
void writeToObjFile(FILE* file, objectFileData data);
void extraCredit(struct symbol* symbolTable[], char* filename, address* addresses);


int main(int argc, char* argv[]) {
    address addresses = { 0x00, 0x00, 0x00 };

    // call arguments
    char *fileName = argv[1];
    if (argc < 2) displayError(MISSING_COMMAND_LINE_ARGUMENTS, NULL);

    // create and initialize symbol table
    symbol *symbols[SYMBOL_TABLE_SIZE];
    initializeSymbolTable(symbols);

    //begin processing
    performPass1(symbols, fileName, &addresses);
    performPass2(symbols, fileName, &addresses);
    extraCredit(symbols, fileName, &addresses);

    //print statements
//    displaySymbolTable(symbols);
//    printf("\nAssembly Summary\n"
//           "----------------\n");
//    printf("    Starting Address: 0x%X\n", addresses.start);
//    printf("      Ending Address: 0x%X\n", addresses.current);
//    printf("Program Size (bytes): %d\n\n", addresses.current - addresses.start);

}
///end main

char* createFilename(char* filename, const char* extension) {
    char* temp = (char*)malloc(sizeof(char) * strlen(filename) + 1);
    char* temp1 = (char*)malloc(sizeof(char) * strlen(filename) + 1);

    strcpy(temp1, filename);
    strcpy(temp, strtok(temp1, "."));
    strcat(temp, extension);
    return temp;
}
/// end createFileName

void performPass1(struct symbol* symbolTable[], char* filename, address* addresses) {
    FILE *file;
    file = fopen(filename,"r");
    char txt_file_line[INPUT_BUF_SIZE];

    if (file == NULL) displayError(FILE_NOT_FOUND, NULL);

//    printf("\nSymbol Table Log\n"
//           "----------------\n");

    while (fgets(txt_file_line, INPUT_BUF_SIZE, file) != NULL) {
        int maxMemory = 0x8000;

        if(txt_file_line[0] == COMMENT) continue;  // skips comments
        if(txt_file_line[0] < 32) displayError(BLANK_RECORD, NULL); // if blank line, exit the program

        char castCurrMem2CharString[sizeof(addresses->current)];
        if(addresses->current > maxMemory) {
            sprintf(castCurrMem2CharString, "%X", addresses->current);
            displayError(OUT_OF_MEMORY, castCurrMem2CharString);
        }

        segment *threeSegments = prepareSegments(txt_file_line);

        if (isDirective(threeSegments->first) != 0 ||
            isOpcode(threeSegments->first))
            displayError(ILLEGAL_SYMBOL, threeSegments->first);

        if (isStartDirective(isDirective(threeSegments->second))) {
            addresses[0].start = strtol(threeSegments->third, NULL, 16);
            addresses[0].current = strtol(threeSegments->third, NULL, 16);
        } else if (!isOpcode(threeSegments->second)) {
            int incrementValue = getMemoryAmount(isDirective(threeSegments->second), threeSegments->third);
            addresses->increment = incrementValue;
        }

        if(isOpcode(threeSegments->second)) addresses->increment = 0x3;

        if (isDirective(threeSegments->second) == 0 &&
            !isOpcode(threeSegments->second))
            displayError(ILLEGAL_OPCODE_DIRECTIVE, threeSegments->second);

        if (strlen(threeSegments->first)) insertSymbol(symbolTable, threeSegments->first, addresses->current);

        if (strcmp(threeSegments->first, "COPY") == 0) continue;
        memset(txt_file_line, 0, INPUT_BUF_SIZE);
        //printf("%X    %s\n", addresses->current, threeSegments->first);
        //fflush(stdout);
        addresses->current += addresses->increment;
    }
    fclose(file);
}
/// end performPass1

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
void performPass2(struct symbol* symbolTable[], char* filename, address* addresses) {
    // Do not modify this statement
    objectFileData objectData = {0, {0x0}, {"\0"}, 0, 0x0, 0, {0}, 0, '\0', 0x0};

    int modIndexLocation = 0;
    char txt_file_line[INPUT_BUF_SIZE];
    //creation of two file names for the two output files
    char *lstExt = createFilename(filename, ".lst");
    char *objExt = createFilename(filename, ".obj");

    FILE *file;
    FILE *fOpenLst;
    FILE *fOpenObj;

    file = fopen(filename, "r");
    if (file == NULL) displayError(FILE_NOT_FOUND, NULL);
    fOpenLst = fopen(lstExt, "w");
    fOpenObj = fopen(objExt, "w");

    while (fgets(txt_file_line, INPUT_BUF_SIZE, file) != NULL) {
        if (txt_file_line[0] == COMMENT) continue;

        segment *threeSegments = prepareSegments(txt_file_line);
        char* newThirdSegment = (char*) malloc(sizeof(newThirdSegment));
        strcpy(newThirdSegment, threeSegments->third);
//        printf("%s", newSecondSegment);
//        fflush(stdout);

        //assigning initial values to objectData
        int directiveType = isDirective(threeSegments->second);
        if (isStartDirective(directiveType)) {
            objectData.recordType = 'H';
            strcpy(objectData.programName, threeSegments->first);
            objectData.startAddress = addresses->start;
            objectData.recordAddress = addresses->start;
            objectData.programSize = addresses->current - addresses->start;
            addresses->current = addresses->start;

            writeToObjFile(fOpenObj, objectData);
            writeToLstFile(fOpenLst, addresses->current, threeSegments, BLANK_INSTRUCTION);
            fflush(stdout);
        }

        if (isEndDirective(directiveType) == true) {
            if (objectData.recordByteCount > 0) {
                writeToObjFile(fOpenObj, objectData);
                resetObjectFileData(&objectData, addresses);
            }
            objectData.recordType = 'E';
            writeToObjFile(fOpenObj, objectData);
            writeToLstFile(fOpenLst, addresses->current, threeSegments, BLANK_INSTRUCTION);
        }

        if (isReserveDirective(directiveType) == true) {
            if (objectData.recordByteCount > 0) {
                writeToObjFile(fOpenObj, objectData);
                resetObjectFileData(&objectData, addresses);
            }
            writeToLstFile(fOpenLst, addresses->current, threeSegments, BLANK_INSTRUCTION);

            addresses->increment = getMemoryAmount(directiveType, threeSegments->third);
            objectData.recordAddress += addresses->increment;
        }

        if (isDataDirective(directiveType) == true) {
            addresses->increment = getMemoryAmount(directiveType, threeSegments->third);
            int difference = MAX_RECORD_BYTE_COUNT - addresses->increment;
            if (objectData.recordByteCount > difference) {
                writeToObjFile(fOpenObj, objectData);
                resetObjectFileData(&objectData, addresses);
            }

            int byteWordValue = getByteWordValue(directiveType, newThirdSegment);
            objectData.recordEntries[objectData.recordEntryCount].numBytes = addresses->increment;
            objectData.recordEntries[objectData.recordEntryCount].value = byteWordValue;
            objectData.recordEntryCount += 1;
            objectData.recordByteCount = objectData.recordByteCount + addresses->increment;
            writeToLstFile(fOpenLst, addresses->current, threeSegments, byteWordValue);
        }

        if (isOpcode(threeSegments->second) == true) {
            int difference = MAX_RECORD_BYTE_COUNT - 3;

            if (objectData.recordByteCount > difference) {
                strcpy(&objectData.recordType, "T");
                writeToObjFile(fOpenObj, objectData);
                resetObjectFileData(&objectData, addresses);
            }

            int multipliedValue = getOpcodeValue(threeSegments->second);
            multipliedValue *= OPCODE_MULTIPLIER;
            int fullOpcodeValue = 0;

            char* symbolName;
            if (strstr(threeSegments->third, INDEX_STRING)) {
                symbolName = strtok(newThirdSegment, ",");
                multipliedValue += X_MULTIPLER;
                fullOpcodeValue = getSymbolAddress(symbolTable, symbolName) + multipliedValue;
            }

            //if (multipliedValue != 0x4C) {  //opcode value of 0x4C
            if (strcmp(threeSegments->second, "RSUB") != 0) {
                int symbolAddress = getSymbolAddress(symbolTable, newThirdSegment);
                if (symbolAddress == -1) displayError(UNDEFINED_SYMBOL, newThirdSegment);
                else fullOpcodeValue = symbolAddress + multipliedValue;
            } else { fullOpcodeValue = RSUB_INSTRUCTION; }

            objectData.recordEntries[objectData.recordEntryCount].numBytes = 3;
            objectData.recordEntries[objectData.recordEntryCount].value = fullOpcodeValue;
            objectData.recordEntryCount += 1;
            objectData.recordByteCount += 3;

            writeToLstFile(fOpenLst, addresses->current, threeSegments, fullOpcodeValue);

            addresses->increment = 3;
        }

        addresses->current += addresses->increment;
        memset(txt_file_line, 0, INPUT_BUF_SIZE);

    }
    fclose(file);
    fclose(fOpenLst);
    fclose(fOpenObj);
}
/// performPass2


void extraCredit(struct symbol* symbolTable[], char* filename, address* addresses) {
    // Do not modify this statement
    objectFileData objectData = {0, {0x0}, {"\0"}, 0, 0x0, 0, {0}, 0, '\0', 0x0};

    addresses->increment = 0;
    char extraCredit [15] = "EC";
    strcat(extraCredit, filename);
    char extraCreditLst[15] = "EC";
    strcat(extraCreditLst, filename);

    objectData.modificationCount = 0;
    int modIndexLocation = 0;
    char txt_file_line[INPUT_BUF_SIZE];
    //creation of two file names for the two output files
    char *objExtEC = createFilename(extraCredit, ".obj");
    char *objLstEC = createFilename(extraCredit, ".obj");

    FILE *file;
    FILE *fOpenObjEC;
    FILE *fOpenLstEC;

    file = fopen(filename, "r");
    if (file == NULL) displayError(FILE_NOT_FOUND, NULL);
    fOpenObjEC = fopen(objExtEC, "w");
    fOpenLstEC = fopen(objLstEC, "w");

    while (fgets(txt_file_line, INPUT_BUF_SIZE, file) != NULL) {
        if (txt_file_line[0] == COMMENT) continue;

        segment *threeSegments = prepareSegments(txt_file_line);
        char* newThirdSegment = (char*) malloc(sizeof(newThirdSegment));
        strcpy(newThirdSegment, threeSegments->third);

        int directiveType = isDirective(threeSegments->second);
        if (isStartDirective(directiveType)) {
            objectData.recordType = 'H';
            strcpy(objectData.programName, threeSegments->first);
            objectData.startAddress = 0x0000; //addresses->start
            objectData.recordAddress = 0x0000;
            objectData.programSize = addresses->current - addresses->start - 1;
            addresses->current = 0x0000;

            writeToObjFile(fOpenObjEC, objectData);
            //writeToLstFile(fOpenLstEC, addresses->current, threeSegments, BLANK_INSTRUCTION);
            fflush(stdout);
        }

        /// extra credit conversion start
        char* ecThird = (char*)malloc(sizeof(ecThird));
        strcpy(ecThird, threeSegments->third);
        for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
            if (strcmp(threeSegments->third, "FIRST") == 0) continue;
            if (symbolTable[i] == NULL) continue;
            char* tempString = (char*) malloc(sizeof(tempString));
            strcpy(tempString, symbolTable[i]->name);
            if (strstr(threeSegments->third, INDEX_STRING)) tempString = strtok(ecThird, ",");
            if (strcmp(ecThird, tempString) == 0) {
                objectData.modificationEntries[objectData.modificationCount] = addresses->current + 1;
                //printf("%s matches at index %d address of %04X,  %i\n", ecThird, i, objectData.modificationEntries[objectData.modificationCount], objectData.modificationCount);
                objectData.modificationCount++;
                fflush(stdout);
                break;
            }
        }
        /// extra credit conversion end

        if (isEndDirective(directiveType) == true) {
            if (objectData.recordByteCount > 0) {
                writeToObjFile(fOpenObjEC, objectData);
                resetObjectFileData(&objectData, addresses);
            }
            objectData.recordType = 'M';
            writeToObjFile(fOpenObjEC, objectData);
            objectData.recordType = 'E';
            writeToObjFile(fOpenObjEC, objectData);
            //writeToLstFile(fOpenLstEC, addresses->current, threeSegments, BLANK_INSTRUCTION);
        }

        if (isReserveDirective(directiveType) == true) {
            if (objectData.recordByteCount > 0) {
                writeToObjFile(fOpenObjEC, objectData);
                resetObjectFileData(&objectData, addresses);
            }
            //writeToLstFile(fOpenLstEC, addresses->current, threeSegments, BLANK_INSTRUCTION);
            addresses->increment = getMemoryAmount(directiveType, threeSegments->third);
            objectData.recordAddress += addresses->increment;
        }

        if (isDataDirective(directiveType) == true) {
            addresses->increment = getMemoryAmount(directiveType, threeSegments->third);
            int difference = MAX_RECORD_BYTE_COUNT - addresses->increment;
            if (objectData.recordByteCount > difference) {
                writeToObjFile(fOpenObjEC, objectData);
                resetObjectFileData(&objectData, addresses);
            }

            int byteWordValue = getByteWordValue(directiveType, newThirdSegment);
            objectData.recordEntries[objectData.recordEntryCount].numBytes = addresses->increment;
            objectData.recordEntries[objectData.recordEntryCount].value = byteWordValue;
            objectData.recordEntryCount += 1;
            objectData.recordByteCount = objectData.recordByteCount + addresses->increment;
            //writeToLstFile(fOpenLstEC, addresses->current, threeSegments, byteWordValue);
        }

        if (isOpcode(threeSegments->second) == true) {
            int difference = MAX_RECORD_BYTE_COUNT - 3;

            if (objectData.recordByteCount > difference) {
                strcpy(&objectData.recordType, "T");
                writeToObjFile(fOpenObjEC, objectData);
                resetObjectFileData(&objectData, addresses);
            }

            int multipliedValue = getOpcodeValue(threeSegments->second);
            multipliedValue *= OPCODE_MULTIPLIER;
            int fullOpcodeValue = 0;

            char* symbolName;
            if (strstr(threeSegments->third, INDEX_STRING)) {
                symbolName = strtok(newThirdSegment, ",");
                multipliedValue += X_MULTIPLER;
                fullOpcodeValue = getSymbolAddress(symbolTable, symbolName) + multipliedValue - 0x1000;
            }

            //if (multipliedValue != 0x4C) {  //opcode value of 0x4C
            if (strcmp(threeSegments->second, "RSUB") != 0) {
                int symbolAddress = getSymbolAddress(symbolTable, newThirdSegment);
                if (symbolAddress == -1) displayError(UNDEFINED_SYMBOL, newThirdSegment);
                else fullOpcodeValue = symbolAddress + multipliedValue - 0x1000;
            } else { fullOpcodeValue = RSUB_INSTRUCTION; }

            objectData.recordEntries[objectData.recordEntryCount].numBytes = 3;
            objectData.recordEntries[objectData.recordEntryCount].value = fullOpcodeValue;
            objectData.recordEntryCount += 1;
            objectData.recordByteCount += 3;
            //writeToLstFile(fOpenLstEC, addresses->current, threeSegments, fullOpcodeValue);

            addresses->increment = 3;
        }

        addresses->current += addresses->increment;
        memset(txt_file_line, 0, INPUT_BUF_SIZE);
    }
    fclose(file);
    fclose(fOpenObjEC);
    fclose(fOpenLstEC);
}
/// extra credit end


segment* prepareSegments(char* statement) {
    struct segment* temp = (segment*)malloc(sizeof(segment));
    strncpy(temp->first, statement, SEGMENT_SIZE - 1);
    strncpy(temp->second, statement + SEGMENT_SIZE - 1, SEGMENT_SIZE - 1);
    strncpy(temp->third, statement + (SEGMENT_SIZE - 1) * 2, SEGMENT_SIZE - 1);

    trim(temp->first);
    trim(temp->second);
    trim(temp->third);
    return temp;
}
/// end prepareSegments

void resetObjectFileData(objectFileData* objectData, address* addresses) {
    objectData->recordAddress = addresses->current;
    objectData->recordByteCount = 0;
    objectData->recordEntryCount = 0;
}
/// end resetObjectFileData

void trim(char value[])
{
    for (int x = 0; x < SEGMENT_SIZE; x++)
    {
        if (value[x] == SPACE || x == (SEGMENT_SIZE - 1))
        {
            value[x] = '\0';
        }
    }
}
/// end trim

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
void writeToLstFile(FILE* file, int address, segment* segments, int opcode) {
    int directiveNumber = isDirective(segments->second);

    if (isStartDirective(directiveNumber) ||
        isEndDirective(directiveNumber)) {
        fprintf(file, "%-8.04X%-8s%-8s%-12s\n", address, segments->first, segments->second, segments->third);
//            printf("%-8.04X%-8s%-8s%-12s\n", address, segments->first, segments->second, segments->third);
//            fflush(stdout);
    } else if (isEndDirective(directiveNumber)) {
        fprintf(file, "%-8.04X%-8s-8%s-12%s", address, segments->first, segments->second, segments->third);
//        printf("%-8.04X%-8s-8%s-12%s", address, segments->first, segments->second, segments->third);
//        fflush(stdout);
    }

    if(isReserveDirective(directiveNumber)) {
        fprintf(file, "%-8.04X%-8s%-8s%-12s\n", address, segments->first, segments->second, segments->third);
//        printf("%-8.04X%-8s%-8s%-12s\n", address, segments->first, segments->second, segments->third);
//        fflush(stdout);
    }

    if (isDataDirective(directiveNumber)) {
        if (directiveNumber == 1) {
            fprintf(file, "%-8.04X%-8s%-8s%-12s%02X\n", address, segments->first, segments->second, segments->third, opcode);
//            printf("%-8.04X%-8s%-8s%-12s%02X\n", address, segments->first, segments->second, segments->third, opcode);
//            fflush(stdout);
        } else {
            fprintf(file, "%-8.04X%-8s%-8s%-12s%06X\n", address, segments->first, segments->second, segments->third, opcode);
//            printf("%-8.04X%-8s%-8s%-12s%06X\n", address, segments->first, segments->second, segments->third, opcode);
//            fflush(stdout);
        }
    }

    if (isOpcode(segments->second)) {
        fprintf(file, "%-8.04X%-8s%-8s%-12s%06X\n", address, segments->first, segments->second, segments->third, opcode);
//        printf("%-8.04X%-8s%-8s%-12s%06X\n", address, segments->first, segments->second, segments->third, opcode);
//        fflush(stdout);
    }
}
/// end writeToLstFile

void writeToObjFile(FILE* file, objectFileData fileData) {
    if (fileData.recordType == 'H') {
        fprintf(file, "%C%-6s%06X%06X", fileData.recordType, fileData.programName, fileData.startAddress, fileData.programSize);
//        printf("%C%-6s%06X%06X", fileData.recordType, fileData.programName, fileData.startAddress, fileData.programSize);
//        fflush(stdout);
    } else if (fileData.recordType == 'T') {
        fprintf(file, "\n%C%06X%02X", fileData.recordType, fileData.recordAddress, fileData.recordByteCount);
//        printf("\n%C%06X%02X", fileData.recordType, fileData.recordAddress, fileData.recordByteCount);
//        fflush(stdout);
        for (int i = 0; i < fileData.recordEntryCount; i++) {
            if (fileData.recordEntries[i].numBytes == 3) {
                fprintf(file, "%06X", fileData.recordEntries[i].value);
//                printf("%06X", fileData.recordEntries[i].value);
//                fflush(stdout);
            } else {
                fprintf(file, "%02X", fileData.recordEntries[i].value);
//                printf("%02X", fileData.recordEntries[i].value);
//                fflush(stdout);
            }
        }
    }  else if (fileData.recordType == 'M') {
//        printf("\n");
        fflush(stdout);
        for (int i = 0; i < 31; i++) {
            fprintf(file, "%C%06X04+%s\n", fileData.recordType, fileData.modificationEntries[i],fileData.programName);
//            printf("%C%06X04+%s\n", fileData.recordType, fileData.modificationEntries[i], fileData.programName);
//            fflush(stdout);
        }
    }  else if (fileData.recordType == 'E') {
        fprintf(file, "%C%06X", fileData.recordType, fileData.startAddress);
//        printf("%C%06X", fileData.recordType, fileData.startAddress);
//        fflush(stdout);
    }
}
/// end writeToObjFile

/// END PROGRAM ///