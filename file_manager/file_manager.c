// file_manager.c

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex; // mutex lock variable

void *listen_pipe()
{
    int fd1;
    int fd2;

    char *myfifo = "/tmp/my_named_pipe"; // name of the named pipe

    mkfifo(myfifo, 0666); // create the named pipe

    char text[80];
    char success[80] = "successful command";

    while (1)
    {
        fd1 = open(myfifo, O_RDONLY); // open the named pipe
        
        // wait for a message from file_client
        pthread_mutex_lock(&mutex); // acquire the mutex lock
        read(fd1, text, sizeof(text));
        printf("Received: %s", text); // print the message
        close(fd1);
        fd2 = open(myfifo, O_WRONLY); // open the named pipe
        pthread_mutex_unlock(&mutex); // release the mutex lock

        // send a response to file_client
        write(fd2, success, strlen(success) + 1);

        close(fd2);
    }

    return NULL;
}


int main()
{
    pthread_t tcb;
    pthread_create(&tcb, NULL, listen_pipe, NULL);
    pthread_mutex_init(&mutex, NULL); // initialize the mutex lock
    char input[80];

    while (1)
    {
        fgets(input, 80, stdin);

        if (strcmp(input, "exit\n") == 0) // exit command no argument
        {
            exit(0);
        }
    }
    pthread_join(tcb, NULL); // wait for the thread to finish
    

    pthread_mutex_destroy(&mutex); // destroy the mutex lock
    return 0;
}