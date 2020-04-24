#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
#define USAGE_ARGUMENTS 2
#define OUTPUT_FILE "railway_planner_output.txt"

#define USAGE_ERROR "Usage: RailWayPlanner <InputFile>"
#define FILE_EXIST_ERROR "File doesn't exists."
#define FILE_EMPTY_ERROR "File is empty."
#define INVALID_INPUT_ERROR "Invalid input in line: %d" //TODO: Check if lond is needed

FILE *getFile(char *fileLocation)
{
    FILE *fPtr = fopen(fileLocation, "wa");
    if (fPtr == NULL)
    {
        exit(EXIT_FAILURE);
    }
    return fPtr;
}

bool checkFileExists(char *fileLocation)
{
    FILE *fPtr = fopen(fileLocation, "r");
    if (fPtr == NULL)
    {
        return false;
    }
    fclose(fPtr);
    return true;
}

void writeToFile(char *fileLocation, char *data)
{
    FILE *file = getFile(OUTPUT_FILE);
    fputs(data, file);
    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != USAGE_ARGUMENTS)
    {
        writeToFile(OUTPUT_FILE, USAGE_ERROR);
        exit(EXIT_FAILURE);
    }

    if (checkFileExists(argv[1]) == false)
    {
        writeToFile(OUTPUT_FILE, FILE_EXIST_ERROR);
        exit(EXIT_FAILURE);
    }

    writeToFile(OUTPUT_FILE, "hello");

    return 0;
}
