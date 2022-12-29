// file_client.c

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex; // mutex lock variable

int main()
{
    int fd1;
    int fd2;

    char *myfifo = "/tmp/my_named_pipe"; // name of the named pipe

    mkfifo(myfifo, 0666); // create the named pipe
    pthread_mutex_init(&mutex, NULL); // initialize the mutex lock

    char text[80];
    char success[80] = "successful command";

    while (1)
    {
        fd1 = open(myfifo, O_WRONLY); // open the named pipe
        
        // read a line of text from the user
        printf("Enter a message: ");
        fgets(text, 80, stdin);

        // write the message to the named pipe
        write(fd1, text, strlen(text) + 1);
        close(fd1);
        fd2 = open(myfifo, O_RDONLY); // open the named pipe
        // wait for a response from file_manager
        pthread_mutex_lock(&mutex); // acquire the mutex lock
        read(fd2, success, 80);
        printf("Received: %s", success);
        pthread_mutex_unlock(&mutex); // release the mutex lock

        close(fd2);
    }

    pthread_mutex_destroy(&mutex); // destroy the mutex lock
    return 0;
}
