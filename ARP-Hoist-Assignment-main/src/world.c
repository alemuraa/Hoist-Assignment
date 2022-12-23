#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Buffer to store the string to write to the log file
char log_buffer[100];

// File descriptor for the log file
int log_fd;

// Constants to store the minimum and maximum x position
const float x_min = 0.0;
const float x_max = 40.0;

// Constants to store the minimum and maximum z position
const float z_min = 0.0;
const float z_max = 40.0;

// Variables initialization
float pos_x=0.0;
float pos_z=0.0;
float err_x=0.0;
float err_z=0.0;

// Variable declaration in order to get the time
time_t rawtime;
struct tm *info;

// Variable to store the value of the write function
int check;

// Variable declaration in order to get the time
time_t rawtime;
struct tm *info;

// Function to check the correctness of the pipe
void CheckCorrectness(int c) 
{
    if(c == -1) 
    {
        perror("Error in writing function");
        exit(1);
    }
}

// Compute random error of 1% of current position
float compute_err(float pos)
{
    if (pos==0.0)
    {
        return 0.0;
    }

    int err;
    err=(int)(pos);
    int n = ( rand() % (err - (-err) + 1) ) + (-err);

    // Write to the log file
    sprintf(log_buffer, "<world> Error computed: %s\n", asctime(info));
    check = write(log_fd, log_buffer, strlen(log_buffer));
    CheckCorrectness(check);

    return (float)n/100;
}

// Function to send the position to Inspection
void send_pos(const char * myfifo,float pos_x)
{
    int fd;
    if ((fd = open(myfifo, O_WRONLY))==-1)
    { 
        close(fd);
        perror("Error opening fifo");
        exit(1);
    }

    check = write(fd, &pos_x, sizeof(pos_x));
    CheckCorrectness(check);
    close(fd);
}

int main()
{
    // Open the log file
    if ((log_fd = open("world.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1)
    {
        perror("Error opening motor_x file");
        return 1;
    }

    // Variable declaration
    int fd;
    float z_old=0.0;
    float x_old=0.0;

    // Creation of the FIFOs
    const char * pos_x_fifo = "/tmp/pos_x_fifo";
    const char * pos_z_fifo = "/tmp/pos_z_fifo";
    const char * real_x_fifo = "/tmp/real_x_fifo";
    const char * real_z_fifo = "/tmp/real_z_fifo";
    mkfifo(pos_x_fifo, 0666);
    mkfifo(pos_z_fifo, 0666);
    mkfifo(real_x_fifo, 0666);
    mkfifo(real_z_fifo, 0666);

    while (1)
    {
        // Get current time
        time(&rawtime);
        info = localtime(&rawtime);

        // Read the x position from the FIFOs
        if(fd = open(pos_x_fifo, O_RDONLY))
        {
            if(read(fd, &pos_x, sizeof(pos_x))==-1)
            {
                
                close(fd);
                perror("Error reading fifo");
                exit(1);
            }

            close(fd);
        }

        else
        {
            close(fd);
            perror("Error opening fifo");
            exit(1);
        }

        // Read the z position from the FIFOs
        if(fd = open(pos_z_fifo, O_RDONLY))
        {
            if(read(fd, &pos_z, sizeof(pos_z))==-1)
            {  
                close(fd);
                perror("Error reading fifo");
                exit(1);
            }

            close(fd);
        }

        else 
        {
            close(fd);
            perror("Error opening fifo");
            exit(1);
        }

        if (pos_x==x_old)
        {
            pos_x=x_old;
        }

        // Check if the position exceeds the maximum bound
        else if (pos_x>x_max)
        {
            pos_x=x_max;
        }

        // Check if the position exceeds the minimum bound
        else if (pos_x<x_min)
        {
            pos_x=x_min;
        }

        else
        {
            err_x=compute_err(pos_x);
        }

        if (pos_z==z_old)
        {
            pos_z=z_old;
        }

        // Check if the position exceeds the maximum bound
        else if (pos_z>z_max)
        {
            pos_z=z_max;
        }

        // Check if the position exceeds the minimum bound
        else if (pos_z<z_min)
        {
            pos_z=z_min;
        }

        else
        {
            err_z=compute_err(pos_z);
        }

        // Update position
        x_old = pos_x;
        z_old = pos_z;

        // Update the real position
        pos_x=pos_x+err_x;
        pos_z=pos_z+err_z;
        
        // Send the real position to World
        send_pos(real_x_fifo,pos_x);
        send_pos(real_z_fifo,pos_z);
    }

    return 0;
}
    
    


    
        

    