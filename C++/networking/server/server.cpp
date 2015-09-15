/* A simple server in the internet domain using TCP
 The port number is passed as an argument
 This version runs forever, forking off a separate
 process for each connection
 */
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <sstream>


#include <sys/wait.h>	/* for the waitpid() system call */
#include <signal.h>	/* signal name macros, and the kill() prototype */



void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}



std::string readReq(int); //read entire request
std::string parseFirstLine(std::string); //cut out first line because we assume its a GET request so we only need to see the 1st line
std::string parseFileName(std::string); //then even more specifically parse the file name only
std::string serveReq(std::string,int); //this function serves the request based on file name and file descriptor
std::string getContentType(std::string); //this function takes care of figuring out the file type


void error(char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct sigaction sa;          
    
    if (argc < 2) {
	portno = 2222; 	 //if no argument is provided pass 2222 as default port
    }

    else
    portno=atoi(argv[1]); //otherwise take the argument as the port
  
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Welcome to Andrej and Shai's Server" << std::endl;
    std::cout << "We are running on port " << portno << std::endl;
	std::cout << "Now Serving..." << std::endl;
	
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //open the socket
    if (sockfd < 0)
    {
        error((char*)"ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    
    bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    
    listen(sockfd,256);
    
    clilen = sizeof(cli_addr);
    
    /****** Kill Zombie Processes ******/
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    /*********************************/
    

    //this loop is the server beginning to listen now on portno 
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //accept an incoming request
        
        if (newsockfd < 0)
             error((char*)"ERROR on accept");
        
        pid = fork(); //create a new process
        if (pid < 0)
             error((char*)"ERROR on fork");
        
        if (pid == 0)  { // fork() returns a value of 0 to the child process
            close(sockfd);
            
	
        std::cout << "Request! The request received was: " << std::endl;
    	//dump the request to the consol
    	//this is what the client requests of us
    	std::cout << " " << std::endl;
    	std::string requested = readReq(newsockfd);
    	std::cout << requested << std::endl;
    	std::cout << " " << std::endl;
    	std::cout << " " << std::endl;
	
	//take the request and snip out the first line
            std::string firstLine = parseFirstLine(requested);
    //then snip out the entire file name only
            std::string fileName = parseFileName(firstLine);    
	//finally serve the request
            std::string request = serveReq(fileName,newsockfd);
            close(newsockfd);
             //clean up and exit
            exit(0);
        }
        else //returns the process ID of the child process to the parent
            close(newsockfd); // parent doesn't need this
    } /* end of while */
    return 0; /* we never get here */
}


std::string readReq (int sock)
{
    int n;
    char buffer[256];
    
    bzero(buffer,256);
    n = read(sock,buffer,255);
    
    std::string tester(buffer);
    
    if (n < 0)  error((char*)"ERROR reading from socket");
   
    return tester;
}

std::string parseFirstLine(std::string toParse)
{
    int pos = toParse.find_first_of("\n");
	//find the first newline cut it off there
    
    std::string returner = toParse.substr(0,pos);
   
    return returner;
    
}

std::string parseFileName(std::string firstLineString)
{
    char* firstLine = new char[firstLineString.size() + 1];
    bzero((char*)firstLine, firstLineString.size()+1);
    strncpy(firstLine, firstLineString.c_str(), firstLineString.size());

    char *command = strtok( firstLine, " " );
    char *fileNameCstring = strtok( NULL, " " );

    std::string fileName;
    
    fileName = fileNameCstring;
    
    //default file name is index.html 
    if( !(strcmp("/", fileNameCstring)))
    {
        fileName = "index.html";
    }
    else
        fileName = fileName.substr(1, fileName.size()-1);
    
    return fileName;
}


std::string serveReq(std::string file,int sockfd)
{
    
    
    ////////BEGIN HEADER FORMATTING /////////
    
    //these are the default responses
    std::string version = "HTTP/1.1";
	std::string status = "200 OK";
	std::string nextLine = "\r\n";
    std::string space = " ";
    std::string contentType = getContentType(file);
    std::string header = "";
    std::string length = "";
    
    int contentLength = 0 ;
    struct stat filestatus;
    stat(file.c_str(), &filestatus );
    contentLength = filestatus.st_size;
    
    
    
    if( !(std::ifstream(file.c_str())) )
    {
        status = "404 File Not Found";
        file = "notHere.html";
        
        struct stat filestatus;
        stat(file.c_str(), &filestatus );
        int lengthNow = filestatus.st_size;
        
        contentLength = lengthNow;
        contentType = "text/html";
    }

	
    std::stringstream ss;
    ss<<contentLength;
    length = ss.str();
    header.append(version);
    header.append(space);
    header.append(status);
    header.append(nextLine); //sets up the line like HTTP/1.1 200 OK or 404
    

	 header.append("Connection: close"); //this is the closing line like in the book
    header.append(nextLine);
    
  header.append("Content-Type:");
    header.append(space);
    header.append(contentType);
    header.append(nextLine);
        //set up the content type line 



    if(contentLength > 0)
    {
        header.append("Content-Length:");
        header.append(space);
        header.append(length);
        header.append(nextLine); //sets up "Content Length: 6821" line if the file is there
    }
    //finish off with content length descrpt.
  
   
    header.append(nextLine);//finish off header
//////// BEGIN THE REST OF REQUEST HERE////////
    
    std::ifstream ifs;
std::string reqToServ;

std::cout << "Response! The response is: " << std::endl;
std::cout << " " << std::endl;
    std::cout<< header << std::endl; //dump header to console
   	
ifs.open( file.c_str(), std::ifstream::in | std::ios::binary);
	//open in binary mode the file we are asked to serve
     
     char c = ifs.get(); //run loop to take all the data in file
     while ( ifs.good() ) {
     reqToServ.push_back(c);
     c = ifs.get();
     }
     ifs.close(); //close the file desc
	
	const char *array = reqToServ.c_str(); //set up a char array with the data to be served

	int n = 1;
	//std:: cout << array;
	int r = 0;
    int cutOff=0;
    while(n<contentLength)
    { 
        r = send(sockfd,reqToServ.c_str(),reqToServ.length(),0);
        if(r>reqToServ.length())
        {
            cutOff=0;
        }
        else
            cutOff=r;
        reqToServ = reqToServ.substr(cutOff,reqToServ.length());
        if(reqToServ.empty())
            break;
        n+=r;
    }

	//write out the whole file to the clients FD
	//the problem here was with the large files but since my method sends a bit of the file at a time it ends up working great
  
    return ""; //i originally had this return a string so that later on if we are gonna use this server we can add a lil more info about the request being served
	//as of now it just returns "" 
}

std::string getContentType(std::string file)
{
    //standard html and text files
    if(file.find(".html") != std::string::npos)
		return "text/html";
	else if(file.find(".txt") != std::string::npos)
		return "text/plain";
    //Add in the support for GIF and JPEG images.
    else if(file.find(".gif") != std::string::npos)
		return "image/gif";
    else if(file.find(".GIF") != std::string::npos)
        return "image/gif";    
	else if(file.find(".jpg") != std::string::npos)
        return "image/jpeg";
    else if(file.find(".jpeg") != std::string::npos)
		return "image/jpeg";
    else if(file.find(".JPG") != std::string::npos)
		return "image/jpeg";
    else if(file.find(".JPEG") != std::string::npos)
		return "image/jpeg";
    
    return "text/html"; //if its none of these we will make the assumption it is text/html file but this can be changed to preference
}
