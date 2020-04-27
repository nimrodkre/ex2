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
#define PIECE_LENGTH_LOC 3
#define TOKEN_LENGTH 1

#define USAGE_ERROR "Usage: RailWayPlanner <InputFile>"
#define FILE_EXIST_ERROR "File doesn't exists."
#define FILE_EMPTY_ERROR "File is empty."
#define INVALID_INPUT_ERROR "Invalid input in line: "
#define MINIMUM_PRICE_FILE "The minimal price is: "
#define NO_POSSIBLE_RAIL -1
#define STRING_INT_CONVERSION_ERROR -1
#define NO_ERROR 0

/**
 * holds the data of the piece
 * price - int
 * length - int
 * rightConnection - string
 * leftConnTokenNum - int with the location of the left connection in the connections array of rail
 */
typedef struct Piece
{
    int price;
    int length;
    char rightConnection;
    int leftConnTokenNum;
}Piece;

/**
 * length - int
 * numConnections - int
 * connectionsAllowed - string
 * pieces - array of Piece
 * numPieces - int
 */
typedef struct RailWayPlanner
{
    int length;
    int numConnections;
    char *connectionsAllowed;
    Piece *pieces;
    int numPieces;
}RailWayPlanner;

/**
 * open a file in the desired location and returns it
 * @param fileLocation a string with the location of the wanted file
 * @param type the type of opening - string
 * @return the file
 */
FILE *getFile(const char *fileLocation, char *type)
{
    FILE *fPtr = fopen(fileLocation, type);
    if (fPtr == NULL)
    {
        exit(EXIT_FAILURE);
    }
    return fPtr;
}

/**
 * Checks if in the given location there's a file
 * @param fileLocation the location of the wanted file
 * @return boolean true or false
 */
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

/**
 * writes to the output file the wanted string
 * @param data the data to write
 */
void writeToFile(const char *data)
{
    FILE *file = getFile(OUTPUT_FILE, WRITE_FILE);
    fputs(data, file);
    fclose(file);
}

/**
 * removes \n from the string
 * @param line the line from which to remove the enter
 */
void removeEnter(char line[])
{
    unsigned long ln = strlen(line) - 1;
    if (*line && line[ln] == '\n')
    {
        line[ln] = '\0';
    }
}

/**
 * converts string to int
 * @param line the string to convert
 * @return if not able returns -1 else returns the int
 */
int stringToInt(const char line[])
{
    long result = 0;
    char *eptr = NULL;
    unsigned long len = strlen(line);
    for (unsigned long i = 0; i < len; i++)
    {
        if (line[i] < '0' || line[i] > '9')
        {
            return STRING_INT_CONVERSION_ERROR;
        }
    }
    if (line[0] == '0')
    {
        return 0;
    }
    result  = strtol(line,  &eptr, 10);
    if(result == 0)
    {
        return STRING_INT_CONVERSION_ERROR;
    }
    return (int)result;
}

/**
 * from the given line finds the connections of the railroad and puts them in the array
 * @param line the line from which to read the connections
 * @param connections the array of connections to fill
 * @return failure or no error
 */
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

/**
 * Checks if the given connecition is in the array of connections
 * @param connection the connection to check
 * @param allowedConnections array of allowed connections
 * @return location is allowedConnection is in else -1
 */
int checkConnection(const char *connection, const char *allowedConnections)
{
    for (size_t i = 0; i < strlen(allowedConnections); i++)
    {
        if (strlen(connection) == 1 && *connection == allowedConnections[i])
        {
            return i;
        }
    }
    return NO_POSSIBLE_RAIL;
}

/**
 * takes care of getting the left conection - special as we save the left connection location
 * @param token token to check
 * @param allowedConnections list of allowed connection
 * @param pieces list with all of the pieces
 * @param loc location of the piece in pieces to enter to
 * @return failed or success
 */
int pieceGetLeftConnection(const char *token, const char *allowedConnections, Piece *pieces, const int loc)
{
    int connectionNum = checkConnection(token, allowedConnections);
    if (connectionNum == NO_POSSIBLE_RAIL)
    {
        return NO_POSSIBLE_RAIL;
    }
    pieces[loc].leftConnTokenNum = connectionNum;
    return EXIT_SUCCESS;
}

/**
 * From the line, is able to get all data of piece
 * @param line the line to read
 * @param pieces the array of pieces which have already been found
 * @param loc the index of the piece which we will build
 * @param allowedConnections a list with all legal connections for our pieces
 * @return FAILURE OR NO ERROR
 */
int getPiece(char line[], Piece *pieces, const int loc, const char *allowedConnections)
{
    char *token = NULL;
    token = strtok(line, ",");
    int curr = 1;

    while (token != NULL)
    {
        if (curr == PIECE_LEFT_CONNECTION_LOC)
        {
            if (pieceGetLeftConnection(token, allowedConnections, pieces, loc) == NO_POSSIBLE_RAIL)
            {
                return EXIT_FAILURE;
            }
        }
        else if (curr == PIECE_RIGHT_CONNECTION_LOC)
        {
            if (checkConnection(token, allowedConnections) == NO_POSSIBLE_RAIL)
            {
                return EXIT_FAILURE;
            }
            pieces[loc].rightConnection = *token;
        }
        else if (curr == PIECE_LENGTH_LOC)
        {
            if (stringToInt(token) == STRING_INT_CONVERSION_ERROR || stringToInt(token) == 0)
            {
                return EXIT_FAILURE;
            }
            pieces[loc].length = stringToInt(token);
        }
        else
        {
            if (stringToInt(token) == STRING_INT_CONVERSION_ERROR || stringToInt(token) == 0)
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

/**
 * frees the given railway from the heap
 * @param railWay a pointer to the railway to free
 */
void freeRailWayPlanner(RailWayPlanner *railWay)
{
    free(railWay->connectionsAllowed);
    free((railWay->pieces));
}

/**
 * prints user data error - from line 4 and onwards
 * @param lineNumber the line number in which there was an error
 */
void userDataError(const int lineNumber)
{
    FILE *file = getFile(OUTPUT_FILE, WRITE_FILE);
    fputs(INVALID_INPUT_ERROR, file);
    fprintf(file, "%d.", lineNumber);
    fclose(file);
}

/**
 * Case of the first line in the file
 * @param rail the rail in which to enter data
 * @param line string with line
 * @return error or none
 */
int userDataRailLength(RailWayPlanner *rail, char *line)
{
    if (stringToInt(line) == STRING_INT_CONVERSION_ERROR)
    {
        return FILE_RAIL_LENGTH_LINE;
    }
    rail->length = stringToInt(line);
    return NO_ERROR;
}

/**
 * case of the second line in file
 * @param rail the rail in which to enter the data
 * @param line line string
 * @return error or none
 */
int userDataNumConnections(RailWayPlanner *rail, char *line)
{
    if (stringToInt(line) == STRING_INT_CONVERSION_ERROR || stringToInt(line) == 0)
    {
        return FILE_NUM_CONNECTIONS_LINE;
    }
    rail->numConnections = stringToInt(line);
    return NO_ERROR;
}

/**
 * case of the third line
 * @param rail the rail
 * @param line string with line
 * @param connections list with connections
 * @return error or none
 */
int userDataRailConnections(RailWayPlanner *rail, char *line, char *connections)
{
    if (getRailConnections(line, connections) == EXIT_FAILURE)
    {
        return FILE_CONNECTIONS_TYPE_LINE;
    }
    rail->connectionsAllowed = connections;
    return NO_ERROR;
}


/**
 * reads user data from the given file
 * @param fileLocation the location of the user file given to us
 * @param rail a pointer to RailWayPlanner which we would like to fill up from the data of the file
 */
void getUserData(const char *fileLocation, RailWayPlanner *rail)
{
    FILE *file = getFile(fileLocation, READ_FILE);
    char line[MAX_LINE_LEN];
    int lineNumber = 1;
    int lineError = NO_ERROR;
    rail->numPieces = 1;
    rail->connectionsAllowed = NULL;

    Piece *pieces = (Piece *)calloc(1, sizeof(Piece));
    if (pieces == NULL)
    {
        fclose(file);
        exit(EXIT_FAILURE);
    }


    while (fgets(line, sizeof(line), file) != NULL)
    {
        removeEnter(line);

        if (lineNumber == FILE_RAIL_LENGTH_LINE)
        {
            lineError = userDataRailLength(rail, line);
        }
        else if (lineNumber == FILE_NUM_CONNECTIONS_LINE)
        {
            lineError = userDataNumConnections(rail, line);
        }
        else if (lineNumber == FILE_CONNECTIONS_TYPE_LINE)
        {
            char *connections = (char *)calloc(rail->numConnections + 1, sizeof(char));
            if (connections == NULL)
            {
                fclose(file);
                free(pieces);
                exit(EXIT_FAILURE);
            }
            lineError = userDataRailConnections(rail, line, connections);
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

            if (pieces == NULL)
            {
                fclose(file);
                free(pieces);
                free(rail->connectionsAllowed);
                exit(EXIT_FAILURE);
            }
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
    // we know that the file was empty
    if (lineNumber == 1)
    {
        writeToFile(FILE_EMPTY_ERROR);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    rail->pieces = pieces;
    rail->numPieces--;

    fclose(file);
}

/**
 * returns the minimum value between the given two
 * @param a first int
 * @param b second int
 * @return the minimum int between the 2
 */
int min(int a, int b)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

/**
 * algorithm which calculates the cheapest possible rail to build with the given parameters
 * @param mat out table built till now
 * @param rail pointer to RailWatPlanner
 * @param endingChar the ending char with which we want to finish our railway
 * @param lenWanted the length of the wanted railway
 * @return the cheapest price found
 */
int findCheapest(const int *mat, const RailWayPlanner *rail, const int endingChar, const int lenWanted)
{
    int cheapestPrice = INT_MAX;
    for (int i = 0; i < rail->numPieces; i++)
    {
        // if piece looks okay - length is smaller than needed and connection is as needed
        if (rail->pieces[i].rightConnection == rail->connectionsAllowed[endingChar] &&
            rail->pieces[i].length <= lenWanted)
        {
            int lenSearching = lenWanted - rail->pieces[i].length;
            // try to find a railway which corresponds
            int correspondingMatPlace = rail->numConnections * lenSearching + rail->pieces[i].leftConnTokenNum;
            // make sure that our starting point is okay, and that according to the table it's legal to do
            if (mat[correspondingMatPlace] != INT_MAX)
            {
                int price = mat[correspondingMatPlace] + rail->pieces[i].price;
                cheapestPrice = min(cheapestPrice, price);
            }
        }

    }
    return cheapestPrice;
}

/**
 * builds a table as explained in the exercise
 * @param rail a pointer to RailWayPointer
 * @return a pointer to the table
 */
int *buildTable(RailWayPlanner *rail)
{
    int *mat;
    mat = (int *)calloc((rail->length + 1) * rail->numConnections, sizeof(int));

    if (mat == NULL)
    {
        freeRailWayPlanner(rail);
        exit(EXIT_FAILURE);
    }

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
    return mat;
}

/**
 * from the given table finds the cheapest possible way to build the railway
 * @param table the table build
 * @param rail pointer to RailWayPointer
 * @return the cheapest possible price
 */
int findCheapestBuild(const int *table, const RailWayPlanner *rail)
{
    int cheapest = INT_MAX;
    for (int endingChar = 0; endingChar < rail->numConnections; endingChar++)
    {
        if (table[rail->length * rail->numConnections + endingChar] < cheapest)
        {
            cheapest = table[rail->length * rail->numConnections + endingChar];
        }
    }
    return cheapest;
}

/**
 * writes the cheapest price possible to the output file
 * @param price the cheapest price
 */
void writeCheapPrice(const int price)
{
    FILE *file = getFile(OUTPUT_FILE, WRITE_FILE);
    fputs(MINIMUM_PRICE_FILE, file);
    if (price < 0 || price == INT_MAX)
    {
        fprintf(file, "%d", NO_POSSIBLE_RAIL);
    }
    else
    {
        fprintf(file, "%d", price);
    }
    fclose(file);
}

/**
 * main function and the brain of the program
 * @param argc number of the arguments received
 * @param argv the arguments received
 * @return SUCCESS or FAILURE
 */
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
    int *table = buildTable(&rail);
    int cheapest = findCheapestBuild(table, &rail);
    freeRailWayPlanner(&rail);
    free(table);
    writeCheapPrice(cheapest);

    return EXIT_SUCCESS;
}
