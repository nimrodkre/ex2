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
#define PIECE_LEFT_CONNECTION_LOC 1
#define PIECE_RIGHT_CONNECTION_LOC 2
#define PIECE_QUANTITY_LOC 3
#define PIECE_PRICE_LOC 4
#define TOKEN_LENGTH 1

#define USAGE_ERROR "Usage: RailWayPlanner <InputFile>"
#define FILE_EXIST_ERROR "File doesn't exists."
#define FILE_EMPTY_ERROR "File is empty." //TODO: TAKE CARE
#define INVALID_INPUT_ERROR "Invalid input in line: %d" //TODO: Check if lond is needed
#define STRING_INT_CONVERSION_ERROR -1
#define NO_ERROR 0

typedef struct Piece
{
    int price;
    int quantity;
    char rightConnection;
    char leftConnection;
}Piece;

typedef struct RailWayPlanner
{
    int length;
    int numConnections;
    char *connectionsAllowed;
    Piece *pieces;
    int numPieces;
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
            return STRING_INT_CONVERSION_ERROR;
        }
    }
    return atoi(line);
}

int getRailConnections(char line[], char *connections)
{
    char *token = strtok(line, ",");
    int curr = 0;

    while (token != NULL)
    {
        if (strlen(token) != TOKEN_LENGTH)
        {
            return EXIT_FAILURE;
        }
        connections[curr] = *token;
        curr++;
        token = strtok(0, ",");
    }

    return NO_ERROR;
}

bool checkConnection(const char *connection, const char *allowedConnections)
{
    for (int i = 0; i < strlen(allowedConnections); i++)
    {
        if (*connection == allowedConnections[i])
        {
            return true;
        }
    }
    return false;
}

int getPiece(char line[], Piece *pieces, const int loc, const char *allowedConnections)
{
    char *token = strtok(line, ",");
    int curr = 1;

    while (token != NULL)
    {
        if (curr == PIECE_LEFT_CONNECTION_LOC)
        {
            if (checkConnection(token, allowedConnections) == false)
            {
                return EXIT_FAILURE;
            }
            pieces[loc].leftConnection = *token;
        }
        else if (curr == PIECE_RIGHT_CONNECTION_LOC)
        {
            if (checkConnection(token, allowedConnections) == false)
            {
                return EXIT_FAILURE;
            }
            pieces[loc].rightConnection = *token;
        }
        else if (curr == PIECE_QUANTITY_LOC)
        {
            if (stringToInt(token) == STRING_INT_CONVERSION_ERROR)
            {
                return EXIT_FAILURE;
            }
            pieces[loc].quantity = stringToInt(token);
        }
        else
        {
            if (stringToInt(token) == STRING_INT_CONVERSION_ERROR)
            {
                return EXIT_FAILURE;
            }
            pieces[loc].price = stringToInt(token)  ;
        }

        token = strtok(0, ",");
        curr++;
    }

    return NO_ERROR;
}

void getUserData(const char *fileLocation)
{
    FILE *file = getFile(fileLocation, READ_FILE);
    char line[MAX_LINE_LEN];
    long lineNumber = 1;
    int lineError = NO_ERROR;
    RailWayPlanner rail;
    Piece *pieces = (Piece *)malloc(sizeof(Piece));
    rail.numPieces = 1;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        removeEnter(line);

        if (lineNumber == FILE_RAIL_LENGTH_LINE)
        {
            if (stringToInt(line) == STRING_INT_CONVERSION_ERROR)
            {
                writeToFile(INVALID_INPUT_ERROR);
                lineError = FILE_RAIL_LENGTH_LINE;
            }
            rail.length = stringToInt(line);
        }
        else if (lineNumber == FILE_NUM_CONNECTIONS_LINE)
        {
            if (stringToInt(line) == STRING_INT_CONVERSION_ERROR)
            {
                writeToFile(INVALID_INPUT_ERROR);
                lineError = FILE_NUM_CONNECTIONS_LINE;
            }
            rail.numConnections = stringToInt(line);
        }
        else if (lineNumber == FILE_CONNECTIONS_TYPE_LINE)
        {
            char *connections = (char *)malloc(rail.numConnections * sizeof(char));
            if (getRailConnections(line, connections) == EXIT_FAILURE)
            {
                writeToFile(INVALID_INPUT_ERROR);
                lineError = FILE_CONNECTIONS_TYPE_LINE;
            }
            rail.connectionsAllowed = connections;
        }
        else
        {
            if (lineNumber == 5)
            {
                int i = 0;
            }
            if (getPiece(line, pieces, rail.numPieces - 1, rail.connectionsAllowed) == EXIT_FAILURE)
            {
                writeToFile(INVALID_INPUT_ERROR);
                lineError = lineNumber;
            }
            rail.numPieces++;
            pieces = (Piece *)realloc(pieces, sizeof(Piece) * rail.numPieces);
        }
        lineNumber ++;
    }
    rail.pieces = pieces;
    rail.numPieces--;
    for (int i = 0; i < rail.numPieces; i++)
    {
        printf("%c,%c,%d,%d\n", rail.pieces[i].leftConnection, rail.pieces[i].rightConnection, rail.pieces[i].quantity, rail.pieces[i].price);
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
