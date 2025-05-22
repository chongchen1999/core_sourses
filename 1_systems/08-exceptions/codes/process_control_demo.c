#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // 获取并打印当前进程 ID
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("Parent Process: PID = %d, PPID = %d\n", pid, ppid);

    // 创建子进程
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // === 子进程部分 ===
        printf("Child Process: PID = %d, PPID = %d\n", getpid(), getppid());

        // 让子进程暂停 2 秒
        printf("Child sleeping for 2 seconds...\n");
        sleep(2);

        // 使用 execve() 执行 /bin/ls -l
        char *argv[] = {"/bin/ls", "-l", NULL};
        char *envp[] = {NULL}; // 简单使用空环境变量
        execve("/bin/ls", argv, envp);

        // 如果 execve 失败
        perror("execve failed");
        _exit(EXIT_FAILURE); // 使用 _exit() 快速退出子进程
    } else {
        // === 父进程部分 ===
        printf("Parent is waiting for child (PID: %d)...\n", child_pid);

        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("Child did not exit normally.\n");
        }

        printf("Parent process exiting.\n");
    }

    return 0;
}
