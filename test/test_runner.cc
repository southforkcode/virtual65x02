#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, const char **argv) {
    for(int i=1; i<argc; i++) {
        int pid, status;
        if((pid = fork())) {
            waitpid(pid, &status, 0);
        } else {
            printf("Executing %s...\n", argv[i]);
            execl(argv[i], argv[i], "-v high", "-d yes", NULL);
        }
        if(status != 0) break;
    }
}