/*
 *
 * connect.c
 * 
 * Assignment 4
 * CS 360
 * 
 * Author: JT Mundi
 * 
 * Date Created: 02/22/19
 * Last Modified: 02/24/19
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define COLON ":"
#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2
#define READ 0
#define WRITE 1

/* 
 * void errorH(int error)
 * 
 * Handle function erros for specified error number.
 * Print out error message along with error number
 * using errno.h library. 
 * 
 * Exit the program with exit failuire. 
 * 
 */
void errorH(int error)
{
    switch (error)
    {

    //Invalid number of arguments in terminal
    case 1:
        fprintf(stderr, "Invalid arguments | Error Number %d : %s \n", errno, strerror(errno));
        fprintf(stderr, "Interface: ./connect <arg1> : <arg2>\n");
        exit(EXIT_FAILURE);
        break;

    //Invalid use of colon to seperation commands
    case 2:
        fprintf(stderr, "Invalid colon locaiton | Error Number %d : %s \n", errno, strerror(errno));
        fprintf(stderr, "Interface: ./connect <arg1> : <arg2>\n");
        exit(EXIT_FAILURE);
        break;

    //Invalid number of colons, can only use one
    case 3:
        fprintf(stderr, "Too many colons | Error Number %d : %s \n", errno, strerror(errno));
        fprintf(stderr, "Interface: ./connect <arg1> : <arg2>\n");
        exit(EXIT_FAILURE);
        break;

    //Error in creating a fork
    case 4:
        fprintf(stderr, "Fork error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
        break;

    //Error in creating a process pipe
    case 5:
        fprintf(stderr, "Pipe error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
        break;

    //Error while executing the command
    case 6:
        fprintf(stderr, "Execvp error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
        break;

    //Error while duplicating fd stream
    case 7:
        fprintf(stderr, "Dup error | Error Number %d : %s \n", errno, strerror(errno));
        exit(EXIT_FAILURE);
        break;

    //Default error handler
    default:
        fprintf(stderr, "Error | Error Number %d : %s \n", errno, strerror(errno));
        fprintf(stderr, "Interface: ./connect <arg1> : <arg2>\n");
        exit(EXIT_FAILURE);
        break;
    }
}
int main(int argc, char const *argv[])
{
    //Make sure atleast one argument present
    if (argc <= 1)
    {
        errorH(1);
    }

    int colonFlag = 0;
    int colonCount = 0;
    int colonLocation = 0;
    int argOne = 1;
    int argTwo = 0;

    /* Parse in the provided arguments
     * look for colon seperator. Error check
     * for the number of colons and location of the
     * colon in the arguments.
     * 
     * Index the colon location and set the colon to
     * NULL for parsing later in execvp()
     */
    for (size_t i = 0; i < argc; i++)
    {

        //String compare for ":"
        if ((strcmp(argv[i], COLON) == 0))
        {
            colonFlag = 1;
            ++colonCount;

            //Colon count check
            if (colonCount > 1)
            {
                errorH(3);
            }

            //Check for colon location
            if ((i == 0) || (i == argc - 1) || (i == 1))
            {
                errorH(2);
            }

            //Update colon index location
            colonLocation = i;
            argTwo = i + 1;
            argv[i] = NULL;
        }
    }

    pid_t childPID;
    int pipeFD[2];

    /* Pipe a new process pipeFD 
     * pipeFD[0] for reading
     * pipeFD[1] for writing
     * Perform error check
     */
    if (pipe(pipeFD) == -1)
    {
        errorH(5);
    }

    /* 
     * Fork a new process
     * and set child pid.
     * Perform error check.
     */
    childPID = fork();

    if (childPID == -1)
    {
        errorH(4);
    }

    // Child Process
    if (childPID == 0)
    {
        close(STD_IN);

        //Duplicate the read stream
        if (dup(pipeFD[READ]) == -1)
        {
            errorH(7);
        }

        //Close the read and write descriptors
        close(pipeFD[WRITE]);
        close(pipeFD[READ]);

        //Execute argument 2, kill if any error
        if (execvp(argv[argTwo], &argv[argTwo]) == -1)
        {
            kill(childPID, SIGKILL);
            errorH(6);
        }
    }

    else
    {
        close(STD_OUT);

        //Duplicate the write stream
        if (dup(pipeFD[WRITE]) == -1)
        {
            errorH(7);
        }

        //Close read and write file descriptors 
        close(pipeFD[READ]);
        close(pipeFD[WRITE]);

        //Execute argument 1, kill if any error
        if (execvp(argv[argOne], &argv[argOne]) == -1)
        {
            kill(childPID, SIGKILL);
            errorH(6);
        }
    }

    return 0;
}