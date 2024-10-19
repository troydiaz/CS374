#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movies.h"
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>

/* Struct for movies information */
struct movies
{
    char *title;
    int *year;
    char *language;
    double *rating;
    struct movies *next;
};

int checkPrefix(char *fileName){
    char *filePrefix = "movies_";

    if(strstr(fileName, filePrefix) == fileName){
        return 1;
    }
    else{
        return 0;
    }
}

void getLargestFile(){
    // Pointer to file data
    struct stat st;

    // Pointer to directory
    struct dirent *de;

    char *largestFile = NULL;
    long int currFileSize = 0;

    // Open current working directory
    DIR *dir = opendir(".");

    if (dir == NULL){
        printf("Could not open current directory!\n\n");
    }

    // Traverse working directory for specific prefix & suffix
    while((de = readdir(dir)) != NULL){
        char *fileName = de->d_name;
        if(checkPrefix(fileName) == 1){
            // Store largest file name and size
            if(stat(fileName, &st) == 0){
                if(st.st_size >= currFileSize){
                    currFileSize = st.st_size;
                    largestFile = de->d_name;
                }
            }
        }
    }

    if (largestFile != NULL){
        // Process largest file
        printf("The file name: %s\n", largestFile);
        printf("The size in bytes: %ld\n\n", currFileSize);
        struct movies *head = processFile(largestFile);
        createDir(head);
        freeList(head);

    }
    else{
        printf("No files were found!\n\n");
    }

    // Close working directory
    closedir(dir);

    return;
}

void getSmallestFile(){
    // Pointer to file data
    struct stat st;

    // Pointer to directory
    struct dirent *de;

    char *smallestFile = NULL;
    long int currFileSize = LONG_MAX;

    // Open current working directory
    DIR *dir = opendir(".");

    if (dir == NULL){
        printf("Could not open current directory!\n\n");
    }

    // Traverse working directory for specific prefix & suffix
    while((de = readdir(dir)) != NULL){
        char *fileName = de->d_name;
        if(checkPrefix(fileName) == 1){
            // Store largest file name and size
            if(stat(fileName, &st) == 0){
                if(st.st_size < currFileSize){
                    currFileSize = st.st_size;
                    smallestFile = de->d_name;
                }
            }
        }
    }

    if (smallestFile != NULL){
        // Process largest file
        printf("The file name: %s\n", smallestFile);
        printf("The size in bytes: %ld\n\n", currFileSize);
        struct movies *head = processFile(smallestFile);
        createDir(head);
        freeList(head);
    }
    else{
        printf("No files were found!\n\n");
    }

    // Close working directory
    closedir(dir);
    return;
}

void clearInputBuffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF){}
}

void getSpecificFile(){
    size_t size = 0;
    char* fileName = NULL;
    
    clearInputBuffer();

    // Read full line
    printf("Enter the complete file name: ");
    getline(&fileName, &size, stdin);

    // Remove newline character at end of string
    fileName[strcspn(fileName, "\n")] = 0;

    // Open filename for reading
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        // If file not found
        printf("The file %s was not found. Try again.\n\n", fileName);
    }
    else{
        // Close file and parse
        fclose(file);
        printf("Now processing the chosen file named %s\n", fileName);
        struct movies *head = processFile(fileName);

        createDir(head);

        freeList(head);
    }

    // Free getline
    free(fileName);
    return;
}

void createDir(struct movies *head){
    // Create directory name
    char dirName[256];
    int randNum = rand() % 100000;
    sprintf(dirName, "diaztr.movies.%d", randNum);

    // Create new directory with rwxr-x--- permissions
    if(mkdir(dirName, 0750)==0){
        createFiles(head, dirName);
        printf("Created directory with name %s\n\n", dirName);
    }
    else{
        // Directory already exists
        printf("Error: directory with that name already exists!\n\n");
    }
    
    return;
}

void createFiles(struct movies *head, char* dirName){
    struct movies *temp = head;
    char filePath[300];
    FILE *file;

    while(temp != NULL){
        // i.e. dirName/yr.txt
        sprintf(filePath, "%s/%d.txt", dirName, *(temp->year));

        // Open in append mode
        file = fopen(filePath, "a");
        if(file == NULL){
            printf("Error opening file: %s\n", filePath);
        }
        // Write to file 
        fprintf(file, "%s\n", temp->title);
        fclose(file);

        // Set file permissions to rw-r-----
        chmod(filePath, 0640);
        temp = temp->next;
    }
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

    if (moviesFile == NULL){
        printf("Error opening file: %s\n", filePath);
        return NULL;
    }

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;

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

void selectFileOptions(){
    int choice = 1;

    while(choice != 4){
        printf("Enter 1 to pick the largest file\n");
        printf("Enter 2 to pick the smallest file\n");
        printf("Enter 3 to specify the name of a file\n");
        printf("Enter 4 to go back to the main menu\n");

        printf("\nEnter a choice from 1 to 4: ");
        scanf("%d", &choice);

        if (choice < 1 || choice > 4){
            printf("Invalid choice. Choose between 1-4.\n");
        }
        else if(choice == 1){
            getLargestFile();
        }
        else if(choice == 2){
            getSmallestFile();
        }
        else if(choice == 3){
            getSpecificFile();
        }
        else if (choice == 4) {
            break;
        }
    }
}

void displayOptions(){
    int choice = 0;

    while(choice != 2)
    {
        printf("1. Select file to process\n");
        printf("2. Exit the program\n");
        printf("\n");
        
        printf("Enter a choice from 1 to 2: ");
        scanf("%d", &choice);
        printf("\n");

        if(choice == 1){
            selectFileOptions();
        }
        else if(choice == 2){
            printf("Goodbye!\n");
            break;
        }
        else {
            printf("Invalid choice. Choose between 1-2.\n");
        }
    }
}
