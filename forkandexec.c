#include <stdio.h>
#include <stdlib.h> // exit için
#include <unistd.h>  // fork.execve , waitpid için 
#include <sys/wait.h> // waitpid için 

int main()
{
        pid_t pid;
        int stat;
        char *av[] = {"/bin/ls", "-l", NULL}; // Çalıştırılacak komutu path ten bul ve av lere göre 
        char *envp[] = {NULL}; // Şuanlı boş 

        pid = fork(); // yeni süreç başlatıyoruz işlem için

        if (pid == -1)
        {
                perror("fork"); // klasik hata kontrolü
                exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
                // Child process için buradayız 
                printf("child process : Komut çalıştırılıyor...\n");
                // execve ile "/bin/ls -l" komutunu çalıştırıcaz
                if(execve(av[0],av, envp) == -1)
                {
                        perror("execve");
                        exit(EXIT_FAILURE);
                }
        }
        else
        {
                printf("parent process : waiting to child process ending (PID %d\n)",pid);
                if(waitpid(pid, &stat, 0) == -1)
                {
                        perror("waitpid"); // waitpid başarısızda klasik hata mesajı mk;
                        exit(EXIT_FAILURE);
                }
                if(WIFEXITED(stat))
                {
                        printf("Parent process: child process has normally ending amk exit code %d\n",WEXITSTATUS(stat));
                }
                else
                {
                        printf("Parent process : child process düzgün bitemedi amünakoyim");
                }

        }
        return 0;
        
}