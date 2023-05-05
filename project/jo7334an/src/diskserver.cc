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

    dirp = opendir("./database"); 
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_DIR) {
            std::string subpath = entry->d_name; 
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

    dirp = opendir(path.c_str()); 
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_DIR) {
            std::string subpath = entry->d_name; 
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
bool sort_article(const ServerInterface::Article& a1,const ServerInterface::Article& a2){
    double d = std::difftime(a2.created, a1.created);
    if (d<0){
        return false;
    } else 
    return true;
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
        int64_t created;
        file.open(subpath+"/created");
        file >> created;
        file.close();
        Article art = {title,author,text,id,created};
        list.push_back(art);
        }
    }
    }
    closedir(dirp);
    std::sort(list.begin(), list.end(), sort_article);
    for (auto& a : list){
        cout << a.created << endl;
    }
    return list;
}
bool sort_newsgroup(const ServerInterface::Newsgroup& ng1,const ServerInterface::Newsgroup& ng2){
    double d = std::difftime(ng2.created, ng1.created);
    if (d<0){
        return false;
    } else 
    return true;
}
std::vector<ServerInterface::Newsgroup> DiskServer::newsgroup_list(){
    std::vector<ServerInterface::Newsgroup> list;
    std::string line;
    std::ifstream file;
    DIR * dirp, *sbdir;
    struct dirent * entry;
    dirp = opendir("./database");
    while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_DIR) { 
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
        int64_t created;
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
    sort(list.begin(), list.end(),sort_newsgroup);
    for (auto& a : list){
        cout << a.created << endl;
    }
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
bool DiskServer::try_create_newsgroup(std::string& sb){
    std::string path = "database/"+std::to_string((uint32_t)std::hash<std::string>{}(sb));
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    if(!exists){
        ofstream file(path+"/name");
        file << sb<< endl;
        file.close();
        file.open(path+"/created");
        file <<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << endl;
        file.close();
    }
    return exists;
}
void rm_subdir(std::string path){
    DIR * dirp, *sbdir;
    struct dirent * entry;
    dirp = opendir(path.c_str());
    while ((entry = readdir(dirp)) != NULL) {
        std::string subpath = entry->d_name;
        if (entry->d_type == DT_DIR) {
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
bool DiskServer::try_remove_newsgroup(unsigned int id){
    std::string path = "database/"+std::to_string(id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    if (exists){
        rm_subdir(path);
    } else {
        rmdir(path.c_str());
    }
    return exists;
}
std::pair<bool,std::vector<ServerInterface::Article>> DiskServer::try_list_article(unsigned int id){
    std::string path = "database/"+std::to_string(id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    if (exists){
        std::pair<bool,std::vector<Article>> pair(true,articles_related_to_newsgroup(path));
        return pair;
    } else{
        rmdir(path.c_str());
        std::pair<bool,std::vector<Article>> pair(false,NULL);
        return pair;
    }
}
bool DiskServer::try_create_article(long unsigned int news_group_id, std::string& title,std::string& author,std::string& text){
    std::string path = "database/"+std::to_string(news_group_id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    if (exists){
        Article art{title, author, text, (uint32_t)std::hash<std::string>{}(text),std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()};
        add_article(art, path+"/");
    } else {
        rmdir(path.c_str());
    }
    return exists;
}
Protocol DiskServer::try_remove_article(unsigned int newsgroup_id, unsigned int article_id){
    std::string path = "database/"+std::to_string(newsgroup_id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    if (exists){
        path+="/"+std::to_string(article_id);
        auto art_index = mkdir(path.c_str(),S_IRWXU);
        if(art_index==-1){
            rm_subdir(path);
            return Protocol::ANS_ACK;
        } else {
        rmdir(path.c_str());
        return Protocol::ERR_ART_DOES_NOT_EXIST;
        }
    } else {
        rmdir(path.c_str());
        return Protocol::ERR_NG_DOES_NOT_EXIST;
    }
}
std::pair<Protocol, ServerInterface::Article> DiskServer::try_get_article(unsigned int newsgroup_id, unsigned int article_id){
    std::string path = "database/"+std::to_string(newsgroup_id);
    auto ng_index = mkdir(path.c_str(),S_IRWXU);
    bool exists = ng_index == -1;
    if (exists){
        path += "/"+std::to_string(article_id);
        auto art_index = mkdir(path.c_str(),S_IRWXU);
        if(art_index==-1){
            std::string title = get_article_title(path);
            std::string author = get_article_author(path);
            std::string text = get_article_text(path);
            std::pair<Protocol, Article> pair(Protocol::ANS_ACK, {title, author, text, 0,0});
            return pair;
        } else {
        rmdir(path.c_str());
        std::pair<Protocol, Article> pair(Protocol::ERR_ART_DOES_NOT_EXIST, {"", "", "", 0,0});
        return pair;
        }
    } else {
        rmdir(path.c_str());
        std::pair<Protocol, Article> pair(Protocol::ERR_NG_DOES_NOT_EXIST, {"", "", "", 0,0});
        return pair;
    }
}
std::shared_ptr<Connection> DiskServer::waitForActivity(){
    return server.waitForActivity();
}
void DiskServer::registerConnection(const std::shared_ptr<Connection>& conn){
    server.registerConnection(conn);
}
void DiskServer::deregisterConnection(const std::shared_ptr<Connection>& conn){
    server.deregisterConnection(conn);
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
            server.serve_one();
        }
        return 0;
}