/*********************************************************************** 
** Program Filename: main.c
** Author: Troy Diaz
** Date: 10/26/24
** Description: Program that creates 4 thread to process input from standard input.
** Utilizes threads, mutual exclusion and condition variables. Input thread reads lines 
** of characters from standard input. Line separator thread processes this input by 
** replacing every line separtor with a space. Plus sign thread replaces each pair 
** of plus signs with the caret symbol. And the output thread formats each processed 
** data into lines of 80 characters. 
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_BUFFER 1000         // Max input line length   
#define LINE_LENGTH 80          // Numbers of character per output line
#define BUFFER_SIZE 50          // Number of lines per buffer

// Buffer 1 - hold lines read by input thread
char buffer1[BUFFER_SIZE][MAX_BUFFER];
int buffer1_count = 0; // Number of items in buffer 1
int buffer1_in = 0; // Index of next item added in buffer 1
int buffer1_out = 0; // Index of next item removed in buffer 1

// Buffer 2 - hold lines read by line separator processor
char buffer2[BUFFER_SIZE][MAX_BUFFER];
int buffer2_count = 0;
int buffer2_in = 0;
int buffer2_out = 0;

// Buffer 3 - hold lines read by plus sign thread processor
char buffer3[BUFFER_SIZE][MAX_BUFFER];
int buffer3_count = 0;
int buffer3_in = 0; 
int buffer3_out = 0;

// Buffer 1 Mutex 
pthread_mutex_t buffer1_mutex = PTHREAD_MUTEX_INITIALIZER; // Locks buffer 1
pthread_cond_t buffer1_not_empty = PTHREAD_COND_INITIALIZER; // Not empty signal
pthread_cond_t buffer1_not_full = PTHREAD_COND_INITIALIZER; // Not full signal

// Buffer 2 Mutex 
pthread_mutex_t buffer2_mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t buffer2_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer2_not_full = PTHREAD_COND_INITIALIZER;

// Buffer 3 Mutex 
pthread_mutex_t buffer3_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer3_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer3_not_full = PTHREAD_COND_INITIALIZER;

// Terminate processors flag
int stop_processing = 0;

/********************************************************************* 
** Function: thread_input_reader
** Description: Reads and writes input to Buffer 1.
** Parameters:
**      void *arg - pthreads function signature.
** Pre-Conditions: Buffer 1 is initialized.
** Post-Conditions: Lines are processed to Buffer 1 until termination sign.
*********************************************************************/
void *thread_input_reader(void *arg);

/********************************************************************* 
** Function: thread_line_separator
** Description: Processes Buffer 1 data, replaces newline characters with
** space delimiters, and is read/written to Buffer 2.
** Parameters: 
**      void *arg - pthreads function signature.
** Pre-Conditions: Buffer 1 has data and Buffer 2 is initialized.
** Post-Conditions: Lines are processed to Buffer 2.
*********************************************************************/
void *thread_line_separator(void *arg);

/********************************************************************* 
** Function: thread_plus_sign_processor
** Description: Processes Buffer 2 data, replaces ++ string with ^ character,
** and is read/written to Buffer 3.
** Parameters: 
**      void *arg - pthreads function signature.
** Pre-Conditions: Buffer 2 has data and Buffer 3 is initialized.
** Post-Conditions: Lines are processed to Buffer 3.
*********************************************************************/
void *thread_plus_sign_processor(void *arg);

/********************************************************************* 
** Function: thread_output_writer
** Description: Processes Buffer 3 data, limits each line to be 80 characters,
** writes to sdout. 
** Parameters: 
**      void *arg - pthreads function signature.
** Pre-Conditions: Buffer 3 has data. 
** Post-Conditions: Lines are processed from Buffer 3 and written.
*********************************************************************/
void *thread_output_writer(void *arg);

void *thread_input_reader(void *arg) {
    // Store input line
    char line[MAX_BUFFER]; 

    // Read lines until termination sign
    while (fgets(line, MAX_BUFFER, stdin)) {
        
        // Check for "STOP"
        if (strncmp(line, "STOP", 4) == 0 && (line[4] == '\n' || line[4] == '\0' || line[4] == '!')) {
            // Lock Buffer 1 mutex
            pthread_mutex_lock(&buffer1_mutex);
            stop_processing = 1;

            // Signal waiting flags
            pthread_cond_broadcast(&buffer1_not_empty); 

            // Unlock mutex
            pthread_mutex_unlock(&buffer1_mutex);

            break;
        }

        pthread_mutex_lock(&buffer1_mutex);

        // Wait for not empty signal in Buffer 1
        while (buffer1_count == BUFFER_SIZE) {
            pthread_cond_wait(&buffer1_not_full, &buffer1_mutex);
        }

        // Store Buffer 1 data
        strncpy(buffer1[buffer1_in], line, MAX_BUFFER);
        buffer1_in = (buffer1_in + 1) % BUFFER_SIZE;
        buffer1_count++;

        // Set Buffer 1 empty signal to be not empty
        pthread_cond_signal(&buffer1_not_empty); 
        pthread_mutex_unlock(&buffer1_mutex);
    }

    return NULL;
}

void *thread_line_separator(void *arg) {
    char line[MAX_BUFFER];

    // Process Buffer 1 data
    while (1) {
        // Lock Buffer 1 mutex
        pthread_mutex_lock(&buffer1_mutex);

        // Wait for not empty signal in Buffer 1
        while (buffer1_count == 0 && !stop_processing) {
            pthread_cond_wait(&buffer1_not_empty, &buffer1_mutex);
        }

        // Exit if Buffer 1 is empty
        if (stop_processing && buffer1_count == 0) {
            // Unlock mutex
            pthread_mutex_unlock(&buffer1_mutex);
            break;
        }

        // Read Buffer 1 Data
        strncpy(line, buffer1[buffer1_out], MAX_BUFFER);
        buffer1_out = (buffer1_out + 1) % BUFFER_SIZE;
        buffer1_count--;

        // Set Buffer 1 not full signal
        pthread_cond_signal(&buffer1_not_full); 
        pthread_mutex_unlock(&buffer1_mutex);

        // Replace newline char with spaces
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '\n') {
                line[i] = ' ';
            }
        }

        // Lock Buffer 2 mutex
        pthread_mutex_lock(&buffer2_mutex);

        // Wait for not empty signal in Buffer 2
        while (buffer2_count == BUFFER_SIZE) {
            pthread_cond_wait(&buffer2_not_full, &buffer2_mutex);
        }

        // Write data to Buffer 2
        strncpy(buffer2[buffer2_in], line, MAX_BUFFER);
        buffer2_in = (buffer2_in + 1) % BUFFER_SIZE;
        buffer2_count++;

        // Set Buffer 2 signal is not empty
        pthread_cond_signal(&buffer2_not_empty); 
        pthread_mutex_unlock(&buffer2_mutex);
    }

    return NULL;
}

void *thread_plus_sign_processor(void *arg) {
    char line[MAX_BUFFER]; 

    // Process Buffer 2 data
    while (1) {
        // Lock Buffer 2 mutex
        pthread_mutex_lock(&buffer2_mutex);

        // Wait for not empty signal in Buffer 2
        while (buffer2_count == 0 && !stop_processing) {
            pthread_cond_wait(&buffer2_not_empty, &buffer2_mutex);
        }

        // Exit if Buffer 2 is empty
        if (stop_processing && buffer2_count == 0) {
            pthread_mutex_unlock(&buffer2_mutex);
            break;
        }  

        // Read data from Buffer 2
        strncpy(line, buffer2[buffer2_out], MAX_BUFFER);
        buffer2_out = (buffer2_out + 1) % BUFFER_SIZE;
        buffer2_count--;

        // Set Buffer 2 signal is not full
        pthread_cond_signal(&buffer2_not_full); 
        pthread_mutex_unlock(&buffer2_mutex);

        // Replace every ++ with ^
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '+' && line[i + 1] == '+') {
                line[i] = '^';
                memmove(&line[i + 1], &line[i + 2], strlen(&line[i + 2]) + 1);
            }
        }

        // Lock Buffer 3
        pthread_mutex_lock(&buffer3_mutex);

        // Wait for not empty signal in Buffer 3
        while (buffer3_count == BUFFER_SIZE) {
            pthread_cond_wait(&buffer3_not_full, &buffer3_mutex);
        }

        // Write data to Buffer 3
        strncpy(buffer3[buffer3_in], line, MAX_BUFFER);
        buffer3_in = (buffer3_in + 1) % BUFFER_SIZE;
        buffer3_count++;
        
        // Set signal in Buffer 3 that it is not empty
        pthread_cond_signal(&buffer3_not_empty);
        pthread_mutex_unlock(&buffer3_mutex);
    }

    return NULL;
}

void *thread_output_writer(void *arg) {
    char line[MAX_BUFFER]; 

    // Buffer for 80 character output line
    char output_line[LINE_LENGTH + 1];
    int count = 0; 

    // Null terminate Buffer
    output_line[LINE_LENGTH] = '\0'; 

    // Process Buffer 3 data
    while (1) {
        // Lock Buffer 3
        pthread_mutex_lock(&buffer3_mutex);

        // Wait for not empty signal in Buffer 3
        while (buffer3_count == 0 && !stop_processing) {
            pthread_cond_wait(&buffer3_not_empty, &buffer3_mutex);
        }

        // Exit when Buffer 3 is empty
        if (stop_processing && buffer3_count == 0) {
            pthread_mutex_unlock(&buffer3_mutex);
            break;
        }

        // Read Buffer 3 data
        strncpy(line, buffer3[buffer3_out], MAX_BUFFER);
        buffer3_out = (buffer3_out + 1) % BUFFER_SIZE;
        buffer3_count--;

        // Set Signal that Buffer 3 is not full
        pthread_cond_signal(&buffer3_not_full);
        pthread_mutex_unlock(&buffer3_mutex);

        // Write each line in 80 character lines to sdout
        for (int i = 0; line[i] != '\0'; i++) {
            output_line[count++] = line[i];
            if (count == LINE_LENGTH) {
                printf("%s\n", output_line);
                count = 0; 
            }
        }
    }

    // Last partial line
    if (count > 0) {
        output_line[count] = '\0';
        printf("%s\n", output_line);
    }

    return NULL;
}

int main() {
    // Thread identifiers
    pthread_t input_thread, line_separator_thread, plus_sign_thread, output_thread;

    // Create input thread
    if (pthread_create(&input_thread, NULL, thread_input_reader, NULL) != 0) {
        fprintf(stderr, "Error creating input thread.\n");
        exit(EXIT_FAILURE);
    }

    // Create line separator thread
    if (pthread_create(&line_separator_thread, NULL, thread_line_separator, NULL) != 0) {
        fprintf(stderr, "Error creating line separator thread.\n");
        exit(EXIT_FAILURE);
    }

    // Create plus sign processor thread
    if (pthread_create(&plus_sign_thread, NULL, thread_plus_sign_processor, NULL) != 0) {
        fprintf(stderr, "Error creating plus sign processor thread.\n");
        exit(EXIT_FAILURE);
    }

    // Create output thread
    if (pthread_create(&output_thread, NULL, thread_output_writer, NULL) != 0) {
        fprintf(stderr, "Error creating output thread.\n");
        exit(EXIT_FAILURE);
    }

    // Wait until input thread finishes
    pthread_join(input_thread, NULL);

    // Wait until line separator thread finishes
    pthread_join(line_separator_thread, NULL);

    // Wait until plus sign processor thread finishes
    pthread_join(plus_sign_thread, NULL);

    // Wait until output thread finishes
    pthread_join(output_thread, NULL);

    return 0;
}