#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    for (int i = 0; i < 3; i++) // will have three children
    {
    
        char filepath[20]; // we will build path with i using snprintf
        snprintf(filepath, sizeof(filepath), "/mnt/vm%d/hello.txt", i+1);
        
        char buffer[7]; // buffer to store read

        pid_t pid = fork();

        if (pid == 0) // is a child
        {
            printf("Child: %d\n", getpid());
            FILE *fptr = fopen(filepath, "w"); // create
            if (fptr == NULL) {
                perror("Error opening file");
                return EXIT_FAILURE;
            }

            // writing to files
            fprintf(fptr, "Hello!"); // write
            printf("Child %d wrote to %s\n", getpid(), filepath);
            fclose(fptr); // close before read becasue write may be buffered

            // reading from files
            fopen(filepath, "r"); 
            while (fgets(buffer, 7, fptr)) {
                printf("Child %d reading from %s: %s\n", getpid(), filepath, buffer);
            }
            fclose(fptr);
            exit(0);
        }
        if (pid > 0) // is the parent
        {
            printf("Parent: %d\n", getpid());
            wait(NULL); // wait on my childs
        }
    
    }
}
