#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void executeBinaryToFile(const char *binary_file, const char *output_file) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", binary_file, ">", output_file, (char *)NULL);
        perror("execl");
        exit(1);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(1);
        }
    }
}

int compareFiles(const char *file1, const char *file2) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp("cmp", "cmp", "-s", file1, file2, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return 1;
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <binary file> <text file>\n", argv[0]);
        return 1;
    }

    const char *binary_file = argv[1];
    const char *text_file = argv[2];
    const char *output_file = "output.txt";

    executeBinaryToFile(binary_file, output_file);

    int result = compareFiles(output_file, text_file);

    if (result == 0) {
        printf("The %s exec  has the desired output\n", binary_file);
    } else {
        fprintf(stderr, "ERROR: The %s executable does not have the desired output\n", binary_file);
    }

    return result;
}
