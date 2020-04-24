#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_LINE_LEN 1024
#define USAGE_ARGUMENTS 2
#define OUTPUT_FILE "railway_planner_output.txt"
#define WRITE_FILE "w"
#define READ_FILE "r"
#define FILE_RAIL_LENGTH_LINE 1
#define FILE_NUM_CONNECTIONS_LINE 2
#define FILE_CONNECTIONS_TYPE_LINE 3

#define USAGE_ERROR "Usage: RailWayPlanner <InputFile>"
#define FILE_EXIST_ERROR "File doesn't exists."
#define FILE_EMPTY_ERROR "File is empty."
#define INVALID_INPUT_ERROR "Invalid input in line: %d" //TODO: Check if lond is needed
#define FAILED_COMMAND -1
#define NO_ERROR 0

typedef struct Piece
{
    int price;
    int length;
    char rightConnection[MAX_LINE_LEN];
    char leftConnection[MAX_LINE_LEN];
}Piece;

typedef struct RailWayPlanner
{
    int length;
    int numConnections;
    char connectionsAllowed[];
}RailWayPlanner;

FILE *getFile(const char *fileLocation, char *type)
{
    FILE *fPtr = fopen(fileLocation, type);
    if (fPtr == NULL)
    {
        exit(EXIT_FAILURE);
    }
    return fPtr;
}

bool checkFileExists(const char *fileLocation)
{
    FILE *fPtr = fopen(fileLocation, "r");
    if (fPtr == NULL)
    {
        return false;
    }
    fclose(fPtr);
    return true;
}

void writeToFile(const char *data)
{
    FILE *file = getFile(OUTPUT_FILE, WRITE_FILE);
    fputs(data, file);
    fclose(file);
}

void removeEnter(char line[])
{
    unsigned long ln = strlen(line) - 1;
    if (*line && line[ln] == '\n')
    {
        line[ln] = '\0';
    }
}

int stringToInt(const char line[])
{
    unsigned long len = strlen(line);
    for (unsigned long i = 0; i < len; i++)
    {
        if (line[i] < '0' || line[i] > '9')
        {
            return FAILED_COMMAND;
        }
    }
    return atoi(line);
}

void getUserData(const char *fileLocation)
{
    FILE *file = getFile(fileLocation, READ_FILE);
    char line[MAX_LINE_LEN];
    long lineNumber = 1;
    RailWayPlanner rail;
    int lineError = NO_ERROR;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        removeEnter(line);

        if (lineNumber == FILE_RAIL_LENGTH_LINE)
        {
            if (stringToInt(line) == FAILED_COMMAND)
            {
                writeToFile(INVALID_INPUT_ERROR);
                lineError = FILE_RAIL_LENGTH_LINE;
            }
            rail.length = stringToInt(line);
        }
        else if (lineNumber == FILE_NUM_CONNECTIONS_LINE)
        {
            if (stringToInt(line) == FAILED_COMMAND)
            {
                writeToFile(INVALID_INPUT_ERROR);
                lineError = FILE_NUM_CONNECTIONS_LINE;
            }
            rail.numConnections = stringToInt(line);
        }
        else if (lineNumber == FILE_CONNECTIONS_TYPE_LINE)
        {
            continue;
        }
        else
        {
            continue;
        }
        lineNumber ++;
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != USAGE_ARGUMENTS)
    {
        writeToFile(USAGE_ERROR);
        exit(EXIT_FAILURE);
    }

    if (checkFileExists(argv[1]) == false)
    {
        writeToFile(FILE_EXIST_ERROR);
        exit(EXIT_FAILURE);
    }
    getUserData(argv[1]);
    writeToFile("hello");

    return 0;
}
