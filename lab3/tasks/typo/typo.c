/** @file typo.c
 *
 * @brief Echos characters back with timing data.
 * @author Marc-Daniel Julien <mjulien@andrew.cmu.edu>
 * @author Jeffery He <jmhe@andrew.cmu.edu>
 *
 * Links to libc.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define MAX_BUFFER 1024

 /* String length excluding null character. */
int string_length(char* yo) {
    int i = 0;
    while (i < MAX_BUFFER && yo[i] != 0) {
        i++;
    }
    return i;
}

int main(int argc, char** argv)
{

	ssize_t read_count;
    char input_buf[BUFFER_SIZE];

    const char prompt_message []= "Let's see how fast you can type. START NOW!.\n\0";
    while (1) {
    	/* Display the prompt, this is when the timer starts */
    	write(STDOUT_FILENO, (const void *) prompt_message, sizeof(prompt_message)-1);

    	//Start the timer
    	unsigned long startTime = time();
        /* Read a chunk of size BUFFER_SIZE from STDIN */
        read_count = read(STDIN_FILENO, input_buf, BUFFER_SIZE);

		//calculating time it took to type message
        unsigned long timeElapsed = time() - startTime;

        // Error, return 1 -> exit with status 1
        if(read_count <= 0) { return 1; }

        // No input -> just carriage return character, terminate with 0 (success).
        if(read_count == 1) { return 0; }
        
        // Displays the time it took to type the message
        //if (write(STDOUT_FILENO, (const void *)timeElapsedStr, sizeof(timeElapsedStr)-1 ) < 0) {
        //    return 1;
        //}

        printf("The time it took you to write your message was %ld milliseconds", (unsigned long)timeElapsed);
        putchar('\n');
        
    }
	return 0;
}
