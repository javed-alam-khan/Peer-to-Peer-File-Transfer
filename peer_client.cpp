#include<unistd.h> 
#include<iostream>
#include<stdio.h> 
#include<sys/socket.h> 
#include<stdlib.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<string.h>
#include<bits/stdc++.h>
#include <map> 
// #define PORT 4200
#define BUFF_SIZE 1024
using namespace std;

int PORT;
int NPORT;
string ip;
string userid;
string passwd;
void * newfunc(void *arg)
{
   int new_socket =*((int *) arg); 
   char filename[BUFF_SIZE];
   memset (filename, '\0', BUFF_SIZE);
   recv(new_socket, &filename, BUFF_SIZE, 0); 
  
   
   FILE *fp = fopen (filename, "rb");
   fseek ( fp , 0 , SEEK_END);
   int size = ftell ( fp );
   rewind ( fp );
   int file_size;
   
   char Buffer [BUFF_SIZE] ; 
   int n;
   send ( new_socket , &size, sizeof(size), 0);
   while ( ( n = fread( Buffer , sizeof(char) , BUFF_SIZE , fp ) ) > 0  && size > 0 )
   {
        send (new_socket , Buffer, n, 0 );
        memset ( Buffer , '\0', BUFF_SIZE);
        size = size - n ;
   }

    fclose ( fp );
    close(new_socket);  
  }

void * func(void * arg)
{
    int server_fd, new_socket;
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
     
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
     
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
    while(1)
    {
      if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
      { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
      } 
      
      pthread_t thread;
      pthread_create( &thread, NULL, newfunc, &new_socket); 
      pthread_join( thread, NULL);   
      
    }   
      close( server_fd);
}
void account(int sock, string userid, string passwd)
{
    char uid[BUFF_SIZE];
    char psd[BUFF_SIZE];
    bzero(uid,BUFF_SIZE);
    bzero(psd,BUFF_SIZE);

    int cstatus=0;
    int option=1;
    send(sock , &option , sizeof(option) , 0 );
    strcpy(uid,userid.c_str());
    strcpy(psd,passwd.c_str());
    send(sock , uid , BUFF_SIZE , 0 );
    send(sock , psd , BUFF_SIZE , 0 );
    recv(sock,&cstatus,sizeof(cstatus),0);
    // recv(new_socket, uid, BUFF_SIZE, 0);
    if(cstatus==1)
      cout<<"userid successfully created\n";
}

void login_req(int sock, string userid, string passwd)
{
    char uid[BUFF_SIZE];
    char psd[BUFF_SIZE];
    int lstatus=0;
    int option=2;
    send(sock , &option , sizeof(option) , 0 );
    strcpy(uid,userid.c_str());
    strcpy(psd,passwd.c_str());
    send(sock , uid , BUFF_SIZE , 0 );
    send(sock , psd , BUFF_SIZE , 0 );
    recv(sock,&lstatus,sizeof(lstatus),0);
    
    if(lstatus==1)
      cout<<"successfully logged in \n";
}


int main(int argc, char *argv[]) 
{ 
    int sock = 0;
    int file_size;
    int n;
    int l1;
    int l2;
    char Buffer[BUFF_SIZE] ;
    char filename[BUFF_SIZE];

    getline(cin,userid);
    getline(cin,passwd);

    /*l1=userid.length();
    l2=passwd.length();*/
    // char uid[BUFF_SIZE];
    // char psd[BUFF_SIZE];
    // strcpy(uid,userid.c_str());
    // strcpy(psd,passwd.c_str());
    struct sockaddr_in serv_addr; 
    char hello[] = "Hello from client\n";
    NPORT=atoi(argv[3]);
    // cout<<"hello world"<<endl;
    // cout<<NPORT<<endl;
    PORT=atoi(argv[2]);
    // cout<<PORT<<endl;
    ip=argv[1];
    cout<<ip<<endl;
    cout<<"creating thread\n";
    pthread_t thread;
    pthread_create( &thread, NULL, func, NULL); 
     
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
    
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(NPORT); 
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

    if(connect(sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    // connect (sock  ,(struct sockaddr *)&serv_addr  , sizeof(serv_addr) );
    account(sock, userid, passwd);
    //send(sock , hello , sizeof(hello) , 0 );
    // cout<<"msg sent\n";
    //int file_size;
    
    getline(cin,userid);
    getline(cin,passwd);
    login_req(sock, userid, passwd);

    string flname="client1_file";
    strcpy(filename,flname.c_str()); 
    //memset (filename, '\0', BUFF_SIZE);
    cout<<"filename="<<filename<<endl;
    send(sock , filename, BUFF_SIZE, 0);
    cout<<"sent n waiting to  rcv\n";
    recv(sock, &file_size, sizeof(file_size), 0);
    cout<<"received\n";
    FILE *fp=fopen ("/home/javed/Desktop/socket programming/working/server_file" , "wb"); 
    while ( ( n = recv(sock , Buffer , BUFF_SIZE, 0) ) > 0  &&  file_size > 0)
    { 
     recv(sock, &file_size, sizeof(file_size), 0);        
     fwrite(Buffer , sizeof (char), n, fp);
     memset(Buffer , '\0', BUFF_SIZE);
     file_size = file_size - n;
    }
    
    close( sock);
    return 0;
}    
