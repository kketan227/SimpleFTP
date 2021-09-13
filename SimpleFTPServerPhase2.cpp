/* 
Designed by Ketan Karnakota
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;
void error(const char *msg)
{
    cout<< (msg);
    exit(1);
}
int size=0;
int MAXLINE=256;
int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
	char char_num_blks[256],char_num_last_blk[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
	if(argc <2) {
		cout << "usage of: " <<argv[0]<< " <port>\n";
		exit(1);
	}
     // create a socket
     // defn of socket function(for reference): socket(int domain, int type, int protocol)
     sockfd =  socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket \n");

     // clear address structure
     bzero((char *) &serv_addr, sizeof(serv_addr));		//setting all these values zero
	 portno = atoi(argv[1]);	//conversion of port number

     /* setup the host_addr structure for use in bind call */
     	// server byte order
     	serv_addr.sin_family = AF_INET;  

     	// automatically be filled with current host's IP address
     	serv_addr.sin_addr.s_addr = INADDR_ANY;  

     	// convert short integer value for port must be converted into network byte order
     	serv_addr.sin_port = htons(portno);

     	//Defn of bind: bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     	// bind() passes file descriptor, the address structure, 
     	// and the length of the address structure
     	// This bind() call will bind  the socket to the current IP address on port, portno
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr) ) < 0){
              	cout<< "Error on binding \n";
    			exit(2);
	 }
	cout << "BindDone: " << portno << " \n";

     	// This listen() call tells the socket to listen to the incoming connections.
     	// The listen() function places all incoming connection into a backlog queue
     	// until accept() call accepts the connection.
     	// Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd,5);
	cout << "ListenDone: " << portno << " \n";
     	// The accept() call actually accepts an incoming connection
    while(true){
	clilen = sizeof(cli_addr);
     	// This accept() function will write the connecting client's address info 
     	// into the the address structure and the size of that structure is clilen.
     	// The accept() returns a new socket file descriptor for the accepted connection.
     	// So, the original socket file descriptor can continue to be used 
     	// for accepting new connections while the new socker file descriptor is used for
     	// communicating with the connected client.
     	newsockfd = accept(sockfd, 
                 	(struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
	          error("ERROR on accept \n");

     cout<< "Client: " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) <<"\n";
	//getting the filename here
	char filename[256];
	char extractedfilename[256];
	bzero(filename,256);
	bzero(extractedfilename,256);
    n = read(newsockfd,filename,255);
    if (n < 0) error("ERROR reading from socket");
	//cout<<filename;
	int index=4,in=0;
	if(filename[0]!='g'||filename[1]!='e'||filename[2]!='t'||filename[3]!=' '){
		cout<< "unknownCommand\n";
		cout<<"Client being disconnected\n";
		send(newsockfd,"2",MAXLINE,0);
		close(newsockfd);
		bzero(filename,256);
		bzero(extractedfilename,256);
		continue;
	}
	while(filename[index]!='\n'){
		extractedfilename[in]=filename[index];
		in++;index++;
	}
	cout<<"FileRequested: "<<extractedfilename<<endl;
	//the filename is "extractedfilename"

		int lSize,num_blks,num_last_blk,i;
		FILE *fp;
	if ((fp=fopen(extractedfilename,"r"))!=NULL)
	{
		//size of file
		send(newsockfd,"1",MAXLINE,0);
		fseek (fp , 0 , SEEK_END);
		lSize = ftell (fp);
		rewind (fp);
		num_blks = lSize/MAXLINE;
		num_last_blk = lSize%MAXLINE; 
		sprintf(char_num_blks,"%d",num_blks);
		send(newsockfd, char_num_blks, MAXLINE, 0);
		//cout<<"number of blocks "<<num_blks<<" 	last block size		"<<num_last_blk<<endl;

		for(i= 0; i < num_blks; i++) { 
			size+=fread (buffer,sizeof(char),MAXLINE,fp);
			send(newsockfd, buffer, MAXLINE, 0);
			//cout<<buffer<<"	"<<i<<endl;
		}
		sprintf(char_num_last_blk,"%d",num_last_blk);
		send(newsockfd, char_num_last_blk, MAXLINE, 0);
		if (num_last_blk > 0) { 
			size+=fread (buffer,sizeof(char),num_last_blk,fp);
			send(newsockfd, buffer, MAXLINE, 0);
			//cout<<buffer<<endl;
		}
		fclose(fp);
		//cout<<"File upload done.\n";
		
	}
	else{
		cout<<"FileTransferFail\n";
		cerr<<"File not found or not readable.\nClient being disconnected\n";
		send(newsockfd,"0",MAXLINE,0);
		continue;
	}
		
	//Success
	cout<< "TransferDone: "<<size<<" bytes\n";
     close(newsockfd);
	}
     close(sockfd);
     return 0; 
}
