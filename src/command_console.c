#include "./../include/command_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

// Buffer to store the string to write to the log file
char log_buffer[100];

// File descriptor for the log file
int log_fd;

// Variable to store the value of the write function
int check;

// Function to check the correctness of the pipe
void CheckCorrectness(int c) 
{
    if(c == -1) 
    {
        close(log_fd);
        perror("Error in writing function");
        exit(1);
    }
}

// Function to send the velocity to the respective motor
void send_velocity(const char * myfifo, int v)
{
    int fd;

    if((fd = open(myfifo, O_WRONLY))==-1)
    {
        close(log_fd);
        close(fd);
        perror("Error opening fifo");
        exit(1);
    }

    check = write(fd, &v, sizeof(v));
    CheckCorrectness(check);
    close(fd);  
}

int main(int argc, char const *argv[])
{
    // Open the log file
    if ((log_fd = open("command.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1)
    {
        // If the file could not be opened, print an error message and exit
        perror("Error opening command file");
        exit(1);
    }

    // Variable useful for resizing the screen
    int first_resize = TRUE;

    // Initialize User Interface 
    init_console_ui();

    // Creation of the FIFOs
    const char * vx_fifo = "/tmp/vx_fifo";
    const char * vz_fifo = "/tmp/vz_fifo";
    mkfifo(vx_fifo, 0666);
    mkfifo(vz_fifo, 0666);

    // Variable declaration
    int fd;
    
    // Variable declaration in order to get the time
    time_t rawtime;
    struct tm *info;
    
    // Infinite loop
    while(TRUE)
	{	
        // Get current time
        time(&rawtime);
        info = localtime(&rawtime);

        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();
        
        // If user resizes screen, re-draw UI
        if(cmd == KEY_RESIZE) 
        {
            if(first_resize) 
            {
                first_resize = FALSE;
            }

            else 
            {
                reset_console_ui();
            }
        }

        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) 
        {
            // Check which button has been pressed...
            if(getmouse(&event) == OK) 
            {
                // Vx-- button pressed
                if(check_button_pressed(vx_decr_btn, &event)) 
                {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Decreased");
                    
                    // Write to the log file
                    sprintf(log_buffer, "<command_console> Vx button decreased pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);

                    // Send the velocity to the motor
                    send_velocity(vx_fifo,0);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) 
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vx++ button pressed
                else if(check_button_pressed(vx_incr_btn, &event)) 
                {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Increased");

                    // Write to the log file
                    sprintf(log_buffer, "<command_console> Vx  button increased  pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);

                    // Send the velocity to the motor
                    send_velocity(vx_fifo,1);
                    
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) 
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vx stop button pressed
                else if(check_button_pressed(vx_stp_button, &event)) 
                {
                    mvprintw(LINES - 1, 1, "Horizontal Motor Stopped");

                    // Write to the log file
                    sprintf(log_buffer, "<command_console> Vx button stopped  pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);

                    // Send the velocity to the motor
                    send_velocity(vx_fifo,2);
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) 
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vz-- button pressed
                else if(check_button_pressed(vz_decr_btn, &event)) 
                {
                    mvprintw(LINES - 1, 1, "Vertical Speed Decreased");

                    // Write to the log file
                    sprintf(log_buffer, "<command_console> Vz button decreased  pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);

                    // Send the velocity to the motor
                    send_velocity(vz_fifo,0);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) 
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vz++ button pressed
                else if(check_button_pressed(vz_incr_btn, &event)) 
                {
                    mvprintw(LINES - 1, 1, "Vertical Speed Increased");

                    // Write to the log file
                    sprintf(log_buffer, "<command_console> Vz button increased pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);

                    // Send the velocity to the motor
                    send_velocity(vz_fifo,1);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) 
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // Vz stop button pressed
                else if(check_button_pressed(vz_stp_button, &event)) 
                {
                    mvprintw(LINES - 1, 1, "Vertical Motor Stopped");

                    // Write to the log file
                    sprintf(log_buffer, "<command_console> Vz button stopped  pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);

                    // Send the velocity to the motor
                    send_velocity(vz_fifo,2);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) 
                    {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        refresh();
	}

    // Terminate
    endwin();

    // Close the log file
    close(log_fd);

    return 0;
}
