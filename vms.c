#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

pid_t vm2_pid = -1;

void handler(int sig) {
  printf("VM2 has received the signal from VM1 \n");
}

int checkAllowed(int vmNum, char* path) //vmNum is i+1
{
    char vm[4];
    sprintf(vm, "vm%d", vmNum); // i+1 -> vm1/vm2/vm3
    if (strstr(path, vm) != NULL) {
        printf("Access allowed for %s to %s\n", vm, path);
        return 1;
    } else {
        printf("Denied, %s cannot access %s\n", vm, path);
        return 0;
    }
}

int main() {
    for (int i = 0; i < 3; i++) // will have three children
    {
    
        char filepath[20]; // we will build path with i using snprintf
        snprintf(filepath, sizeof(filepath), "/mnt/vm%d/hello.txt", i+1);
        
        char buffer[7]; // buffer to store read

        pid_t pid = fork();

        if (pid == 0) // is a child
        {
            printf("Child: %d (vm%d)\n", getpid(), i+1);
            if (checkAllowed(i+1, filepath)) { // check if this child allowed 

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
            }

            // demo of vm permissions denial
            switch (i) 
            {
                case 0:
                    if (checkAllowed(i+1, "/mnt/vm2/hello.txt")) {
                        fopen("/mnt/vm2/hello.txt", "r");
                        printf("! vm1 was able to read fron /mnt/vm2/hello.txt !\n");
                    }
                    break; // make sure wrong i doesnt fall thru to other cases
                case 1:
                    if (checkAllowed(i+1, "/mnt/vm3/hello.txt")) {
                        fopen("/mnt/vm3/hello.txt", "r");
                        printf("! vm2 was able to read /mnt/vm3/hello.txt !\n");
                    }
                    break;
                case 2:
                    if (checkAllowed(i+1, "/mnt/vm1/hello.txt")) {
                        fopen("/mnt/vm1/hello.txt", "r");
                        printf("! vm3 was able to read from /mnt/vm1/hello.txt !\n");
                    }
                    break;
            }
            exit(0);
        }
        if (pid > 0) // is the parent
        {
            printf("Parent: %d\n", getpid());
            wait(NULL); // wait on my childs
        }
    
    }
}
