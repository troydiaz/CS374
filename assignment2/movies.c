#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movies.h"

/* Struct for movies information */
struct movies
{
    char *title;
    int *year;
    char *language;
    double *rating;
    struct movies *next;
};

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

    if(flag = 0){
        printf("No data about movies released in %s\n", language);
    }
    printf("\n");
}

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

int getYear(){
    int year = 0;

    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &year);

    return year;
}

char* getLang(){
    char* language;

    printf("Enter the language for which you want to see movies: ");
    scanf("%s", language);

    return language;
}

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