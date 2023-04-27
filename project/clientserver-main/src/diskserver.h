#ifndef DISKSERVER_H
#define DISKSERVER_H
#include "server.h"
#include "serverinterface.h"
#include <fstream>
#include <vector>
class DiskServer : public ServerInterface{
    public:
    DiskServer(int port);
    size_t database_size();
    int get_line_newsgroup(uint32_t id);
    int get_line_article(uint32_t id);
    void delete_article(int);
    void add_article(Article);
    void add_newsgroup(Newsgroup);
    void delete_newsgroup(int);
    Newsgroup parse_newsgroup(std::string);
    std::vector<Article> articles_related_to_newsgroup(int);
    std::vector<Newsgroup> newsgroup_list();
    std::string get_article_title(int);
    std::string get_article_author(int);
    std::string get_article_text(int);
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
};
#endif