#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
using namespace std;
#include <unistd.h>
#define GetCurrentDir getcwd

int create_socket(int,char *);

#define MAXLINE 256 /*max text line length*/

int main(int argc, char **argv)
{
    int sockfd, portno, n;
 struct sockaddr_in servaddr;
	struct hostent *server;
 char sendline[MAXLINE], recvline[MAXLINE];

 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=4) {
  cerr<<"Usage: "<<argv[0]<<" <IP address of the server:port number> <downloadFilename> <recieveInterval>"<<endl;
  exit(1);
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }
	char ipad[256];
	char portnum[256];
	char *p;int i=0;
  	p = strtok (argv[1],":");
  	while (p!= NULL)
  	{
    	if(i==0) {strcpy(ipad,p);		i++;}
		else if(i==1) strcpy(portnum,p);
    	p = strtok (NULL, ":");

  	}
	portno = atoi(portnum);
	server = gethostbyname(ipad);
 //Creation of the socket
 bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&servaddr.sin_addr.s_addr,
         server->h_length);
    servaddr.sin_port = htons(portno);//convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cout<<"ERROR connecting to "<<ipad<<"\n";
		exit(2);
 }
 else cout<<"ConnectDone: "<<ipad<<":"<<portnum<<"\n";
 cout<<"Guide: use get filename\n";
 cout<<"ftp>";

 while (fgets(sendline, MAXLINE, stdin) != NULL) {

  send(sockfd, sendline, MAXLINE, 0);
  char *token,*dummy;
  dummy=sendline;
  token=strtok(dummy," ");
   
   if (strcmp("quit\n",sendline)==0)  {
   	//close(sockfd);
	return 0;
   }

   else if (strcmp("get",token)==0)  {
   	char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int data_port,datasock,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	recv(sockfd, port, MAXLINE,0);
	data_port=atoi(port);
	datasock=create_socket(data_port,argv[1]);
	recv(sockfd,message,MAXLINE,0);
	   //cout<<message<<" message\n";
	token=strtok(NULL," \n");
	   //cout<<token<<" is tocken"<<endl;
	if(strcmp("1",message)==0){
		if((fp=fopen(argv[2],"w"))==NULL)
			cout<<"Error in creating file\n";
		else
		{
			recv(sockfd, char_num_blks, MAXLINE,0);
			num_blks=atoi(char_num_blks);
			for(i= 0; i < num_blks; i++) { 
				recv(datasock, buffer, MAXLINE,0);
				fwrite(buffer,sizeof(char),MAXLINE,fp);
				//cout<<buffer<<endl;
			//cout<<"Sleeping now for the "<<i<<"th time"<<endl;
			usleep(atoi(argv[3])*1000);
			}
			recv(sockfd, char_num_last_blk, MAXLINE,0);
			num_last_blk=atoi(char_num_last_blk);
			if (num_last_blk > 0) { 
				recv(datasock, buffer, MAXLINE,0);
				fwrite(buffer,sizeof(char),num_last_blk,fp);
				//cout<<buffer<<endl;
			}
			fclose(fp);
			cout<<"FileWritten: "<<MAXLINE*num_blks+num_last_blk<<" bytes\n"<<endl;
			cout<<"Data receiving done, connection will be terminated now\n"; 
			send(sockfd, "quit\n", MAXLINE, 0);
			return(0);
		}
	}
	else{
		cerr<<"Error in opening file at server. Check filename\nUsage: get filename"<<endl;
	}
   }
   else{
	cerr<<"Error in command. Check Command"<<endl;
   }
   cout<<"ftp>";

 }

 exit(0);
}


int create_socket(int port,char *addr)
{
 int sockfd;
 struct sockaddr_in servaddr;


 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(addr);
 servaddr.sin_port =  htons(port); //convert to big-endian order

 //Connection of the client to the socket
	//cout<<"port number is "<<port<<endl;
	int z=connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
 if (z<0) {
  cerr<<"Problem in creating data channel"<<endl;
  exit(3);
 }


return(sockfd);
}