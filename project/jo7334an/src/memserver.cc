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
MemoryServer::MemoryServer(int port):server(port),newsgroup_vector(){}
size_t MemoryServer::database_size(){
    return newsgroup_vector.size();
}
std::vector<ServerInterface::Newsgroup> MemoryServer::newsgroup_list(){
    return newsgroup_vector;
}
bool MemoryServer::try_create_newsgroup(std::string& sb){
    bool exists = std::find_if(newsgroup_vector.begin(),
             newsgroup_vector.end(), 
             [&sb] (Newsgroup ng) -> bool { return sb == ng.name; }) != newsgroup_vector.end();

    if(!exists){
    std::vector<Article> articles;
    Newsgroup ng = {sb,(uint32_t)std::hash<std::string>{}(sb),std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()};
    newsgroup_vector.push_back(ng);
    }
    return exists;
}
bool MemoryServer::try_remove_newsgroup(unsigned int id){
    auto it = std::find_if(newsgroup_vector.begin(),
             newsgroup_vector.end(), 
             [&id] (Newsgroup ng) -> bool { return id == ng.id; });
    bool exists = it != newsgroup_vector.end();
    if (exists){
        newsgroup_vector.erase(it);
    }
    return exists;
}
std::pair<bool,std::vector<ServerInterface::Article>> MemoryServer::try_list_article(unsigned int id){
    auto it = std::find_if(newsgroup_vector.begin(),
             newsgroup_vector.end(), 
             [&id] (ServerInterface::Newsgroup ng) -> bool { return id == ng.id; });
    bool exists = it != newsgroup_vector.end();
    if (exists){
        std::pair<bool,std::vector<Article>> pair(true,(*it).articles);
        return pair;
    } else{
        std::pair<bool,std::vector<ServerInterface::Article>> pair(false,NULL);
        return pair;
    }
}
bool MemoryServer::try_create_article(long unsigned int news_group_id, std::string& title, std::string& author, std::string& text){
    auto it = std::find_if(newsgroup_vector.begin(),
             newsgroup_vector.end(), 
             [&news_group_id] (Newsgroup ng) -> bool { return news_group_id == ng.id; });
    bool exists = it != newsgroup_vector.end();
    if (exists){
        Article art{title, author, text, (uint32_t)std::hash<std::string>{}(text),std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()};
        (*it).articles.push_back(art);
    }
    return exists;
}
Protocol MemoryServer::try_remove_article(unsigned int newsgroup_id, unsigned int article_id){
    auto ng_it = std::find_if(newsgroup_vector.begin(),
             newsgroup_vector.end(), 
             [&newsgroup_id] (Newsgroup ng) -> bool { return newsgroup_id == ng.id; });
    bool exists = ng_it != newsgroup_vector.end();
    if (exists){
        auto art_it = std::find_if((*ng_it).articles.begin(),(*ng_it).articles.end(),[&article_id] (Article art) -> bool { return article_id == art.id; });
        if(art_it!=(*ng_it).articles.end()){
            (*ng_it).articles.erase(art_it);
            return Protocol::ANS_ACK;    
        } else {
        return Protocol::ERR_ART_DOES_NOT_EXIST;
        }
    } else {
        return Protocol::ERR_NG_DOES_NOT_EXIST;
    }
}
std::pair<Protocol, ServerInterface::Article> MemoryServer::try_get_article(unsigned int newsgroup_id, unsigned int article_id){
    auto ng_it = std::find_if(newsgroup_vector.begin(),
             newsgroup_vector.end(), 
             [&newsgroup_id] (Newsgroup ng) -> bool { return newsgroup_id == ng.id; });
    bool exists = ng_it != newsgroup_vector.end();
    if (exists){
        auto art_it = std::find_if((*ng_it).articles.begin(),(*ng_it).articles.end(),[&article_id] (Article art) -> bool { return article_id == art.id; });
        if(art_it!=(*ng_it).articles.end()){
            std::pair<Protocol, Article> pair(Protocol::ANS_ACK,(*art_it));
            return pair;
        } else {
        std::pair<Protocol, Article> pair(Protocol::ERR_ART_DOES_NOT_EXIST, {"", "", "", 0,0});
        return pair;
        }
    } else {
        std::pair<Protocol, Article> pair(Protocol::ERR_NG_DOES_NOT_EXIST, {"", "", "", 0,0});
        return pair;
    }
    
}

std::shared_ptr<Connection> MemoryServer::waitForActivity(){
    return server.waitForActivity();
}
void MemoryServer::registerConnection(const std::shared_ptr<Connection>& conn){
    server.registerConnection(conn);
}
void MemoryServer::deregisterConnection(const std::shared_ptr<Connection>& conn){
    server.deregisterConnection(conn);
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

        MemoryServer memory_server(port);
        if (!memory_server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return memory_server;
}
int main(int argc, char* argv[])
{
        auto server = init(argc, argv);

        while (true) {
            server.serve_one();
        }
        return 0;
}