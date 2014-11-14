/** @file test1.c
 *
 * @test files
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

unsigned long startTime;
int gotMoney = 0;

char topLine []=          "|v-----------------|\n\0"; //row 0
char middleline1 []=      "|@o$  o            |\n\0"; //row 1
char middleline2 []=      "|        o         |\n\0"; //row 2
char middleline3 []=      "|         o        |\n\0"; //row 3
char middleline4 []=      "|o              o  |\n\0";
char middleline5 []=      "| o      o         |\n\0";
char bottomLine []=       "|------------------|\n\0";

int row = 1;
int col = 1;

int movePiece(char direction){
    putchar('\n');putchar('\n');putchar('\n');putchar('\n');putchar('\n');
    putchar('\n');putchar('\n');putchar('\n');putchar('\n');putchar('\n');
    putchar('\n');putchar('\n');putchar('\n');putchar('\n');putchar('\n');
    putchar('\n');putchar('\n');putchar('\n');putchar('\n');putchar('\n');
    write(STDOUT_FILENO, (const void *) topLine, 21);
    write(STDOUT_FILENO, (const void *) middleline1, 21);
    write(STDOUT_FILENO, (const void *) middleline2, 21);
    write(STDOUT_FILENO, (const void *) middleline3, 21);
    write(STDOUT_FILENO, (const void *) middleline4, 21);
    write(STDOUT_FILENO, (const void *) middleline5, 21);
    write(STDOUT_FILENO, (const void *) bottomLine, 21);
    
    char nextChar = ' ';
    int colNext = col;
    int rowNext = row;

    switch(direction){
        case('d'):
            colNext = col + 1;
            break;
        case('a'):
            colNext = col - 1;
            break;
        case('s'):
            rowNext = row + 1;
            break;
        case('w'):
            rowNext = row - 1;
            break;
    }

    switch(rowNext){
        case(0):
            nextChar = topLine[colNext];
            if(nextChar == 'v' && gotMoney == 1){
                unsigned long timeElapsed = time() - startTime;
                printf("You win! It took you %ld milliseconds!", timeElapsed);
                putchar('\n');
                printf("Thank you for playing!");
                return 1;
            }
            if(nextChar != ' '){
                return 1;
            }
            break;
        case(1):
            nextChar = middleline1[colNext];
            if(nextChar == '$'){
                middleline2[col] = ' ';
                middleline1[col] = '@';
                printf("You got the money! Now go back to where you started!");
                putchar('\n');
                gotMoney = 1;
            }
            else if(nextChar != ' '){
                putchar(nextChar);
                return 1;
            }else{
                switch(direction){
                    case('w'):
                        middleline2[col] = ' ';
                        middleline1[col] = '@';
                        break;
                    default:
                        middleline1[col] = ' ';
                        middleline1[colNext] = '@';
                }
            }
            break;
        case(2):
            nextChar = middleline2[colNext];
            if(nextChar != ' '){
                putchar(nextChar);
                return 1;
            }else{
                switch(direction){
                    case('w'):
                        middleline3[col] = ' ';
                        middleline2[col] = '@';
                        break;
                    case('s'):
                        middleline1[col] = ' ';
                        middleline2[col] = '@';
                        break;
                    default:
                        middleline2[col] = ' ';
                        middleline2[colNext] = '@';
                }
            }
            break;
        case(3):
            nextChar = middleline3[colNext];
            if(nextChar != ' '){
                putchar(nextChar);
                return 1;
            }else{
                switch(direction){
                    case('w'):
                        middleline4[col] = ' ';
                        middleline3[col] = '@';
                        break;
                    case('s'):
                        middleline2[col] = ' ';
                        middleline3[col] = '@';
                        break;
                    default:
                        middleline3[col] = ' ';
                        middleline3[colNext] = '@';
                }
            }
            break;
        case(4):
            nextChar = middleline4[colNext];
            if(nextChar != ' '){
                putchar(nextChar);
                return 1;
            }else{
                switch(direction){
                    case('w'):
                        middleline5[col] = ' ';
                        middleline4[col] = '@';
                        break;
                    case('s'):
                        middleline3[col] = ' ';
                        middleline4[col] = '@';
                        break;
                    default:
                        middleline4[col] = ' ';
                        middleline4[colNext] = '@';
                }
            }
            break;
        case(5):
            nextChar = middleline5[colNext];
            if(nextChar != ' '){
                putchar(nextChar);
                return 1;
            }else{
                switch(direction){
                    case('s'):
                        middleline4[col] = ' ';
                        middleline5[col] = '@';
                        break;
                    default:
                        middleline5[col] = ' ';
                        middleline5[colNext] = '@';
                }
            }
            break;
        case(6):
            nextChar = bottomLine[colNext];
            if(nextChar != ' '){
                return 1;
            }
            break;
    }

        switch(direction){
        case('d'):
            col = col + 1;
            break;
        case('a'):
            col = col - 1;
            break;
        case('s'):
            row = row + 1;
            break;
        case('w'):
            row = row - 1;
            break;
    }

    return 0;
}

int main(int argc, char** argv)
{

	ssize_t read_count = 2;
    char start_buf[BUFFER_SIZE];
    char input_buf[BUFFER_SIZE];
    char direction = 'a'; //w up, d right, s down, a left
    int stuck = 0;

    putchar('\n');
    putchar('\n');
    printf("Press 'i' followed by 'Enter' for instructions, otherwise just hit 'Enter' to start");
    putchar('\n');

    read(STDIN_FILENO, start_buf, BUFFER_SIZE);
    if(start_buf[0] == 'i'){
        printf("You are the @ character...");
        putchar('\n');
        sleep(2100);
        printf("You're sliding on ice, but walls and the 'o' blocks will stop you");
        putchar('\n');
        sleep(3100);
        printf("Your objective is to grab the money.");
        putchar('\n');
        sleep(2100);
        printf("Use standard gaming WASD keys to move up left down right respectively");
        putchar('\n');
        sleep(3100);
        printf("*****Be sure to hit 'Enter' after you type in your key to move*****");
        putchar('\n');
        sleep(2100);
        putchar('\n');
        printf("Press 'Enter' when you're ready");
        putchar('\n');
        read(STDIN_FILENO, start_buf, BUFFER_SIZE);
    }
    
    printf("Your game starts in 2 seconds");
    putchar('\n');
    putchar('\n');
    sleep(2000);
    startTime = time();
    while (1) {
        
        sleep(200);
        if (stuck){
            read_count = read(STDIN_FILENO, input_buf, BUFFER_SIZE);
            direction = input_buf[0];
        }
        stuck = movePiece(direction);

        
    }
	return 0;
}
