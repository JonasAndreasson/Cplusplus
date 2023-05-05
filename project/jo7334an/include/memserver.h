#ifndef MEMSERVER_H
#define MEMSERVER_H
#include "server.h"
#include "serverinterface.h"
#include "protocol.h"
class MemoryServer : public ServerInterface{
    public:
    MemoryServer(int port);
    virtual size_t database_size();
    virtual std::vector<Newsgroup> newsgroup_list();
    virtual bool try_create_newsgroup(std::string& sb);
    virtual bool try_remove_newsgroup(unsigned int id);
    virtual bool try_create_article(long unsigned int ng_id, std::string& title,std::string& author,std::string& text);
    virtual Protocol try_remove_article(unsigned int newsgroup_id, unsigned int article_id);
    virtual std::pair<bool,std::vector<Article>> try_list_article(unsigned int newsgroup_id);
    virtual std::pair<Protocol, Article> try_get_article(unsigned int newsgroup_id, unsigned int article_id);
    virtual std::shared_ptr<Connection> waitForActivity();
    virtual void registerConnection(const std::shared_ptr<Connection>& conn);
    virtual void deregisterConnection(const std::shared_ptr<Connection>& conn);
    virtual Server& get_server(){return server;}
    virtual bool isReady(){return server.isReady();}
    private:
    std::vector<Article> article_list;
    std::vector<Newsgroup> newsgroup_vector;
    Server server;
};
#endif