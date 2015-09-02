#include "connection.h"


class Server {
public:
    Server(int port,int window,int prob_loss,int prob_corr);
    bool acceptConnections();
    string waitReq();
    string getFileData(string fileName);
    bool sendFile(string file);
    void closeConnection(bool finReceived);
private:
    sockaddr_in m_ClientAddr;
    int m_sock;
    int m_portno;
    int m_window;
    int m_prob_loss;
    int m_prob_corr;
    int m_firstAck;
};


Server::Server(int port,int window,int prob_loss,int prob_corr)
{
    m_sock = 0;
    m_portno = port;
    m_window = window;
    m_prob_loss = prob_loss;
    m_prob_corr = prob_corr;
    m_firstAck = 0;
}


bool Server::acceptConnections() {
    sockaddr_in LocalAddr;
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    fcntl(sock,F_SETFL,O_NONBLOCK);
    m_sock = sock;
    
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_port = htons(m_portno);
    LocalAddr.sin_addr.s_addr = INADDR_ANY;
    
    //get our info on the local server, this is us
    //the last line says i will accept connects on ANY IP addr coming
    
    if (bind(sock,(sockaddr*)&LocalAddr,sizeof(sockaddr))) {
        cout << "Binding failed! " << endl;
        return false;
        
    } else {
        cout << "Sender: Listening on port " <<m_portno << endl; //bind it, we are listening
    }
    
    
    sockaddr_in ClientAddr;
    socklen_t i = sizeof(sockaddr);
    
    packet p;
    header h;
    //int length;
    
    int get = 0;
    while(get == 0) {//now we are running a loop until we get a SYN packet
        //this is how we know we are dealin with a real request
        // recieve first packet and get client address
        get = recvfrom(sock,(void *)&p,sizeof(packet),0,(sockaddr*)&ClientAddr,&i); //allow a request, and save that clients info into Client Addr
        
        if(get != 0){
            if(p.type == SYN) {//check if its a syn
                //strcmp returns 0 if they are EQUAL so this statement just says
                //if they are not equal then go into this bracket
              
                cout << "Sender: SYN received"<< endl;
            }
            else {
                 get=0;
            }
        }
        
        
    }
    
    m_ClientAddr=ClientAddr;
    
    int size = p.my_head.length; //this is just for testing
    //but its good to know we can access the headers info
    
    // send response
    char data [DATA_SIZE];
    memset(data,'\0',sizeof(data));
    //prepare a packet to send back
    //its a synack
    packet send;
    send.type=SYNACK;
    
    if (sendto(sock,(const void *)&send,(sizeof(packet)),0,(sockaddr*)&ClientAddr,sizeof(sockaddr_in)) > 0) {//send to our client the connection
        cout << "Sender: SYNACK sent"<< endl;
        return true; //return true cuz we are good to go
    }
    
    return false;
    
}

string Server::waitReq() {
    
    int waitReq = 0; //set up variable to keep the loop going
    socklen_t i = sizeof(sockaddr);
    packet p;
    while(waitReq==0) {//stay in loop till we get a DATA_REQ packet
        waitReq = recvfrom(m_sock,(void *)&p,sizeof(packet),0,(sockaddr*)&m_ClientAddr,&i); //keep accepting
        if(p.type == FIN) {
            cout << "Sender: FIN received seq#"<<p.seq<<", ACK#"<< p.ack<<", FIN 1, content-length:1" << endl;
            closeConnection(true);
            return "";
        }
        else if(p.type==DATA_REQ){ //if its not a file request type fuhgetaboutit
            cout<<"Sender: DATA received seq#" <<p.seq<< ", ACK#"<<p.ack<<", content-length: "<<p.my_head.length <<endl;
            cout<< "Sender: File requested is \""<< p.data<<"\""<<endl;
            cout <<"......................................"<<endl;
            m_firstAck = p.my_head.length;
        } else {
            waitReq = 0;
        }
        
    }
    return p.data; //if we get to here it means we have a valid packet
    //return this string, this will be the FILE the user requested
    
}

string Server::getFileData(string fileToServe) {
    
    //this will call the servers wait for a file request function
    //after we recieve a file request we store it in fileToServe string
    //setlocale(LC_ALL, "");
    
    
    
    std::ifstream in(fileToServe.c_str()); //open file
    
    if(!in)
    {
    
        
        cout << "No Such File Exists in this Directory ... Exiting" << endl;
        packet p;
        p.type = BADFILE;
        sendto(m_sock,(const void *)&p,(sizeof(packet)),0,(sockaddr*)&m_ClientAddr,sizeof(sockaddr_in));
        //closeConnection(true);
        exit(0);
        
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    
    std::string contents(buffer.str()); //read the file into string
    //called contents
    
    //string contents
    // will now have the whole file
    //uncomment this line below to see the whole file displayed
    // cout << contents  << endl;
    
    //note if the type is not .txt we will need to add the
    //std::ios::binary flag
    //to the opening of the file so that we can serve binary contents too
    
    return contents;
}



bool Server::sendFile(string file) {

    
    
    int base=0; //base+WINDOW_SIZE-1 last packet in window
    int nextAvailSeqNum=0;
    bool resend = false;
    
    double size = file.length();
    double test = size/(DATA_SIZE-1);
    int packArray = size/(DATA_SIZE-1);
    
    double result = packArray - test;
    if(result < 0) {
        packArray++; //Number of packets total to send
    }

    packet arrayPackets[packArray]; //Array of packets
    
    string tempFile = "";
    int i=0;
    int currAck = m_firstAck;
    while(i<packArray) {
        packet p;
        p.type = DATA_SENDING;
        p.numPacketsInFile = 0;
        tempFile = file.substr((i*(DATA_SIZE-1)),(DATA_SIZE-1));
        strcpy(p.data,tempFile.c_str());
        p.seq = nextAvailSeqNum;
        p.my_head.length = sizeof(packet);

        p.ack = currAck;
        
        p.sendTime.tv_sec=0;
        p.sendTime.tv_usec=0;
        arrayPackets[i] = p;

        i++;
        currAck++;
        nextAvailSeqNum+= sizeof(packet);
    }
    
    //First packet sends number of packets to be expected
    arrayPackets[0].numPacketsInFile = packArray;
    srand(time(NULL));

    if(packArray <= m_window) {
        m_window = packArray;
    }
    
    for(int j = 0; j<m_window;j++) //send the first window out
    {
        //  cout<< "Enter sending window loop"<<endl;
        timeval check;
        gettimeofday(&check,NULL);
  
        arrayPackets[j].sendTime.tv_usec = check.tv_usec;

        sendto(m_sock,(const void *)&arrayPackets[j],(sizeof(packet)),0,(sockaddr*)&m_ClientAddr,sizeof(sockaddr_in));
        cout<<"Sender: DATA sent seq#" <<arrayPackets[j].seq<< ", ACK#"<<arrayPackets[j].ack<<", content-length: "<<arrayPackets[j].my_head.length <<endl;
    }
    
    //wait on base_ack
    
    int packetsDelivered = 0;
    int basePacket = 0;
    
    //int timeout=0;
    while (packetsDelivered < packArray) { //until all packets sent

        timeval now;
        gettimeofday(&now,NULL);
        
        long timeMicro = now.tv_usec; //current time

        //check if our base packet timer ran out
        int diff = timeMicro - arrayPackets[basePacket].sendTime.tv_usec;

        //Watch out for overflow!
        if(diff > MAX_TIMOUT_USEC || diff <0)
        {
            cout << "Sender: (ACK lost or corrupted) Timeout"<< endl;
            
            //our timer ran out for the base packet so must resend all packets in the window and change their times sent
            
            int l = basePacket;
            int b = basePacket;
            for(l;l<(b+m_window);l++) {
                if(l< packArray) {
                    timeval now;
                    gettimeofday(&now,NULL);
                    
                    
                    long timeMicro = now.tv_usec; //current time
                    arrayPackets[l].sendTime.tv_usec = timeMicro; //set it
                    
                    sendto(m_sock,(const void *)&arrayPackets[l],(sizeof(packet)),0,(sockaddr*)&m_ClientAddr,sizeof(sockaddr_in)); //send it
                    cout<<"Sender: DATA sent seq#" <<arrayPackets[l].seq<< ", ACK#"<<arrayPackets[l].ack<<", content-length: "<<arrayPackets[l].my_head.length <<endl;
                }
                
            }
            
            
        }
        
        packet p;
        socklen_t i = sizeof(sockaddr);
        int got = recvfrom(m_sock,(void *)&p,sizeof(packet),0,(sockaddr*)&m_ClientAddr,&i);
      
        
        if(p.type == FIN){
            cout << "Sender: FIN received seq#"<<p.seq<<", ACK#"<< p.ack<<", FIN 1, content-length:1" << endl;
            closeConnection(true);
            return true;
        }
        
        else if(p.type == ACK /*&& timeout <MAX_TIMEOUT*/) {
            //cout << "p.seq-m_firstAck: " <<p.seq-m_firstAck<< " basePacket: " <<basePacket<<endl;
            //timeout++;
            if(p.seq == basePacket+m_firstAck) {//got the expected packet
                //timeout = 0;
                cout<<"Sender: ACK received seq#" <<p.seq<< ", ACK#"<<p.ack<<", content-length: "<<p.my_head.length <<endl;
                packetsDelivered++;
                basePacket++;

                cout<<"Sender: Sliding window"<<endl;
            
                //send the base+window packet
                int random = rand()%100+1;
                int randomC = rand()%100+1;
                if(random >m_prob_loss && randomC > m_prob_corr){

                    int nextpkt = basePacket+(m_window-1);
        
                    if(nextpkt < packArray) {
                        timeval now; //get the time of this packet to be sent next
                        gettimeofday(&now,NULL);
                        arrayPackets[nextpkt].sendTime.tv_usec = now.tv_usec;


                        sendto(m_sock,(const void *)&arrayPackets[nextpkt],(sizeof(packet)),0,(sockaddr*)&m_ClientAddr,sizeof(sockaddr_in));
                        cout<<"Sender: DATA sent seq#" <<arrayPackets[nextpkt].seq<< ", ACK#"<<arrayPackets[nextpkt].ack<<", content-length: "<<arrayPackets[nextpkt].my_head.length <<endl;

                    }
                }   
            }
        }
        
    }
    cout << "Sender: File transfer complete" << endl;
    return true;
    
}


void Server::closeConnection(bool finReceived){

    packet p;
    socklen_t i = sizeof(sockaddr);
    //int timeout=0;
    if(!finReceived){
    while(true/* && timeout <MAX_TIMEOUT*/) {
        recvfrom(m_sock,(void *)&p,sizeof(packet),0,(sockaddr*)&m_ClientAddr,&i);
        if(p.type == FIN) {
            cout << "Sender: FIN received seq#"<<p.seq<<", ACK#"<< p.ack<<", FIN 1, content-length:1" << endl;
            break;
        } else {
            //timeout++;
            continue;
        }
    }
    }

    packet fAck;
    fAck.type=FINACK;
    fAck.seq=p.ack;
    fAck.ack = p.seq;
   // timeout = 0;
    while(true /*&& timeout < MAX_TIMEOUT*/) {
        if( sendto(m_sock,(const void *)&fAck,(sizeof(packet)),0,(sockaddr*)&m_ClientAddr,sizeof(sockaddr_in)) <0){
            //timeout++;
            continue;
        } else {
            cout << "Sender: FINACK sent seq#"<< fAck.seq<<", ACK#"<< fAck.ack<<", FIN 1, content-length:1" << endl;
            break;
        }
    }
    
    packet ack;
    i = sizeof(sockaddr);
    //timeout=0;
    while(true /*&& timeout<MAX_TIMEOUT*/) {
        recvfrom(m_sock,(void *)&ack,sizeof(packet),0,(sockaddr*)&m_ClientAddr,&i);
        if(ack.type == LASTACK) {
            cout << "Sender: ACK received seq#"<< ack.seq<<", ACK#"<< ack.ack<<", FIN 1, content-length:1" << endl;
            memset( &m_ClientAddr, 0, sizeof(m_ClientAddr));
            close(m_sock);
            cout<<"Sender: Close connection"<<endl;
            break;
        } else {
            //timeout++;
            continue;
        }
    }  
    
}


int main (int argc, char *argv[]){  
    int portno;
    int window;
    int prob_loss;
    int prob_corr;
    cout << "Welcome to Flashman Wade RDT: Server" << endl;
        
        
    for (int i=1; i< argc; i++) {
        if(i==1) {//looking for host name
            portno = atoi(argv[i]);
        } else if(i==2) {//looking for host port no
            window =atoi(argv[i]);
        } else if(i==3) {//looking for prob loss
            prob_loss = atoi(argv[i]);
        } else if(i==4) {//looking for prob corr
            prob_corr =atoi(argv[i]);
        }  
    }
       
    Server *my_server = new Server(portno,window,prob_loss,prob_corr);

    my_server->acceptConnections();
        
    string fileToServe = my_server->waitReq();
    string file =  my_server->getFileData(fileToServe);

        
    my_server->sendFile(file);

    my_server->closeConnection(false);
    return 0;
}