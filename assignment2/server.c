// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    const char* user = "nobody";
    struct passwd *user_info;
    pid_t child_process;    

    if(argc==2){
        printf("Inside child process after execvp call");
        int soc_value = atoi(argv[1]);
        valread = read( soc_value , buffer, 1024);
        printf("%s\n",buffer );
        send(soc_value , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
    }

    else{
    // Show ASLR
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    
    printf("Creating the child process \n");
    child_process = fork();
    if(child_process==0){
    int p_socket = dup(new_socket);
    if(p_socket == -1){
        printf("There were some errors while creating duplicate of parent socket :  \n");
    	return -1;
    }
    user_info = getpwnam(user);
    if(user_info == NULL){
    	printf("Unable to find the user with name Nobody");	
    	return -1;    
    }
    uid_t re = setuid(user_info->pw_uid);
    if(re < 0){
    	printf("There were some errors while dropping the privileges. \n");
    	return -1;
    }
    printf("Privileges are dropped by setting the user privileges to Nobody \n"); 
    //Using execvp function
    char buff[10];
    sprintf(buff, "%d", p_socket); 
    char* arguments[] = {argv[0], buff, NULL};
    int val = execvp(argv[0], arguments);
    if(val < 0){
        printf("There were some erros while executing execvp function");
        return -1;
      }
    }
    else if(child_process < 0 ){
        printf("There were some errors while forking \n");    
    }    
    else{
    	wait(NULL);
    }
  } 
    return 0;
}
