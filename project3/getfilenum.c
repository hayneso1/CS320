#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../kernel/fs.h"
#include "user.h"

#define O_CREAT 0x0200 // Create if non-existent

void create_file(const char *filename)
{
    int fd = open(filename, O_CREAT);
    if (fd >= 0)
    {
        fprintf(2, "Created: %s\n", filename);
    }
    else
    {
        fprintf(2, "Failed to create %s\n", filename);
    }
}

int main(void)
{
    int pid = getpid();

    // Test Syscall defined in user.h
    int fileCount = getfilenum(pid);
    fprintf(2, "Current files open for PID %d: %d\n", pid, fileCount);

    create_file("Test1");
    create_file("Test2");
    create_file("Test3");
    create_file("Test4");
    create_file("Test5");
    create_file("Test6");

    // Test Syscall defined in user.h
    fileCount = getfilenum(pid);
    fprintf(2, "New number of open files for PID %d: %d\n", pid, fileCount);

    exit(0);
}
