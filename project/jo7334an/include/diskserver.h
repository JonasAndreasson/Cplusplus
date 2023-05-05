#ifndef DISKSERVER_H
#define DISKSERVER_H
#include "server.h"
#include "serverinterface.h"
#include <fstream>
#include <vector>
class DiskServer : public ServerInterface{
    public:
    DiskServer(int port);
    
    size_t newsgroup_size(std::string);
    void add_article(Article, std::string);
    Newsgroup parse_newsgroup(std::string);
    std::vector<Article> articles_related_to_newsgroup(std::string);
    std::string get_article_title(std::string);
    std::string get_article_author(std::string);
    std::string get_article_text(std::string);
    
    virtual std::shared_ptr<Connection> waitForActivity();
    virtual void registerConnection(const std::shared_ptr<Connection>& conn);
    virtual void deregisterConnection(const std::shared_ptr<Connection>& conn);
    virtual size_t database_size();
    virtual std::vector<Newsgroup> newsgroup_list();
    virtual bool try_create_newsgroup(std::string& sb);
    virtual bool try_remove_newsgroup(unsigned int id);
    virtual std::pair<bool,std::vector<Article>> try_list_article(unsigned int newsgroup_id);
    virtual bool try_create_article(long unsigned int ng_id, std::string& title,std::string& author,std::string& text);
    virtual Protocol try_remove_article(unsigned int ng_id, unsigned int article_id);
    virtual std::pair<Protocol, Article> try_get_article(unsigned int newsgroup_id, unsigned int article_id);
    virtual Server& get_server(){return server;}
    virtual bool isReady(){return server.isReady();}
    private:
    Server server;
};
#endif