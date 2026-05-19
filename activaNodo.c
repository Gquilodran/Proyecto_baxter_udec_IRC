#include <stdio.h>   
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>      
#include <sys/types.h>
#include <sys/wait.h>

#define SERIAL_PORT "/dev/ttyS*" 

int main() {
    char buffer[256];
    int fd = open(SERIAL_PORT, O_RDONLY);
    
    if (fd < 0) {
        perror("Error al abrir el puerto serial");
        return 1;
    }

    while (1) {
        int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; 
            printf("ESP32 dice: %s\n", buffer); 

            pid_t pid = fork();

            if (pid < 0) {
                perror("Error al crear el fork");
                exit(1);
            }
            if (pid == 0) {
                printf("Prueba de que funciona\n");
                 execlp("rosrun", "baxter_examples", "head_wobbler.py"); //rosrun baxter_examples head_wobbler.py
                exit(0);
            } 
  
            else {
                wait(NULL);
            }
        }
    }
    close(fd);
    return 0;
}