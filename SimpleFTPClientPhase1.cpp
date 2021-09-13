/*Designed by Ketan Karnakota*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <iostream>
using namespace std;

void error(const char *msg)
{
    cout<<(msg)<<"\n";
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       cout << "usage of: " <<argv[0]<< " <serverIPAddr:port> <fileToReceive>\n";
       exit(1);
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
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket\n");
    server = gethostbyname(ipad);
    if (server == NULL) {
        cout << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ 
        cout<<"ERROR connecting to "<<ipad<<"\n";
		exit(2);
	}
    else cout<<"ConnectDone: "<<ipad<<":"<<portnum<<"\n";

 	
	
	int MAXLINE=256;int lol=0;
	char char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int lSize,num_blks,num_last_blk,j;
 	FILE *fp;
	recv(sockfd,message,MAXLINE,0);
	if(strcmp("1",message)==0){
		if((fp=fopen(argv[2],"w"))==NULL){
			cout<<"Error to create file/ Write to the file/n";
			exit(11);
		}
		else
		{
			recv(sockfd, char_num_blks, MAXLINE,0);
			num_blks=atoi(char_num_blks);
			for(j= 0; j < num_blks; j++) { 
				recv(sockfd, buffer, MAXLINE,0);
				lol+=fwrite(buffer,sizeof(char),MAXLINE,fp);
				//cout<<buffer<<endl;
			}
			recv(sockfd, char_num_last_blk, MAXLINE,0);
			num_last_blk=atoi(char_num_last_blk);
			if (num_last_blk > 0) { 
				recv(sockfd, buffer, MAXLINE,0);
				lol+=fwrite(buffer,sizeof(char),num_last_blk,fp);
				//cout<<buffer<<endl;
			}
			fclose(fp);
			//cout<<"File download done."<<endl;
		}
	}
	else if(strcmp("2",message)==0){
		cerr<<"File not correct at server\n";
		exit(12);
	}

	cout<<"FileWritten: "<<lol<<" bytes\n";
    close(sockfd);
    return 0;
}
