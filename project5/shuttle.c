#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SHUTTLE_CAPACITY 30

sem_t shuttle_sem;
sem_t attendee_sem;
pthread_mutex_t count_mutex;
int attendee_count = 0;
int to_board = 0;
int total_attendees = 100; // Total number of attendees

void *shuttle(void *arg)
{
    while (1)
    {
        // Shuttle waits until there are enough attendees
        for (int i = 0; i < SHUTTLE_CAPACITY; i++)
        {
            sem_wait(&attendee_sem);
        }

        pthread_mutex_lock(&count_mutex);
        to_board = (attendee_count >= SHUTTLE_CAPACITY) ? SHUTTLE_CAPACITY : attendee_count;
        pthread_mutex_unlock(&count_mutex);

        // Signal attendees to board
        for (int i = 0; i < to_board; i++)
        {
            sem_post(&shuttle_sem);
        }

        // Shuttle departs and arrives after some time
        printf("Shuttle departs with %d attendees\n", to_board);
        sleep(5); // Simulate travel time

        // Unloading passengers
        for (int i = 0; i < to_board; i++)
        {
            sem_post(&attendee_sem);
        }

        printf("Shuttle returns\n");

        // Check if all attendees have been transported
        pthread_mutex_lock(&count_mutex);
        if (attendee_count == 0)
        {
            pthread_mutex_unlock(&count_mutex);
            break; // Exit the loop if all attendees are transported
        }
        pthread_mutex_unlock(&count_mutex);
    }
}

void *attendee(void *arg)
{
    pthread_mutex_lock(&count_mutex);
    attendee_count++;
    printf("Attendee arrived, total: %d\n", attendee_count);
    pthread_mutex_unlock(&count_mutex);

    sem_post(&attendee_sem); // Notify shuttle of arrival

    sem_wait(&shuttle_sem); // Wait to board the shuttle

    pthread_mutex_lock(&count_mutex);
    attendee_count--;
    pthread_mutex_unlock(&count_mutex);

    // Attendee boards the shuttle
    printf("Attendee boards the shuttle\n");

    // Attendee waits to get off at the destination
    sem_wait(&attendee_sem);
    printf("Attendee gets off at destination\n");
}

int main()
{
    pthread_t shuttle_thread;
    pthread_t attendee_threads[100]; // Assuming 100 attendees

    // Initialize semaphores and mutex
    sem_init(&shuttle_sem, 0, 0);
    sem_init(&attendee_sem, 0, 0);
    pthread_mutex_init(&count_mutex, NULL);

    // Create shuttle thread
    pthread_create(&shuttle_thread, NULL, shuttle, NULL);

    // Create attendee threads
    for (int i = 0; i < 100; i++)
    {
        pthread_create(&attendee_threads[i], NULL, attendee, NULL);
        sleep(1); // Stagger arrival of attendees
    }

    // Join threads (optional, based on your application's need)
    pthread_join(shuttle_thread, NULL);
    for (int i = 0; i < 100; i++)
    {
        pthread_join(attendee_threads[i], NULL);
    }

    // Cleanup
    sem_destroy(&shuttle_sem);
    sem_destroy(&attendee_sem);
    pthread_mutex_destroy(&count_mutex);

    return 0;
}
