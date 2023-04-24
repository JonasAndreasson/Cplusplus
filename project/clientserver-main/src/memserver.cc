#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <functional>
#include "memserver.h"
#include "serverinterface.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
using std::cerr;
using std::endl;
using std::cout;
MemoryServer::MemoryServer(int port):server(port),newsgroup_list(){
}
void MemoryServer::list_newsgroup(std::shared_ptr<Connection>& conn){
    cout << "Entered NewsGroup\n";
    unsigned char byte2 = conn->read();
    if ((Protocol)byte2 != Protocol::COM_END){
        cout << "Expected COM_END" << '\n';
        //KILL CONNECTION
    }
    cout << "Sending response\n";
    send_newsgroup(conn);
}
void MemoryServer::send_newsgroup(std::shared_ptr<Connection>& conn){
    conn->write((unsigned char)Protocol::ANS_LIST_NG); //8
    conn->write((unsigned char)Protocol::PAR_NUM);
    auto size = newsgroup_list.size();
    send_N(conn, size);
    for (auto &newsgroup : newsgroup_list)
    {
        conn->write((unsigned char)Protocol::PAR_NUM);
        send_N(conn,newsgroup.id);
        conn->write((unsigned char)Protocol::PAR_STRING);
        send_N(conn,newsgroup.name.size());
        for (char &c : newsgroup.name){
            conn->write(c);
        }
    }
    conn->write((unsigned char)Protocol::ANS_END);
    cout << "Sending Termination code\n";
}

void MemoryServer::create_newsgroup(std::shared_ptr<Connection>& conn){
    unsigned char byte = conn->read(); // string_p COM_END
    if ((Protocol)byte != Protocol::PAR_STRING){
        cout << "Invalid start parameter";
        return;
    }
    unsigned int N = read_N(conn);
    cout << "Expecting length of "<< N << '\n';
    std::string sb = "";
    for (unsigned int i = 0; i < N; i++){
        byte = conn->read(); //this is the chars that should make a string.
        sb+=byte;
    }
    if((Protocol)conn->read() != Protocol::COM_END){
        return;
    }
    cout << "The name of the newsgroup is " << sb << '\n';
    
    bool exists = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&sb] (Newsgroup ng) -> bool { return sb == ng.name; }) != newsgroup_list.end();
    conn->write((unsigned char)Protocol::ANS_CREATE_NG);
    if(!exists){
    std::vector<Article> articles();
    Newsgroup ng = {sb,std::hash<std::string>{}(sb),time(NULL)};
    newsgroup_list.push_back(ng);
    conn->write((unsigned char)Protocol::ANS_ACK);
    conn->write((unsigned char)Protocol::ANS_END);
    } else {
        conn->write((unsigned char)Protocol::ANS_NAK);
        conn->write((unsigned char)Protocol::ERR_NG_ALREADY_EXISTS);
        conn->write((unsigned char)Protocol::ANS_END);
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