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
#define TOKEN_LENGTH 1

#define USAGE_ERROR "Usage: RailWayPlanner <InputFile>"
#define FILE_EXIST_ERROR "File doesn't exists."
#define FILE_EMPTY_ERROR "File is empty." //TODO: TAKE CARE
#define INVALID_INPUT_ERROR "Invalid input in line: " //TODO: Check if lond is needed
#define STRING_INT_CONVERSION_ERROR -1
#define NO_ERROR 0

typedef struct Piece
{
    int price;
    int length;
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
            pieces[loc].length = stringToInt(token);
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

void freeRailWayPlanner(RailWayPlanner *railWay)
{
    /**
    for (int i = 0; i < railWay->numConnections; i++)
    {
        free(&(railWay->connectionsAllowed[i]));
    }*/
    free(railWay->connectionsAllowed);
    /**
    for (int i = 0; i < railWay->numPieces; i++)
    {
        free(&(railWay->pieces[i]));
    }*/
    free((railWay->pieces));
}

void userDataError(const int lineNumber)
{
    FILE *file = getFile(OUTPUT_FILE, WRITE_FILE);
    fputs(INVALID_INPUT_ERROR, file);
    fprintf(file, "%d", lineNumber);
    fclose(file);

}

void getUserData(const char *fileLocation, RailWayPlanner *rail)
{
    FILE *file = getFile(fileLocation, READ_FILE);
    char line[MAX_LINE_LEN];
    int lineNumber = 1;
    int lineError = NO_ERROR;
    Piece *pieces = (Piece *)malloc(sizeof(Piece));
    rail->numPieces = 1;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        removeEnter(line);

        if (lineNumber == FILE_RAIL_LENGTH_LINE)
        {
            if (stringToInt(line) == STRING_INT_CONVERSION_ERROR)
            {
                lineError = FILE_RAIL_LENGTH_LINE;
            }
            rail->length = stringToInt(line);
        }
        else if (lineNumber == FILE_NUM_CONNECTIONS_LINE)
        {
            if (stringToInt(line) == STRING_INT_CONVERSION_ERROR)
            {
                lineError = FILE_NUM_CONNECTIONS_LINE;
            }
            rail->numConnections = stringToInt(line);
        }
        else if (lineNumber == FILE_CONNECTIONS_TYPE_LINE)
        {
            char *connections = (char *)malloc(rail->numConnections * sizeof(char));
            if (getRailConnections(line, connections) == EXIT_FAILURE)
            {
                lineError = FILE_CONNECTIONS_TYPE_LINE;
            }
            rail->connectionsAllowed = connections;
        }
        else
        {

            if (getPiece(line, pieces, rail->numPieces - 1, rail->connectionsAllowed) == EXIT_FAILURE)
            {
                lineError = lineNumber;
            }
            rail->numPieces++;
            pieces = (Piece *)realloc(pieces, sizeof(Piece) * rail->numPieces);
        }

        if (lineError != NO_ERROR)
        {
            rail->pieces = pieces;
            freeRailWayPlanner(rail);
            userDataError(lineError);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        lineNumber ++;
    }
    rail->pieces = pieces;
    rail->numPieces--;

    fclose(file);
}

int findCheapest(const int *mat, RailWayPlanner *rail, int endingChar, int lenWanted)
{
    int cheapestPrice = INT_MAX;
    for (int i = 0; i < rail->numPieces; i++)
    {
        if (rail->pieces[i].rightConnection == rail->connectionsAllowed[endingChar] && rail->pieces[i].length <= lenWanted)
        {
            int lenSearching = lenWanted - rail->pieces[i].length;
            for (int j = 0; j < rail->numConnections; j++)
            {
                if (rail->connectionsAllowed[j] == rail->pieces[i].leftConnection && mat[rail->numConnections * lenSearching + j] != INT_MAX)
                {
                    int correspondingMatPlace = rail->numConnections * lenSearching + j;
                    int price = mat[correspondingMatPlace] + rail->pieces[i].price;

                    if (price < cheapestPrice)
                    {
                        cheapestPrice = price;
                    }
                }
            }
        }

    }
    return cheapestPrice;
}

void buildTable(RailWayPlanner *rail)
{
    int *mat;
    mat = (int *)malloc(rail->numPieces * rail->numConnections * sizeof(int));
    //CharToNumDict *charNumDict = (CharToNumDict *)malloc(rail->numConnections * sizeof(CharToNumDict));
    //0
    for (int i = 0; i < rail->numConnections; i++)
    {
        mat[i] = 0;
    }
    for (int len = 1; len <= rail->length; len++)
    {
        for (int endingChar = 0; endingChar < rail->numConnections; endingChar++)
        {
            mat[len * rail->numConnections + endingChar] = findCheapest(mat, rail, endingChar, len);
        }
    }

    for (int i = 0; i < rail->length + 1; i++)
    {
        for (int j = 0; j < rail->numConnections; j ++)
        {
            printf("%d ", mat[i * rail->numConnections + j]);
        }
        printf("\n");
    }
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
    RailWayPlanner rail;
    getUserData(argv[1], &rail);
    buildTable(&rail);

    return 0;
}
