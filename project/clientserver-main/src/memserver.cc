#include "memserver.h"
#include "serverinterface.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include <iostream>
using std::cerr;
using std::endl;
using std::cout;
MemoryServer::MemoryServer(int port):server(port){
    
}
void MemoryServer::list_newsgroup(std::shared_ptr<Connection>& conn){
    unsigned char byte2 = conn->read();
    unsigned char byte3 = conn->read();
    unsigned char byte4 = conn->read();
    if (byte2 != MessageHandler::COM_END){
        //ERROR?
    }
    send_newsgroup(conn);
}
void MemoryServer::send_newsgroup(std::shared_ptr<Connection>& conn){
    conn->write(MessageHandler::ANS_LIST_NG); //8
    conn->write(newsgroup_list.size()); // 16
    //For i in newsgroup_list 
    //conn->write(i.id);    //40
    //conn->write(i.title) //(40 +8*N)* M 
    conn->write(MessageHandler::ANS_END);
    //then send padding
    //conn->write(0x00);
}

void MemoryServer::create_newsgroup(std::shared_ptr<Connection>& conn){
    unsigned byte2 = conn->read(); // string_p COM_END
    if (byte2 != MessageHandler::PAR_STRING){
        //Disconnect user
        //kill connection.
        return;
    }
    byte2 = conn->read();
    std::string sb = "";
    for (unsigned char i = 0; i < byte2; i++){
        byte2 = conn->read(); //this is the chars that should make a string.
        sb+=byte2;
    }
    if(conn->read() != MessageHandler::COM_END){
        //something is wrong. Kill connection
        return;
    }

    
    bool exists = true;
    conn->write(MessageHandler::ANS_CREATE_NG);
    if(exists){
    Newsgroup ng;
    //ng.created=now;
    //ng.id = static counter
    //ng.articles = empty vector;
    ng.name=sb;
    newsgroup_list.push_back(ng);
    conn->write(MessageHandler::ANS_ACK);
    conn->write(MessageHandler::ANS_END);
    conn->write(0x00);
    } else {
        conn->write(MessageHandler::ANS_NAK);
        conn->write(ERR_NG_ALREADY_EXIST);
        conn->write(MessageHandler::ANS_END);
    }
    
}

bool MemoryServer::isReady(){
    return server.isReady();
}
void MemoryServer::serve_one(){
    auto conn = server.waitForActivity();
    if (conn != nullptr) {
        try {
            process_request(conn);
        } catch (ConnectionClosedException&) {
            server.deregisterConnection(conn);
            cout << "Client closed connection" << endl;
        }
    } else {
        conn = std::make_shared<Connection>();
        server.registerConnection(conn);
        cout << "New client connects" << endl;
    }
}

void serve_one(ServerInterface& server){
    server.serve_one();
}

MemoryServer init(int argc, char* argv[]){
        if (argc != 2) {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[1]);
        } catch (std::exception& e) {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        MemoryServer server(port);
        if (!server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}

int main(int argc, char* argv[])
{
        auto server = init(argc, argv);

        while (true) {
            serve_one(server);
        }
        return 0;
}