#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <ios>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <string>
#include <functional>
#include "diskserver.h"
#include "serverinterface.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
using std::cerr;
using std::endl;
using std::cout;
using std::ofstream;

size_t DiskServer::database_size(){
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir("./database"); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_DIR) { /* If the entry is a regular file */
            file_count++; 
        }
    }
    closedir(dirp);
    return file_count-2;
}


int DiskServer::get_line_newsgroup(uint32_t id){}
int DiskServer::get_line_article(uint32_t id){}
void DiskServer::delete_article(int line){}
void DiskServer::add_article(Article art){}
void DiskServer::add_newsgroup(Newsgroup ng){}
void DiskServer::delete_newsgroup(int line){}
std::vector<ServerInterface::Article> DiskServer::articles_related_to_newsgroup(int line){}
std::vector<ServerInterface::Newsgroup> DiskServer::newsgroup_list(){
    std::vector<ServerInterface::Newsgroup> list;
    std::string line;
    std::ifstream file;
    DIR * dirp, *sbdir;
    struct dirent * entry;
    dirp = opendir("./database");
    while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_DIR) { /* If the entry is a regular file */
        std::string subpath = entry->d_name;
        if (subpath == "." || subpath == ".."){

        } else {
        subpath = "./database/" + subpath;
        std::string name;
        std::string buffer;
        cout << subpath <<endl;
        file.open(subpath+"/name");
        while (file >> buffer){
            name += buffer + " ";
        }
        name = name.substr(0,name.size()-1);
        file.close();
        time_t created;
        file.open(subpath+"/created");
        file >> created;
        file.close();
        cout << name << endl;
        Newsgroup ng = {name,(uint32_t)std::hash<std::string>{}(name),created};
        list.push_back(ng);
        }
    }
    }
    closedir(dirp);
    return list;
}
std::string DiskServer::get_article_title(int line){}
std::string DiskServer::get_article_author(int line){}
std::string DiskServer::get_article_text(int line){}


DiskServer::DiskServer(int port):server(port){
}
void DiskServer::list_newsgroup(std::shared_ptr<Connection>& conn){
    cout << "Entered NewsGroup\n";
    unsigned char byte2 = conn->read();
    if ((Protocol)byte2 != Protocol::COM_END){
        cout << "Expected COM_END" << '\n';
        //KILL CONNECTION
    }
    cout << "Sending response\n";
    send_newsgroup(conn);
    cout << "Reponse sent\n";
}
void DiskServer::send_newsgroup(std::shared_ptr<Connection>& conn){
    conn->write((unsigned char)Protocol::ANS_LIST_NG); //8
    conn->write((unsigned char)Protocol::PAR_NUM);
    auto size = database_size();
    send_N(conn, size);
    for (auto &newsgroup : newsgroup_list())
    {
        cout << newsgroup.id << endl;
        cout << newsgroup.name << endl;
        conn->write((unsigned char)Protocol::PAR_NUM);
        send_N(conn,newsgroup.id);
        send_string_p(conn, newsgroup.name);
    }
    conn->write((unsigned char)Protocol::ANS_END);
}

void DiskServer::create_newsgroup(std::shared_ptr<Connection>& conn){
    unsigned char byte = conn->read(); // string_p COM_END
    if ((Protocol)byte != Protocol::PAR_STRING){
        cout << "Invalid start parameter";
        return;
    }
    unsigned int N = read_N(conn);
    std::string sb = "";
    for (unsigned int i = 0; i < N; i++){
        byte = conn->read(); //this is the chars that should make a string.
        sb+=byte;
    }
    if((Protocol)conn->read() != Protocol::COM_END){
        cout << "Expected Com_END" << endl;
        return;
    }
    cout << "Trying to create dir" << endl;
    std::string path = "database/"+std::to_string((uint32_t)std::hash<std::string>{}(sb));
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    perror("mkdir");
    bool exists = ng_index == -1;
    conn->write((unsigned char)Protocol::ANS_CREATE_NG);
    if(!exists){
        ofstream file(path+"/name");
        file << sb<< endl;
        file.close();
        file.open(path+"/created");
        file <<time(NULL) << endl;
        file.close();
        conn->write((unsigned char)Protocol::ANS_ACK);
        conn->write((unsigned char)Protocol::ANS_END);
    } else {
        conn->write((unsigned char)Protocol::ANS_NAK);
        conn->write((unsigned char)Protocol::ERR_NG_ALREADY_EXISTS);
        conn->write((unsigned char)Protocol::ANS_END);
    }
}
void rm_subdir(std::string path){
    DIR * dirp, *sbdir;
    struct dirent * entry;
    dirp = opendir(path.c_str());
    while ((entry = readdir(dirp)) != NULL) {
        std::string subpath = entry->d_name;
        if (entry->d_type == DT_DIR) { /* If the entry is a regular file */
            if (subpath == "." || subpath == ".."){
                    
            } else {
                subpath = path +"/"+ subpath;
                rm_subdir(subpath);
            }
        } else if (entry->d_type == DT_REG){
            remove((path+"/"+subpath).c_str());
        }
    }
    closedir(dirp);
    rmdir(path.c_str());
}
void DiskServer::remove_newsgroup(std::shared_ptr<Connection>& conn){
    if ((Protocol)conn->read() != Protocol::PAR_NUM){
        return;
    }
    unsigned int id = read_N(conn);
    if ((Protocol)conn->read() != Protocol::COM_END){
        return;
    }
    std::string path = "database/"+std::to_string(id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    conn->write((unsigned char)Protocol::ANS_DELETE_NG);
        if (exists){
        rm_subdir(path);
        conn->write((unsigned char)Protocol::ANS_ACK);
    } else {
        rmdir(path.c_str());
        conn->write((unsigned char)Protocol::ANS_NAK);
        conn->write((unsigned char)Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char)Protocol::ANS_END);
}
void DiskServer::list_article(std::shared_ptr<Connection>& conn){
    if ((Protocol)conn->read() != Protocol::PAR_NUM){
        return;
    }
    unsigned int id = read_N(conn);
    if ((Protocol)conn->read() != Protocol::COM_END){
        return;
    }
    auto ng_index = get_line_newsgroup(id);
    bool exists = ng_index != -1;
    conn->write((unsigned char)Protocol::ANS_LIST_ART);

    if (exists){
        conn->write((unsigned char)Protocol::ANS_ACK);
        conn->write((unsigned char)Protocol::PAR_NUM);
        unsigned int size = database_size();
        send_N(conn,size);
        for (Article& a : articles_related_to_newsgroup(ng_index)){
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
void DiskServer::create_article(std::shared_ptr<Connection>& conn){
    if ((Protocol)conn->read()!=Protocol::PAR_NUM){
        return;
    }
    long unsigned int news_group_id = read_N(conn);
    if ((Protocol)conn->read()!=Protocol::PAR_STRING){
        return;
    }
    auto title_N = read_N(conn);
    std::string title = "";
    for (unsigned int i =0; i<title_N;++i){
        title+=conn->read();
    }

    if ((Protocol)conn->read()!=Protocol::PAR_STRING){
        return;
    }
    auto author_N = read_N(conn);
    std::string author = "";
    for (unsigned int i =0; i<author_N;++i){
        author+=conn->read();
    }

    if ((Protocol)conn->read()!=Protocol::PAR_STRING){
        return;
    }
    auto text_N = read_N(conn);
    std::string text = "";
        for (unsigned int i =0; i<text_N;++i){
        text+=conn->read();
    }
    if((Protocol)conn->read()!=Protocol::COM_END){
        return;
    }

    auto ng_index = get_line_newsgroup(news_group_id);
    bool exists = ng_index != -1;
    conn->write((unsigned char) Protocol::ANS_CREATE_ART);
    if (exists){
        conn->write((unsigned char) Protocol::ANS_ACK);
        Article art{title, author, text, (uint32_t)std::hash<std::string>{}(text),time(NULL)}; //Forcing 32bits conversion to accomadate for 32bit architecture.
        add_article(art);
    } else {
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
    }
    conn->write((unsigned char) Protocol::ANS_END);
}
void DiskServer::delete_article(std::shared_ptr<Connection>& conn){
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        return;
    }
    auto news_group_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        return;
    }
    auto article_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::COM_END){
        return;
    }
    auto ng_index = get_line_newsgroup(news_group_id);
    bool exists = ng_index != -1;
    conn->write((unsigned char) Protocol::ANS_DELETE_ART);
    if (exists){
        auto art_index = get_line_article(article_id);
        if(art_index!=-1){
            delete_article(art_index);
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
void DiskServer::get_article(std::shared_ptr<Connection>& conn){
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        return;
    }
    auto news_group_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::PAR_NUM){
        return;
    }
    auto article_id = read_N(conn);
    if((Protocol)conn->read()!=Protocol::COM_END){
        return;
    }
    auto ng_index = get_line_newsgroup(news_group_id);
    bool exists = ng_index != -1;
    conn->write((unsigned char) Protocol::ANS_GET_ART);
    if (exists){
        auto art_index = get_line_article(article_id);
        if(art_index!=-1){
            conn->write((unsigned char) Protocol::ANS_ACK);
            //titel
            std::string title = get_article_title(art_index);
            send_string_p(conn, title);
            //author
            std::string author = get_article_author(art_index);
            send_string_p(conn, author);
            //text
            std::string text = get_article_text(art_index);
            send_string_p(conn, text);
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
bool DiskServer::isReady(){
    return server.isReady();
}
void DiskServer::serve_one(){
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
DiskServer init(int argc, char* argv[]){
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

        DiskServer server(port);
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