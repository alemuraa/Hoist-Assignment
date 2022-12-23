#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

// Buffer to store the string to write to the log file
char log_buffer[100];

// File descriptor for the log file
int log_fd;

// Constants to store the minimum and maximum x position
const float x_min = 0.0;
const float x_max = 40.0;

// variable to store velocity and position
float v_x = 0.0;
float pos_x = 0.0;

// Variable declaration in order to get the time
time_t rawtime;
struct tm *info;

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

// Function to send the position to world file
void send_pos(const char * myfifo,float pos_x)
{
    int fd;

    if ((fd = open(myfifo, O_WRONLY))==-1)
    {
        close(log_fd);
        close(fd);
        perror("Error opening fifo");
        exit(1);
    }

    check = write(fd, &pos_x, sizeof(pos_x));
    CheckCorrectness(check);
    close(fd);
}

float increment(const char* fifo,int id)
{
    int fd;
    
    if(fd = open(fifo, O_RDONLY|O_NDELAY))
    {
        if(read(fd, &id, sizeof(id))==-1)
        {
            close(log_fd);
            close(fd);
            perror("Error reading from fifo");
            exit(1);
        }
        
        // Decrease the velocity
        if(id==0)
        {
           // Write to the log file
           sprintf(log_buffer, "<motor_x> v_x decreased: %s\n", asctime(info));
           check = write(log_fd, log_buffer, strlen(log_buffer));
           CheckCorrectness(check);

            return -1.0;
        }

        // Increase the velocity
        else if(id==1)
        {
           // Write to the log file
           sprintf(log_buffer, "<motor_x> v_x increased: %s\n", asctime(info));
           check = write(log_fd, log_buffer, strlen(log_buffer));
           CheckCorrectness(check);

            return 1.0;
        }

        // Stop/Reset the motor
        else if(id==2)
        {
           v_x = 0.0;

           // Write to the log file
           sprintf(log_buffer, "<motor_x> motor_x stopped: %s\n", asctime(info));
           check = write(log_fd, log_buffer, strlen(log_buffer));
           CheckCorrectness(check);

           return 0.0;
        }

        else
        {
            return 0.0;
        }
    }

    close(log_fd);
    close(fd);
    perror("Error opening fifo");
    exit(1);
}

void stop_handler(int sig)
{
    if (sig == SIGUSR1)
    {
        // Stop the motor
        v_x = 0;

        // Listen for stop signal
        if (signal(SIGUSR1, stop_handler) == SIG_ERR)
        {
            exit(1);
        }
    }
}

// Reset signal handler
void reset_handler(int sig)
{
    if (sig == SIGUSR2)
    {
        // Setting velocity to -5
        v_x = -5;
        
        // Listen for stop signal
        if (signal(SIGUSR2, reset_handler) == SIG_ERR)
        {
            exit(1);
        }
    }
}

int main()
{
    // Open the log file
    if ((log_fd = open("mx.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1)
    {
        perror("Error opening motor_x file");
        return 1;
    }
 
    // Variable declaration
    float vx_inc = 0.0;
    int id = -1;

    // Creation of the FIFOs
    const char * vx_fifo = "/tmp/vx_fifo";
    const char * pos_x_fifo = "/tmp/pos_x_fifo";
    mkfifo(vx_fifo, 0666);
    mkfifo(pos_x_fifo, 0666);

    // Listen for signals
    if (signal(SIGUSR1, stop_handler) == SIG_ERR || signal(SIGUSR2, reset_handler) == SIG_ERR)
    {
        // Close file descriptors
        close(vx_fifo);
        close(pos_x_fifo);
        close(log_fd);

        exit(1);
    }
    
    while (1)
    {
        // Get current time
        time(&rawtime);
        info = localtime(&rawtime);

        vx_inc = increment(vx_fifo,id);
        
        // Increment the velocity and the position
        v_x = v_x + vx_inc;
        pos_x = pos_x + v_x;
        
        // Check if the position is within the bounds
        if(pos_x > x_max)
        {
            pos_x = x_max;
            v_x = 0.0;
        }
        else if(pos_x < x_min)
        {
            pos_x = x_min;
            v_x = 0.0;
        }
        
        // Send the position to the world file
        send_pos(pos_x_fifo,pos_x);

        sleep(1);
    }
    
    // Close the FIFOs
    close(vx_fifo);
    close(pos_x_fifo);
    close(log_fd);

    return 0;
}