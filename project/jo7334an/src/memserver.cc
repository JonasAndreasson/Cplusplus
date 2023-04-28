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
        conn->~Connection();
        return;
    }
    cout << "Sending response\n";
    send_newsgroup(conn);
    cout << "Reponse sent\n";
}
void MemoryServer::send_newsgroup(std::shared_ptr<Connection>& conn){
    conn->write((unsigned char)Protocol::ANS_LIST_NG);
    conn->write((unsigned char)Protocol::PAR_NUM);
    auto size = newsgroup_list.size();
    send_N(conn, size);
    for (auto &newsgroup : newsgroup_list)
    {
        conn->write((unsigned char)Protocol::PAR_NUM);
        send_N(conn,newsgroup.id);
        send_string_p(conn, newsgroup.name);
    }
    conn->write((unsigned char)Protocol::ANS_END);
}

void MemoryServer::create_newsgroup(std::shared_ptr<Connection>& conn){
    unsigned char byte = conn->read();
    if ((Protocol)byte != Protocol::PAR_STRING){
        cout << "Invalid start parameter";
        conn->~Connection();
        return;
    }
    unsigned int N = read_N(conn);
    std::string sb = "";
    for (unsigned int i = 0; i < N; i++){
        byte = conn->read(); 
        sb+=byte;
    }
    if((Protocol)conn->read() != Protocol::COM_END){
        conn->~Connection();
        return;
    }
    
    bool exists = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&sb] (Newsgroup ng) -> bool { return sb == ng.name; }) != newsgroup_list.end();
    conn->write((unsigned char)Protocol::ANS_CREATE_NG);
    if(!exists){
    std::vector<Article> articles();
    Newsgroup ng = {sb,(uint32_t)std::hash<std::string>{}(sb),std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()};
    newsgroup_list.push_back(ng);
    conn->write((unsigned char)Protocol::ANS_ACK);
    conn->write((unsigned char)Protocol::ANS_END);
    } else {
        conn->write((unsigned char)Protocol::ANS_NAK);
        conn->write((unsigned char)Protocol::ERR_NG_ALREADY_EXISTS);
        conn->write((unsigned char)Protocol::ANS_END);
    }
}

void MemoryServer::remove_newsgroup(std::shared_ptr<Connection>& conn){
    if ((Protocol)conn->read() != Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    unsigned int id = read_N(conn);
    if ((Protocol)conn->read() != Protocol::COM_END){
        conn->~Connection();
        return;
    }
    auto it = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&id] (Newsgroup ng) -> bool { return id == ng.id; });
    bool exists = it != newsgroup_list.end();
    conn->write((unsigned char)Protocol::ANS_DELETE_NG);
    if (exists){
        newsgroup_list.erase(it);
        conn->write((unsigned char)Protocol::ANS_ACK);
    } else {
        conn->write((unsigned char)Protocol::ANS_NAK);
        conn->write((unsigned char)Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char)Protocol::ANS_END);
}
void MemoryServer::list_article(std::shared_ptr<Connection>& conn){
    if ((Protocol)conn->read() != Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    unsigned int id = read_N(conn);
    if ((Protocol)conn->read() != Protocol::COM_END){
        conn->~Connection();
        return;
    }
    auto it = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&id] (Newsgroup ng) -> bool { return id == ng.id; });
    bool exists = it != newsgroup_list.end();
    conn->write((unsigned char)Protocol::ANS_LIST_ART);

    if (exists){
        conn->write((unsigned char)Protocol::ANS_ACK);
        conn->write((unsigned char)Protocol::PAR_NUM);
        unsigned int size = (*it).articles.size();
        send_N(conn,size);
        for (Article& a : (*it).articles){
            conn->write((unsigned char)Protocol::PAR_NUM);
            send_N(conn,a.id);
            send_string_p(conn, a.title);
        }
    } else{
        conn->write((unsigned char)Protocol::ANS_NAK);
        conn->write((unsigned char)Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char)Protocol::ANS_END);
}
void MemoryServer::create_article(std::shared_ptr<Connection>& conn){
    if ((Protocol)conn->read()!=Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    long unsigned int news_group_id = read_N(conn);
    cout << news_group_id << '\n';
    for (Newsgroup ng : newsgroup_list){
        cout << ng.id <<'\n';
    }
    if ((Protocol)conn->read()!=Protocol::PAR_STRING){
        conn->~Connection();
        return;
    }
    auto title_N = read_N(conn);
    std::string title = "";
    for (unsigned int i =0; i<title_N;++i){
        title+=conn->read();
    }

    if ((Protocol)conn->read()!=Protocol::PAR_STRING){
        conn->~Connection();
        return;
    }
    auto author_N = read_N(conn);
    std::string author = "";
    for (unsigned int i =0; i<author_N;++i){
        author+=conn->read();
    }

    if ((Protocol)conn->read()!=Protocol::PAR_STRING){
        conn->~Connection();
        return;
    }
    auto text_N = read_N(conn);
    std::string text = "";
        for (unsigned int i =0; i<text_N;++i){
        text+=conn->read();
    }
    if((Protocol)conn->read()!=Protocol::COM_END){
        conn->~Connection();
        return;
    }

    auto it = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&news_group_id] (Newsgroup ng) -> bool { return news_group_id == ng.id; });
    bool exists = it != newsgroup_list.end();
    conn->write((unsigned char) Protocol::ANS_CREATE_ART);
    if (exists){
        conn->write((unsigned char) Protocol::ANS_ACK);
        Article art{title, author, text, (uint32_t)std::hash<std::string>{}(text),std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()};
        (*it).articles.push_back(art);
    } else {
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char) Protocol::ANS_END);
}
void MemoryServer::delete_article(std::shared_ptr<Connection>& conn){
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    auto news_group_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    auto article_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::COM_END){
        conn->~Connection();
        return;
    }
    auto ng_it = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&news_group_id] (Newsgroup ng) -> bool { return news_group_id == ng.id; });
    bool exists = ng_it != newsgroup_list.end();
    conn->write((unsigned char) Protocol::ANS_DELETE_ART);
    if (exists){
        auto art_it = std::find_if((*ng_it).articles.begin(),(*ng_it).articles.end(),[&article_id] (Article art) -> bool { return article_id == art.id; });
        if(art_it!=(*ng_it).articles.end()){
            (*ng_it).articles.erase(art_it);
            conn->write((unsigned char) Protocol::ANS_ACK);    
        } else {
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    } else {
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char) Protocol::ANS_END);
}
void MemoryServer::get_article(std::shared_ptr<Connection>& conn){
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    auto news_group_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        conn->~Connection();
        return;
    }
    auto article_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::COM_END){
        conn->~Connection();
        return;
    }
    auto ng_it = std::find_if(newsgroup_list.begin(),
             newsgroup_list.end(), 
             [&news_group_id] (Newsgroup ng) -> bool { return news_group_id == ng.id; });
    bool exists = ng_it != newsgroup_list.end();
    conn->write((unsigned char) Protocol::ANS_GET_ART);
    if (exists){
        auto art_it = std::find_if((*ng_it).articles.begin(),(*ng_it).articles.end(),[&article_id] (Article art) -> bool { return article_id == art.id; });
        if(art_it!=(*ng_it).articles.end()){
            conn->write((unsigned char) Protocol::ANS_ACK);
            send_string_p(conn, (*art_it).title);
            send_string_p(conn, (*art_it).author);
            send_string_p(conn, (*art_it).text);
        } else {
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    } else {
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char) Protocol::ANS_END);
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