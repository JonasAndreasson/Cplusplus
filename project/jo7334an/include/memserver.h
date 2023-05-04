#ifndef MEMSERVER_H
#define MEMSERVER_H
#include "server.h"
#include "serverinterface.h"
#include "protocol.h"
class MemoryServer : public ServerInterface{
    public:
    MemoryServer(int port);
    virtual bool isReady();
    virtual void serve_one();
    virtual size_t database_size();
    virtual std::vector<Newsgroup> newsgroup_list();
    virtual bool try_create_newsgroup(std::string& sb);
    virtual bool try_remove_newsgroup(unsigned int id);
    virtual bool try_create_article(long unsigned int ng_id, std::string& title,std::string& author,std::string& text);
    virtual Protocol try_remove_article(unsigned int newsgroup_id, unsigned int article_id);
    virtual std::pair<bool,std::vector<Article>> try_list_article(unsigned int newsgroup_id);
    virtual std::pair<Protocol, Article> try_get_article(unsigned int newsgroup_id, unsigned int article_id);
    private:
    Server server;
    std::vector<Article> article_list;
    std::vector<Newsgroup> newsgroup_vector;
};
#endif