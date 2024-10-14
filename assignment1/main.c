/*********************************************************************** 
** Program Filename: main.c
** Author: Troy Diaz
** Date: 10/7/24
** Description: 
** Input: 
** Output: 
*********************************************************************/

// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

/*
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*   Run the executable:
*       ./movies
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Struct for movies information */
struct movies
{
    char *title;
    int *year;
    char *language;
    double *rating;
    struct movies *next;
};

/* Function prototypes */
void searchMoviesbyLang(struct movies *head, char* language);
void searchMoviesinYear(struct movies *head, int year);
void searchHighestRatedinYear(struct movies *head);
int getYear();
char* getLang();
struct movies *createMovies(char *currLine);
struct movies *processFile(char *filePath);
void goToOption(struct movies *head, int choice);
void displayOptions(struct movies *head);
void freeList(struct movies *head);
void displayMovieCount(struct movies *head, char *filePath);

/********************************************************************* 
** Function: searchMoviesbyLang
** Description: Displays all movies by their supported language.
** Parameters: 
**      struct movies *head - Pointer to the start of the linked list.
**      char *language - User entered string for requested language.
** Pre-Conditions: Given a linked list and a string of characters.
** Post-Conditions: Displays all movies by language or lets user know
** that no movie supports that specified language.
*********************************************************************/
void searchMoviesbyLang(struct movies *head, char *language){
    
    struct movies* temp = head;
    
    // Flag to see if movie with language exists
    int flag = 0;

    printf("\n");
    while(temp->next != NULL){
        // Compares two strings, return language
        if(strstr(temp->language, language) != NULL){
            printf("%d %s\n", *(temp->year), temp->title);
            flag = 1;
        }
        temp = temp->next;
    }

    if(flag == 0){
        printf("No data about movies released in %s\n", language);
    }
    printf("\n");
}

/********************************************************************* 
** Function: searchMoviesinYear
** Description: Displays movies released in a specified year.
** Parameters: 
**      struct movies *head - Pointer to the start of the linked list.
**      int year - user entered integer for a year.
** Pre-Conditions: Given a linked list and a integer for year
** Post-Conditions: Displays all movies that were released in that year
** or lets user know that no movies were released in that year.
*********************************************************************/
void searchMoviesinYear(struct movies *head, int year){
    
    // create temp head variable
    struct movies *temp = head;

    // Create flag if a movie has been found
    int flag = 0;

    printf("\n");
    while(temp->next != NULL){
        // If movie was released in year, display movie title
        if (*(temp->year) == year){
            printf("Movie name: %s\n", temp->title);
            flag = 1;
        }
        temp = temp->next;
    }

    // If no movie found, alert user
    if (flag == 0)
    {
        printf("There were no movies in that year!\n");
        printf("\n");
    }
    else{
        printf("\n");
    }

    return;
}

/********************************************************************* 
** Function: searchHighestRatedinYear
** Description: Displays the highest rated movies in a range of years.
** Parameters: 
**      struct movies *head - pointer to the start of the linked list.
** Pre-Conditions: Given a linked list.
** Post-Conditions: Displays the highest rated movie in a year. If there is only
** one movie in that year, display that one. If none, skip since
** no movies were released in that year.
*********************************************************************/
void searchHighestRatedinYear(struct movies *head){
    
    // Movies in which year they're from
    int lowerBound = 1900;
    int upperBound = 2021;

    // To see if a movie exists in that year
    int flag = 0;

    struct movies *temp = head;

    // Save movies in new struct variable
    struct movies *topMovie = NULL;

    // Store highest rating from each movie
    double highestRating = 0.0;

    // Go through each year and stop until upperbound
    while(lowerBound <= upperBound){

        // Reset to point back to start of head, reset top movie and highest rating
        temp = head;
        topMovie = NULL;
        highestRating = 0.0;

        // Traverse ll, search for movies in specified year
        while(temp->next != NULL){
            if(*(temp->year) == lowerBound){
                // store highest rated movie
                if(*(temp->rating) > highestRating){
                    highestRating = *(temp->rating);
                    topMovie = temp;
                }
            }
            temp = temp->next;
        }
        // If there was a movie in that year, print out year, title and rating
        if (topMovie != NULL){
            printf("%d %s %.1f\n", *(topMovie->year), topMovie->title, *(topMovie->rating));
        }

        // Increase year
        lowerBound++;
    }
    printf("\n");
}

/********************************************************************* 
** Function: getYear
** Description: Gets user choice of year.
** Parameters: none
** Pre-Conditions: none
** Post-Conditions: Returns an integer.
*********************************************************************/
int getYear(){
    int year = 0;

    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &year);

    return year;
}

/********************************************************************* 
** Function: getLang
** Description: Gets user input of language.
** Parameters: none
** Pre-Conditions: none
** Post-Conditions: Returns a string.
*********************************************************************/
char* getLang(){
    char* language;

    printf("Enter the language for which you want to see movies: ");
    scanf("%s", language);

    return language;
}

/********************************************************************* 
** Function: createMovies
** Description: Parses current line which is comma delimited and creates
** a movies node
** Parameters: 
**      char *currLine - A string containing a line from the csv file
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a node that includes the title, year, language, and 
** rating.
*********************************************************************/
struct movies *createMovies(char *currLine){
    struct movies *currMovies = malloc(sizeof(struct movies));

    // For strtok_r
    char *saveptr;

    // First token is title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovies->title = calloc(strlen(token)+1, sizeof(char));
    strcpy(currMovies->title, token);

    // Second token is year
    token = strtok_r(NULL, ",", &saveptr);
    currMovies->year = malloc(sizeof(int));
    // Convert string to int
    *(currMovies->year) = atoi(token); 

    // Third token is language
    token = strtok_r(NULL, ",", &saveptr);
    currMovies->language = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovies->language, token);

    // Fourth token is rating
    token = strtok_r(NULL, ",", &saveptr);
    currMovies->rating = malloc(sizeof(double));
    // Convert string to double
    *(currMovies->rating) = atof(token); 

    // End of list
    currMovies->next = NULL; 

    return currMovies;
}

/********************************************************************* 
** Function: processFile
** Description: Linked list of movies from each line a specified file.
** Parameters: 
**      char *filePath - A string titled movies_info.csv or any file name.
** Pre-Conditions: The file is opened for reading.
** Post-Conditions: Returns a pointer to the start of the linked list.
*********************************************************************/
struct movies *processFile(char *filePath){
    
    // Open file for reading
    FILE *moviesFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // Head and tails of linked list
    struct movies *head = NULL;
    struct movies *tail = NULL;

    // Read file line by line
    while((nread = getline(&currLine, &len, moviesFile)) != -1){

        // Get new movies node for each line
        struct movies *newNode = createMovies(currLine);

        if(head == NULL){

            // Set first node and tail 
            head = newNode;
            tail = newNode;
        }
        else{

            // Tail becomes next new node
            tail->next = newNode;
            tail = newNode;
        }
    }

    free(currLine);
    fclose(moviesFile);

    return head;
}

/********************************************************************* 
** Function: goToOption
** Description: Helper function to call functions associated with the options.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
**      int choice - User entered number for options choice.
** Pre-Conditions: Given a linked list and users choice.
** Post-Conditions: Calls functions associated with users choice.
*********************************************************************/
void goToOption(struct movies *head, int choice){
    if(choice == 1){
        int year = getYear();
        searchMoviesinYear(head, year);
    }
    else if(choice == 2){
        searchHighestRatedinYear(head);
    }
    else if(choice == 3){
        char* language = getLang();
        searchMoviesbyLang(head, language);
    }
    else{
        return;
    }
}

/********************************************************************* 
** Function: displayOptions
** Description: Repeatedly displays the options.
** Parameters: 
**      struct movies *head - Pointer to the start of the list
** Pre-Conditions: Given a linked list.
** Post-Conditions: Displays options until user enters 4 to exit program.
*********************************************************************/
void displayOptions(struct movies *head){
    int choice = 0;

    while(choice != 4)
    {
        printf("1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n");
        printf("\n");
        
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &choice);
        printf("\n");

        if(choice < 1 || choice > 4){
            printf("Invalid choice. Choose between 1-4.\n");
        }
        else{
            goToOption(head, choice);
        }
    }

    printf("Goodbye!\n");
}

/********************************************************************* 
** Function: freeList 
** Description: Function to free all nodes in a linked list.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
** Pre-Conditions: Given a linked list
** Post-Conditions: Frees nodes associated with movies LL.
*********************************************************************/
void freeList(struct movies *head){

    struct movies* current = head;
    struct movies* temp = NULL;

    while (current != NULL){
        temp = current->next;

        free(current->title);
        free(current->year);
        free(current->language);
        free(current->rating);
        free(current);

        current = temp;
    }

    free(current);
}

/********************************************************************* 
** Function: displayMovieCount
** Description: Displays file name and movie count.
** Parameters: 
**      struct movies *head - Pointer to the start of the list.
**      char *filePath - String that contains file name
** Pre-Conditions: Given a linked list and file name.
** Post-Conditions: Displays the file name and parsed movies.
*********************************************************************/
void displayMovieCount(struct movies *head, char *filePath){
    struct movies *temp = head;
    int count = 0;
    
    while(temp->next != NULL){
        if (temp->title != NULL){
            count++;
        }
        temp = temp->next;
    }
    printf("\n");
    printf("Processed file %s and parsed data for %d movies.\n", filePath, count);
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_info.csv\n");
        return EXIT_FAILURE;
    }
    struct movies *list = processFile(argv[1]);

    displayMovieCount(list, argv[1]);
    
    displayOptions(list);

    freeList(list);

    return EXIT_SUCCESS;
}