#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

// Variables to store the PIDs
pid_t pid_cmd;
pid_t pid_motor_x;
pid_t pid_motor_z;
pid_t pid_world;
pid_t pid_insp;

// Buffer to store the string to write to the log file
char log_buffer[100];

// File descriptor for the log file
int log_fd;

// Variable to store the status of the child process
int status;

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

int spawn(const char * program, char * arg_list[]) {

  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking...");
    return 1;
  }

  else if(child_pid != 0) {
    return child_pid;
  }

  else {
    if(execvp (program, arg_list) == 0);
    perror("Exec failed");
    return 1;
  }
}


// Function to get when a file was last modified
time_t get_last_modified(char *filename)
{
  struct stat attr;
  stat(filename, &attr);

  return attr.st_mtime;
}

int watchdog()
{
  // Array of the log file paths
  char *log_files[5] = {"command.log", "mx.log", "mz.log", "world.log", "inspection.log"};

  // Array of the PIDs
  pid_t pids[5] = {pid_cmd, pid_motor_x, pid_motor_z, pid_world, pid_insp};

  // Flag to check if a file was modified
  int modified;

  // Variable to keep the number of seconds since the last modification
  int counter = 0;

  while (1)
  {
    // Get current time
    time_t current_time = time(NULL);

    // Loop through the log files
    for (int i = 0; i < 5; i++)
    {
      // Get the last modified time of the log file
      time_t last_modified = get_last_modified(log_files[i]);

      // Check if the file was modified in the last 3 seconds
      if (current_time - last_modified > 3)
      {
        modified = 0;
      }
      else
      {
        modified = 1;
        counter = 0;
      }
    }

    if (modified==0)
    {
      counter += 3;
    }

    // If the counter is greater than 60, kill the child processes
    if (counter > 60)
    {
      // Kill all the processes
      kill(pid_cmd, SIGKILL);
      kill(pid_motor_x, SIGKILL);
      kill(pid_motor_z, SIGKILL);
      kill(pid_world, SIGKILL);
      kill(pid_insp, SIGKILL);

      return 0;
    }

    // Sleep for 2 seconds
    sleep(2);
  }
}

int main() {
 
  // Open the log file
  if ((log_fd = open("master.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1)
  {
    perror("Error opening log file");
    return 1;
  }
        
  // Get the time when the Master starts its execution
  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime( &rawtime );
  char buffer[100];

  // Write into the log file
  sprintf(buffer, "<master_process> Master process started: %s\n", asctime(info));
  check = write(log_fd, buffer, strlen(buffer));
  CheckCorrectness(check);

  // Command Console
  char * arg_list_command[] = { "/usr/bin/konsole", "-e", "./bin/command", NULL };
  pid_cmd = spawn("/usr/bin/konsole", arg_list_command);

  // Motor x
  char * arg_list_motor_x[] = { "./bin/motor_x", NULL };
  pid_motor_x = spawn("./bin/motor_x", arg_list_motor_x);

  // Convert motor x pid to string
  char pid_mx_str[10];
  sprintf(pid_mx_str, "%d", pid_motor_x);

  // Motor z
  char * arg_list_motor_z[] = { "./bin/motor_z", NULL };
  pid_motor_z = spawn("./bin/motor_z", arg_list_motor_z);

  // Convert motor z pid to string
  char pid_mz_str[10];
  sprintf(pid_mz_str, "%d", pid_motor_z);

  // World
  char * arg_list_world[] = { "./bin/world", NULL };
  pid_world = spawn("./bin/world", arg_list_world);

  // Inspection console
  char *arg_list_inspection[] = {"/usr/bin/konsole", "-e", "./bin/inspection", pid_mx_str, pid_mz_str, NULL};
  pid_insp = spawn("/usr/bin/konsole", arg_list_inspection);

  // // Create the log files
  int fd_cmd = open("command.log", O_CREAT | O_RDWR, 0666);
  int fd_mx = open("mx.log", O_CREAT | O_RDWR, 0666);
  int fd_mz = open("mz.log", O_CREAT | O_RDWR, 0666);
  int fd_world = open("world.log", O_CREAT | O_RDWR, 0666);
  int fd_insp = open("inspection.log", O_CREAT | O_RDWR, 0666);
 
  // Check corecctness
  if(fd_cmd <0 || fd_insp <0 ||fd_mx<0 ||fd_mx<0||fd_world<0)
  {
    printf("Error opening FILE");
  }
 
  // Close the log files
  close(fd_cmd);
  close(fd_mx);
  close(fd_mz);
  close(fd_world);
  close(fd_insp);
 
  // Whatchdog funcion call
  watchdog();
  
  // Get the time when the Master finishes its execution
  time( &rawtime );
  info = localtime(&rawtime);

  // write into the log file
  sprintf(buffer, "<master_process> Master process terminated: %s\n", asctime(info));
  check = write(log_fd, buffer, strlen(buffer));
  CheckCorrectness(check);
  
  close(log_fd);

  int status;

  // Check PIDs
  waitpid(pid_cmd, &status, 0);
  waitpid(pid_insp, &status, 0);
  waitpid(pid_motor_x, &status, 0);
  waitpid(pid_motor_z, &status, 0);
  waitpid(pid_world, &status, 0);

  close(log_fd);

  printf ("Main program exiting with status %d\n", status);
  
  return 0;
}

