#ifndef MEMSERVER_H
#define MEMSERVER_H
#include "server.h"
#include "serverinterface.h"
class MemoryServer : public ServerInterface{
    public:
    MemoryServer(int port);
    virtual bool isReady();
    virtual void serve_one();
    virtual void list_newsgroup(std::shared_ptr<Connection>& conn);
    virtual void send_newsgroup(std::shared_ptr<Connection>& conn);
    virtual void create_newsgroup(std::shared_ptr<Connection>& conn);
    virtual void remove_newsgroup(std::shared_ptr<Connection>& conn);
    virtual void list_article(std::shared_ptr<Connection>& conn);
    virtual void create_article(std::shared_ptr<Connection>& conn);
    virtual void delete_article(std::shared_ptr<Connection>& conn);
    virtual void get_article(std::shared_ptr<Connection>& conn);
    private:
    Server server;
    std::vector<Article> article_list;
    std::vector<Newsgroup> newsgroup_list;
};
#endif