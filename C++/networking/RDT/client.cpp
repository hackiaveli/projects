#include "connection.h"
#define MAX_TIMEOUT 1000
class Client {   
public:
    Client(string host,int port,int loss,int corr);
    bool connectToServ();
    bool requestFile(string file);
    string waitForData();
    void closeConnection(bool sendFin);
private:
    sockaddr_in m_ServerAddr;
    int m_sock;
    int m_lastSeq;
    int m_lastAck;
    string m_hostname;
    int m_portno;
    int m_prob_loss;
    int m_prob_corr;
};



Client::Client(string host,int port,int loss,int corr) {
    m_sock = 0;
    m_hostname = host;
    m_portno = port;
    m_prob_loss = loss;
    m_prob_corr = corr;
    m_lastSeq=0;
    m_lastAck=0;
}


bool Client::connectToServ() {
    sockaddr_in ServerAddr;
    
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    fcntl(sock,F_SETFL,O_NONBLOCK);
    m_sock  = sock;
    
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(m_portno);
    ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    m_ServerAddr = ServerAddr;
    //set us up, and the last line says we are requesting the SERVER_IP which we set to local host
    //its defined at the top of the file
    
    
    //prepare a packet and header to send, it will be a SYN packet
    header h;
    packet p;
    p.type= SYN;
    h.length = strlen(p.data);
    p.my_head = h;
    p.seq = 0;
    p.ack = 0;
    
    //send first packet to the server
    if (sendto(sock,(const void *)&p,sizeof(packet),0,(sockaddr*)&ServerAddr,sizeof(sockaddr_in)) <= 0) {return false;}
    else {
        cerr << "Client: SYN sent" << endl;
    }
    
    //now we wait for the response to come to us
    
    packet rec;
    
    
    int keepGoing = 0;
    while (keepGoing == 0) {
        keepGoing = recvfrom(sock,(void *)&rec,(sizeof(packet)),0,NULL,NULL) ;//we open this recfrom
        //and keep rec in a loop
        
        if(rec.type == SYNACK) {//we check each time we get a data piece
            cerr << "Client: SYNACK received" <<endl;
            return true; //connect
        } else { //this means they are equal we got a synack so we good to go
            keepGoing = 0; 
            //if they are not equal in that case we want to keep going in the loop
            //so reset keepgoing back to 0 and go again
        }
    }
    return false;
}

bool Client:: requestFile(string file) {   
    packet p;
    header h;
    strcpy(p.data,file.c_str());
    p.type = DATA_REQ;
    p.seq = 0;
    p.ack = 0;
    h.length = file.length();
    p.my_head = h;

    sendto(m_sock,(const void *)&p,sizeof(packet),0,(sockaddr*)&m_ServerAddr,sizeof(sockaddr_in));
    cerr<<"Client: DATA sent seq#" <<p.seq<< ", ACK#"<<p.ack<<", FIN 0, content-length: "<<p.my_head.length <<endl;
    cerr <<"......................................"<<endl;
    return true;
}

string Client ::waitForData() {
    packet rec;
    int keepGoing = 0;
    vector<packet> packArray;
    int countPackets = 0;
    int numPackets = 0;
    
    //keep track of what seq we should be on
    int curr_ack = 0;

    srand(time(NULL));
    while (keepGoing == 0) {
        keepGoing = recvfrom(m_sock,(void *)&rec,(sizeof(packet)),0,NULL,NULL);
        
        ////cerr << "keep going got " << keepGoing << endl;
        
        if(rec.type == DATA_SENDING) {//once we get a hit add this packet to array
            if(curr_ack == rec.seq) {// add to our array if its in order
                cerr<<"Client: DATA received seq#" <<rec.seq<< ", ACK#"<<rec.ack<<", FIN 0, content-length: "<<rec.my_head.length <<endl;
                int random = rand()%100+1;
                int randomC = rand()%100+1;
                if(random > m_prob_loss || randomC> m_prob_corr) {
                    //prepare ack packet
                    packet p;
                    p.type = ACK;
                    curr_ack += rec.my_head.length;
                    p.ack = curr_ack;
                    p.seq=rec.ack;
                    p.my_head.length = 1;

                     m_lastSeq = p.seq;
                     m_lastAck = p.ack;

                    packArray.push_back(rec);
                    sendto(m_sock,(const void *)&p,sizeof(packet),0,(sockaddr*)&m_ServerAddr,sizeof(sockaddr_in)); //send the ACK packet
                    cerr<<"Client: ACK sent seq#" <<p.seq<< ", ACK#"<<p.ack<<", FIN 0, content-length: "<<p.my_head.length <<endl;
  
                    
                    countPackets++;
    
                    keepGoing = 0;
                } else {
                   cerr<<"Client: ACK sent seq#" << m_lastSeq << ", ACK#"<< m_lastAck <<", FIN 0, content-length: 1"<<endl;

                }
            }  
        }
        if(rec.type ==BADFILE)
        {
            cerr << "No Such File Exists on Server...Exiting" << endl;
            closeConnection(false);
            exit(0);
        }
        keepGoing = 0;
        if(rec.numPacketsInFile !=0){
            numPackets = rec.numPacketsInFile;
        } 
        if(countPackets==numPackets) {
            cerr <<"Client: File transfer complete"<<endl;
            break;
        }   
    }
 
    string file;
    
    for (std::vector<packet>::iterator it = packArray.begin() ; it != packArray.end(); ++it) {
        file.append(it->data);
    }
    return file;
}


void Client::closeConnection(bool sendFin) {
    packet p;
    p.type = FIN;
    p.seq =m_lastSeq+1; 
    p.ack= m_lastAck;
   // int timeout = 0;
   
   if(!sendFin)
   {
       cout << "File Not Found Exiting..." << endl;
       exit(0);
   }
   
    while(true/* && timeout < MAX_TIMEOUT*/) {
        if(sendto(m_sock,(const void *)&p,sizeof(packet),0,(sockaddr*)&m_ServerAddr,sizeof(sockaddr_in)) < 0) {
           // timeout++;
            continue;
        } else {
            cerr << "Client: FIN sent seq#"<< p.seq<<", ACK#"<< p.ack<<", FIN 1, content-length:1" << endl;
            break;
        }
    }
   
    
    packet rec;
    int exitLoop = 0;
    //timeout = 0;
    while(exitLoop ==0 /*&& timeout < MAX_TIMEOUT*/){
        exitLoop = recvfrom(m_sock,(void *)&rec,(sizeof(packet)),0,NULL,NULL);
        if(rec.type != FINACK) {
            exitLoop= 0; //strcmp returns 0 if they ARE equal, so this will return a non-zero number
            //if they are not equal in that case we want to keep going in the loop
            //so reset keepgoing back to 0 and go again
           // timeout++;
        } else {
            cerr << "Client: FINACK received seq#"<< rec.seq<<", ACK#"<< rec.ack<<", FIN 1, content-length:1" << endl;
            packet ack;
            ack.type=LASTACK;
            ack.seq=p.seq+1;
            ack.ack=p.ack+1;
            //int timeout2 = 0;
            while(true /*&& timeout2 <MAX_TIMEOUT*/) {
                int get=sendto(m_sock,(const void *)&ack,sizeof(packet),0,(sockaddr*)&m_ServerAddr,sizeof(sockaddr_in));
                cerr << "Client: ACK sent seq#"<< ack.seq<<", ACK#"<< ack.ack<<", FIN 1, content-length:1" << endl;
                if (get <= 0) {
                   // timeout2++;
                    continue;
                }
                else {
                    memset( &m_ServerAddr, 0, sizeof(m_ServerAddr));
                    close(m_sock);
                     cerr<< "Client: Close connection" <<endl;
                    break;
                }
            }
            
        }
    }
}


int main (int argc, char *argv[]){   
    cerr << "Welcome to Flashman Wade RDT: Client" << endl;
   
    string hostname;
    int portno;
    int prob_loss;
    int prob_corr;
    string filename;
        
    for (int i=1; i< argc; i++){
        if(i==1) {//looking for host name 
           hostname = argv[i];
        } else if(i==2) {//looking for host port no
            portno = atoi(argv[i]);
        } else if(i==3) {//looking for prob loss
            prob_loss = atoi(argv[i]);
        } else if(i==4){ //looking for prob corr
            prob_corr =atoi(argv[i]);
        } else if(i==5) {//looking for filename
            filename = argv[i];
        }
    }
        
    Client *my_connection = new Client(hostname,portno,prob_loss,prob_corr);
             
    if(!my_connection->connectToServ()) {
            cerr<<"Client: Connection to server failed"<<endl;
            return 0;
        } 
        my_connection->requestFile(filename); 
        
        string file = my_connection->waitForData();
        cout << file;
        
        my_connection->closeConnection(true);
        return 0;
   
}




