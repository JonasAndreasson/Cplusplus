#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <ios>
#include <string>
#include <utility>
#include "serverinterface.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
class ServerInterface{
    public:
    struct Article{
        std::string title; //there is a limitation, although 2047 chars i likely enough
        std::string author;
        std::string text;
        unsigned int id; //unique and non-reusable
        int64_t created; // make this so it's invoked when the struct is created;
    };
    struct Newsgroup{
        std::string name; //there is a limitation, although 2047 chars i likely enough
        unsigned int id; //unique and non-reusable
        int64_t created; // make this so it's invoked when the struct is created
        std::vector<Article> articles;
    };
    virtual size_t database_size() = 0;
    virtual std::vector<Newsgroup> newsgroup_list() = 0;
    virtual bool try_create_newsgroup(std::string&) = 0;
    virtual bool try_remove_newsgroup(unsigned int) = 0;
    virtual bool try_create_article(long unsigned int ng_id, std::string& title,std::string& author,std::string& text) = 0;
    virtual Protocol try_remove_article(unsigned int newsgroup_id, unsigned int article_id) = 0;
    virtual std::pair<Protocol, Article> try_get_article(unsigned int newsgroup_id, unsigned int article_id) = 0;
    virtual std::pair<bool,std::vector<Article>> try_list_article(unsigned int newsgroup_id) = 0;
    virtual Server& get_server() = 0;
    virtual bool isReady() = 0;
    virtual void serve_one(){
        auto conn = waitForActivity();
        if (conn != nullptr) {
            try {
                process_request(conn);
            } catch (ConnectionClosedException&) {
                deregisterConnection(conn);
                std::cout << "Client closed connection" << std::endl;
            }
        } else {
            conn = std::make_shared<Connection>();
            registerConnection(conn);
            std::cout << "New client connects" << std::endl;
        }
    }
    virtual std::shared_ptr<Connection> waitForActivity() = 0;
    virtual void registerConnection(const std::shared_ptr<Connection>& conn) = 0;
    virtual void deregisterConnection(const std::shared_ptr<Connection>& conn) = 0;
    virtual void list_newsgroup(std::shared_ptr<Connection>& conn){
        unsigned char byte2 = conn->read();
        if ((Protocol)byte2 != Protocol::COM_END){
            conn->~Connection();
            return;
        }
        conn->write((unsigned char)Protocol::ANS_LIST_NG);
        conn->write((unsigned char)Protocol::PAR_NUM);
        auto size = database_size();
        send_N(conn, size);
        for (auto &newsgroup : newsgroup_list())
        {
            conn->write((unsigned char)Protocol::PAR_NUM);
            send_N(conn,newsgroup.id);
            send_string_p(conn, newsgroup.name);
        }
        conn->write((unsigned char)Protocol::ANS_END);
    }
    virtual void create_newsgroup(std::shared_ptr<Connection>& conn){
        unsigned char byte = conn->read(); 
        if ((Protocol)byte != Protocol::PAR_STRING){
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
        
        if(!try_create_newsgroup(sb)){
            conn->write((unsigned char)Protocol::ANS_CREATE_NG);
            conn->write((unsigned char)Protocol::ANS_ACK);
            conn->write((unsigned char)Protocol::ANS_END);
        } else {
            conn->write((unsigned char)Protocol::ANS_CREATE_NG);
            conn->write((unsigned char)Protocol::ANS_NAK);
            conn->write((unsigned char)Protocol::ERR_NG_ALREADY_EXISTS);
            conn->write((unsigned char)Protocol::ANS_END);
        }
    }
    virtual void remove_newsgroup(std::shared_ptr<Connection>& conn){
        if ((Protocol)conn->read() != Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        unsigned int id = read_N(conn);
        if ((Protocol)conn->read() != Protocol::COM_END){
            conn->~Connection();
            return;
        }
        conn->write((unsigned char)Protocol::ANS_DELETE_NG);
        if (try_remove_newsgroup(id)){
            conn->write((unsigned char)Protocol::ANS_ACK);
        } else {
            conn->write((unsigned char)Protocol::ANS_NAK);
            conn->write((unsigned char)Protocol::ERR_NG_DOES_NOT_EXIST);
        }
        conn->write((unsigned char)Protocol::ANS_END);
    }
    virtual void list_article(std::shared_ptr<Connection>& conn){
        if ((Protocol)conn->read() != Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        unsigned int id = read_N(conn);
        if ((Protocol)conn->read() != Protocol::COM_END){
            conn->~Connection();
            return;
        }
        conn->write((unsigned char)Protocol::ANS_LIST_ART);
        std::pair<bool,std::vector<Article>> response = try_list_article(id);
        if (response.first){
            conn->write((unsigned char)Protocol::ANS_ACK);
            conn->write((unsigned char)Protocol::PAR_NUM);
            unsigned int size = response.second.size();
            send_N(conn,size);
            for (Article& a : response.second){
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
    virtual void create_article(std::shared_ptr<Connection>& conn){
        if ((Protocol)conn->read()!=Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        long unsigned int news_group_id = read_N(conn);
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
        conn->write((unsigned char) Protocol::ANS_CREATE_ART);
        if (try_create_article(news_group_id, title, author, text)){
            conn->write((unsigned char) Protocol::ANS_ACK);
        } else {
            conn->write((unsigned char) Protocol::ANS_NAK);
            conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
        }
        conn->write((unsigned char) Protocol::ANS_END);
    }
    virtual void delete_article(std::shared_ptr<Connection>& conn){
        if((Protocol)conn->read()!=Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        auto newsgroup_id = read_N(conn);
        if((Protocol)conn->read()!=Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        auto article_id = read_N(conn);
        if((Protocol)conn->read()!=Protocol::COM_END){
            conn->~Connection();
            return;
        }
        conn->write((unsigned char) Protocol::ANS_DELETE_ART);
        Protocol response = try_remove_article(newsgroup_id,article_id);
        if (response == Protocol::ANS_ACK){
            conn->write((unsigned char) Protocol::ANS_ACK);    
        } else if (response == Protocol::ERR_ART_DOES_NOT_EXIST){
            conn->write((unsigned char) Protocol::ANS_NAK);
            conn->write((unsigned char) Protocol::ERR_ART_DOES_NOT_EXIST);
        } else {
            conn->write((unsigned char) Protocol::ANS_NAK);
            conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
        }
        conn->write((unsigned char) Protocol::ANS_END);
    }
    virtual void get_article(std::shared_ptr<Connection>& conn){
        if((Protocol)conn->read()!=Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        auto newsgroup_id = read_N(conn);
        if((Protocol)conn->read()!=Protocol::PAR_NUM){
            conn->~Connection();
            return;
        }
        auto article_id = read_N(conn);
        if((Protocol)conn->read()!=Protocol::COM_END){
            conn->~Connection();
            return;
        }
        conn->write((unsigned char) Protocol::ANS_GET_ART);
        std::pair<Protocol, Article> response = try_get_article(newsgroup_id, article_id);
        if(response.first == Protocol::ANS_ACK){
            conn->write((unsigned char) Protocol::ANS_ACK);
            send_string_p(conn, response.second.title);
            send_string_p(conn, response.second.author);
            send_string_p(conn, response.second.text);
        } else if (response.first == Protocol::ERR_ART_DOES_NOT_EXIST){
            conn->write((unsigned char) Protocol::ANS_NAK);
            conn->write((unsigned char) Protocol::ERR_ART_DOES_NOT_EXIST);
        } else {
            conn->write((unsigned char) Protocol::ANS_NAK);
            conn->write((unsigned char) Protocol::ERR_NG_DOES_NOT_EXIST);
        }
        conn->write((unsigned char) Protocol::ANS_END);
    }
    void send_N(std::shared_ptr<Connection>& conn, unsigned int N){
        unsigned char byte1 = N % 256;
        N/=256;
        unsigned char byte2 = N % 256;
        N/=256;
        unsigned char byte3 = N % 256;
        N/=256;
        unsigned char byte4 = N % 256;
        conn->write(byte4); // <- 0x00 0x00 0x00 0xN
        conn->write(byte3); // <- 0x00 0x00 0x00 0xN
        conn->write(byte2); // <- 0x00 0x00 0x00 0xN
        conn->write(byte1); // <- 0x00 0x00 0x00 0xN
    }
    unsigned int read_N(std::shared_ptr<Connection>& conn){
        unsigned char byte1 = conn->read();
        unsigned char byte2 = conn->read();
        unsigned char byte3 = conn->read();
        unsigned char byte4 = conn->read();
        unsigned int N = 0x00;
        N = (N << 8) | byte1;
        N = (N << 8) | byte2;
        N = (N << 8) | byte3;
        N = (N << 8) | byte4;
        return N;
    }
    void send_string_p(std::shared_ptr<Connection>& conn, std::string& s){
        conn->write((unsigned char)Protocol::PAR_STRING);
        send_N(conn,s.size());
        for (char &c : s){
            conn->write(c);
        }
    }
    void process_request(std::shared_ptr<Connection>& conn){
        unsigned char byte1 = conn->read();
        switch((Protocol)byte1){
            case Protocol::COM_LIST_NG:
                std::cout << "Listing news group\n";
                list_newsgroup(conn);
                break;
            case Protocol::COM_CREATE_NG:
                std::cout << "create news group\n";
                create_newsgroup(conn);
                break;
            case Protocol::COM_DELETE_NG:
                std::cout << "delete news group\n";
                remove_newsgroup(conn);
                break;
            case Protocol::COM_LIST_ART:
                list_article(conn);
                break;
            case Protocol::COM_CREATE_ART:
                create_article(conn);
                break;
            case Protocol::COM_DELETE_ART:
                delete_article(conn);
                break;
            case Protocol::COM_GET_ART:
                get_article(conn);
                break;
            default:
                break;
        }
    }
    private:
};
#endif