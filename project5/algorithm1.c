#include <stdio.h>
#include <stdlib.h>

// Defining constants for the number of philosophers and states
#define N 5
#define Eating 1
#define Thinking 1

#define Left(p) (p)
#define Right(p) (((p) + 1) % N)

typedef int *semaphore; 

// Array of semaphores for each fork
semaphore Fork[N]; 

// Function prototypes for various operations
int fork(void);
semaphore makeSemaphore(void);
void philosopher(int me);
void pickUp(int me);
int pipe(int pd[2]);
void putDown(int me);
int read(int fd, void *buf, unsigned len);
void signal(semaphore s);
void sleep(unsigned seconds);
void wait(semaphore s);
int write(int fd, void *buf, unsigned len);

void main()
{
    int i;

    // Initialize semaphores for each fork and signal them
    for (i = 0; i < N; ++i)
    {
        Fork[i] = makeSemaphore();
        signal(Fork[i]);
    }

    // Create child processes for N-1 philosophers
    for (i = 0; i < N - 1; ++i)
    {
        if (fork() == 0)
            break;
    }

    // Start the philosopher routine
    philosopher(i);
}

void pickUp(int me)
{
    // Special case for philosopher 0 to avoid deadlock
    if (me == 0)
    {
        wait(Fork[Right(me)]);
        printf("Philosopher %d picks up left fork\n", me);
        sleep(1);
        wait(Fork[Right(me)]);
        printf("Philosopher %d picks up left fork\n", me);
    }
    else
    {
        wait(Fork[Left(me)]);
        printf("Philosopher %d picks up left fork\n", me);
        sleep(1);
        wait(Fork[Right(me)]);
        printf("Philosopher %d picks up right fork\n", me);
    }
}

void putDown(int me)
{
    // Signal both forks to be put down
    signal(Fork[Left(me)]);
    signal(Fork[Right(me)]);
}

void philosopher(int me)
{
    char *s;
    int i = 1;

    // Infinite loop representing the life cycle of a philosopher
    for (;; ++i)
    {
        pickUp(me);
        // Determine the correct suffix for the ordinal number
        s = i == 1 ? "st" : i == 2 ? "nd"
                        : i == 3   ? "rd"
                                   : "th";
        printf("Philosopher %d eating for the %d%s time \n", me, i, s);
        sleep(Eating); // Simulate eating
        putDown(me);
        printf("Philosopher %d thinking\n", me);
        sleep(Thinking); // Simulate thinking
    }
}

semaphore makeSemaphore(void)
{
    // Allocate and initialize a semaphore
    int *semaphore;
    semaphore = calloc(2, sizeof(int));
    // Semaphore's wait and signal operations are handled by the pipe's behavior in reading and writing. 
    pipe(semaphore);
    return semaphore;
}

void wait(semaphore s)
{
    // Decrement semaphore or block if zero
    int junk;

    if (read(s[0], &junk, 1) <= 0)
    {
        printf("Error: wait() failed, check semaphore creation.\n");
        exit(1);
    }
}

void signal(semaphore s)
{
    // Increment semaphore
    if (write(s[1], "x", 1) <= 0)
    {
        printf("Error: write() failed, check semaphore creation.\n");
        exit(1);
    }
}
