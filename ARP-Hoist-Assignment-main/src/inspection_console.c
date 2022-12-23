#include "./../include/inspection_utilities.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
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

int main(int argc, char const *argv[])
{
    // Open the log file
    if ((log_fd = open("inspection.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1)
    {
        perror("Error opening inspection log file");
        return 1;
    }

    int first_resize = TRUE;

    // End-effector coordinates
    float ee_x, ee_y;
    ee_x = 0.0;
    ee_y = 0.0; 

    // Initialize User Interface 
    init_console_ui();

    const char * real_x_fifo = "/tmp/real_x_fifo";
    const char * real_z_fifo = "/tmp/real_z_fifo";

    mkfifo(real_x_fifo, 0666);
    mkfifo(real_z_fifo, 0666);

    int fd;
    int fd2;

    pid_t pid_motor_x = atoi(argv[1]);
    pid_t pid_motor_z = atoi(argv[2]); 

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
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) {

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // STOP button pressed
                if(check_button_pressed(stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "STP button pressed");
                    
                    // Write to the log file
                    sprintf(log_buffer, "<Inspection_console> STOP button pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);  

                    kill(pid_motor_x, SIGUSR1);
                    kill(pid_motor_z, SIGUSR1);

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // RESET button pressed
                else if(check_button_pressed(rst_button, &event)) {
                    mvprintw(LINES - 1, 1, "RST button pressed");
                    
                    // Write to the log file
                    sprintf(log_buffer, "<Inspection_console> RESET button pressed: %s\n", asctime(info));
                    check = write(log_fd, log_buffer, strlen(log_buffer));
                    CheckCorrectness(check);  

                    kill(pid_motor_x, SIGUSR2); 
		            kill(pid_motor_z, SIGUSR2);
                    
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }
    
    // Open the FIFOs
    if ((fd = open(real_x_fifo, O_RDONLY))==-1)
    {
        close(log_fd);
        close(fd);
        perror("Error opening fifo");
        exit(1);
    }

    // Read the FIFOs from World
    if(read(fd, &ee_x, sizeof(ee_x)) == -1)
    {
        close(log_fd);
        close(fd);
        perror("Error reading fifo");
        exit(1);
    }

    close(fd);

    // Open the FIFOs
    if ((fd2 = open(real_z_fifo, O_RDONLY))==-1)
    {
        close(log_fd);
        close(fd);
        perror("Error opening fifo");
        exit(1);
    }
    
    // Read the FIFOs from World
    if(read(fd2, &ee_y, sizeof(ee_y)) == -1)
    {
        close(log_fd);
        close(fd);
        perror("Error reading fifo");
        exit(1);
    }

    close(fd2);

    update_console_ui(&ee_x,&ee_y);

    sleep(1);
    }
    
    close(log_fd);
    
    // Terminate
    endwin();
    
    return 0;
}

