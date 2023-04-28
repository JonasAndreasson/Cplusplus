#include <iostream>
#include <fstream>
#include <sstream>
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
        if (entry->d_type == DT_DIR) {
            std::string subpath = entry->d_name; //converting for easier comparisons.
            if (subpath == "." || subpath==".."){

            } else {
            cout << entry->d_name << endl;
            file_count++; 
            }
        }
    }
    closedir(dirp);
    return file_count;
}
size_t DiskServer::newsgroup_size(std::string path){
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(path.c_str()); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_DIR) {
            std::string subpath = entry->d_name; //converting for easier comparisons.
            if (subpath == "." || subpath==".."){

            } else {
            cout << entry->d_name << endl;
            file_count++; 
            }
        }
    }
    closedir(dirp);
    return file_count;
}


void DiskServer::add_article(Article art, std::string path){
    path += std::to_string(art.id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    std::ofstream file_stream;
    file_stream.open(path+"/title");
    file_stream << art.title;
    file_stream.close();

    file_stream.open(path+"/author");
    file_stream << art.author;
    file_stream.close();

    file_stream.open(path+"/text");
    file_stream << art.text;
    file_stream.close();
    file_stream.open(path+"/created");
    file_stream << art.created;
    file_stream.close();
    file_stream.open(path+"/id");
    file_stream << art.id;
    file_stream.close();
}
std::vector<ServerInterface::Article> DiskServer::articles_related_to_newsgroup(std::string path){
    std::vector<ServerInterface::Article> list;
    std::string line;
    std::ifstream file;
    DIR * dirp, *sbdir;
    struct dirent * entry;
    dirp = opendir(path.c_str());
    while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_DIR) {
        std::string subpath = entry->d_name;
        if (subpath == "." || subpath == ".."){

        } else {
        subpath = path +"/"+ subpath;
        std::string title;
        std::string buffer;
        file.open(subpath+"/title");
        while (file >> buffer){
            title += buffer + " ";
        }
        title = title.substr(0,title.size()-1);
        file.close();
        std::string author;
        file.open(subpath+"/author");
        while (file >> buffer){
            author += buffer + " ";
        }
        author = author.substr(0,author.size()-1);
        file.close();
        std::string text;
        file.open(subpath+"/text");
        while (file >> buffer){
            text += buffer + " ";
        }
        text = text.substr(0,text.size()-1);
        file.close();
        uint32_t id;
        file.open(subpath+"/id");
        file >> id;
        file.close();
        time_t created;
        file.open(subpath+"/created");
        file >> created;
        file.close();
        Article art = {title,author,text,id,created};
        list.push_back(art);
        }
    }
    }
    closedir(dirp);
    return list;
}
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
std::string DiskServer::get_article_title(std::string path){
    std::ifstream f(path+"/title");
    std::stringstream s;
    if(f){
        s << f.rdbuf();
        f.close();
    }
    return s.str();
}
std::string DiskServer::get_article_author(std::string path){
    std::ifstream f(path+"/author");
    std::stringstream s;
    if(f){
        s << f.rdbuf();
        f.close();
    }
    return s.str();
}
std::string DiskServer::get_article_text(std::string path){
    std::ifstream f(path+"/text");
    std::stringstream s;
    if(f){
        s << f.rdbuf();
        f.close();
    }
    return s.str();
}


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
void DiskServer::remove_newsgroup(std::shared_ptr<Connection>& conn){//スーパー　ありがとうございます！今は3時三十四分です。
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
    std::string path = "database/"+std::to_string(id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    conn->write((unsigned char)Protocol::ANS_LIST_ART);

    if (exists){
        conn->write((unsigned char)Protocol::ANS_ACK);
        conn->write((unsigned char)Protocol::PAR_NUM);
        unsigned int size = newsgroup_size(path);
        send_N(conn,size);
        for (Article& a : articles_related_to_newsgroup(path)){
            conn->write((unsigned char)Protocol::PAR_NUM);
            send_N(conn,a.id);
            send_string_p(conn, a.title);
        }
    } else{
        rmdir(path.c_str());
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

    std::string path = "database/"+std::to_string(news_group_id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    conn->write((unsigned char) Protocol::ANS_CREATE_ART);
    if (exists){
        conn->write((unsigned char) Protocol::ANS_ACK);
        Article art{title, author, text, (uint32_t)std::hash<std::string>{}(text),time(NULL)}; //Forcing 32bits conversion to accomadate for 32bit architecture.
        add_article(art, path+"/");
    } else {
        rmdir(path.c_str());
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
    std::string path = "database/"+std::to_string(news_group_id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    conn->write((unsigned char) Protocol::ANS_DELETE_ART);
    if (exists){
        path+="/"+std::to_string(article_id);
        auto art_index = mkdir(path.c_str(),S_IRWXU);
        if(art_index==-1){
            rm_subdir(path);
            conn->write((unsigned char) Protocol::ANS_ACK);    
        } else {
        rmdir(path.c_str());
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    } else {
        rmdir(path.c_str());
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
    std::string path = "database/"+std::to_string(news_group_id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    conn->write((unsigned char) Protocol::ANS_GET_ART);
    if (exists){
        path += "/"+std::to_string(article_id);
        auto art_index = mkdir(path.c_str(),S_IRWXU);
        if(art_index==-1){
            conn->write((unsigned char) Protocol::ANS_ACK);
            //titel
            std::string title = get_article_title(path);
            send_string_p(conn, title);
            //author
            std::string author = get_article_author(path);
            send_string_p(conn, author);
            //text
            std::string text = get_article_text(path);
            send_string_p(conn, text);
        } else {
        rmdir(path.c_str());
        conn->write((unsigned char) Protocol::ANS_NAK);
        conn->write((unsigned char) Protocol::ERR_ART_DOES_NOT_EXIST);
        }
    } else {
        rmdir(path.c_str());
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