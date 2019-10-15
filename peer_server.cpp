#include<unistd.h> 
#include<iostream>
#include<stdio.h> 
#include<sys/socket.h> 
#include<stdlib.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<string.h> 
// #define PORT 4200
#define BUFF_SIZE 1024
using namespace std;
char str[50]; 
int PORT;
string ip;
void * newfunc(void *arg)
{
   // cout<<"Entered newfunc"<<endl;
   int new_socket =*((int *) arg); 
  
   char filename[BUFF_SIZE];
   // recv(new_socket, &str, sizeof(str), 0);
   // cout<<"str="<<str<<endl;
   //memset (filename , '\0', BUFF_SIZE);
   // filename = (char*)malloc(sizeof(char)*BUFF_SIZE);
   cout<<"waiting to recv\n";
   recv(new_socket, filename, BUFF_SIZE, 0); 
   // cout<<"filename="<<filename<<endl;
   FILE *fp = fopen (filename, "rb");
   fseek ( fp , 0 , SEEK_END);
   int size = ftell ( fp );
   rewind ( fp );
   int file_size;
   
   char Buffer [BUFF_SIZE] ; 
   int n;
   send ( new_socket , &size, sizeof(file_size), 0);
   cout<<"sent\n";
   while ( ( n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp ) ) > 0  && size > 0 )
   {
        send (new_socket , Buffer, n, 0 );
        memset ( Buffer , '\0', BUFF_SIZE);
        size = size - n ;
   }

    fclose ( fp );
    close(new_socket); 
    // delete filename; 
    pthread_exit(NULL);
  }

void * func(void * arg)
{
    // cout<<"func in"<<endl;
    int server_fd, new_socket;
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address);
    
    // cout<<"before socket function call"<<endl;  
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    // cout<<"after socket function call"<<endl;    
     
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET; 
    // address.sin_addr.s_addr = INADDR_ANY;
    address.sin_addr.s_addr=inet_addr(ip.c_str());
     
    address.sin_port = htons( PORT ); 
       
     
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 5) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    // cout<<"before while\n";
     pthread_t thread[20];
     int lv=0;
    while(1)
    {
      // cout<<"after while\n";
      new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen);
      // cout<<"new_socket="<<new_socket<<endl; 
       
      memset (str, '\0', BUFF_SIZE);
      // recv(new_socket, &str, sizeof(str), 0);
      // recv(new_socket, &str, sizeof(str), 0);
      // cout<<"str="<<str<<endl;
     
      //cout<<"thread="<<thread<<endl;
      pthread_create( &thread[lv], NULL, newfunc, &new_socket);
      ++lv;
      // cout<<"thread="<<thread[lv]<<endl; 
      //pthread_join( thread, NULL);   
      
     
    }   
      close(new_socket);
      close(server_fd);
}
int main(int argc, char *argv[]) 
{ 
    PORT=atoi(argv[2]);
    ip=argv[1];
    pthread_t thread;
    // cout<<"ip="<<ip<<endl;
    // cout<<"PORT="<<PORT<<endl;
    // cout<<"before pthread_create"<<endl;    
    pthread_create( &thread, NULL, func, NULL);
    // cout<<"after pthread_create"<<endl; 
    pthread_join( thread, NULL); 
    
    return 0;
}    
  
