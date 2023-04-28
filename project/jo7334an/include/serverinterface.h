#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H
#include <vector>
#include <string.h>
#include <iostream>
#include "connection.h"
#include "protocol.h"
class ServerInterface{
    public:
    virtual bool isReady() = 0;
    virtual void serve_one() = 0;
    virtual void send_newsgroup(std::shared_ptr<Connection>&) = 0;
    virtual void list_newsgroup(std::shared_ptr<Connection>&) = 0;
    virtual void create_newsgroup(std::shared_ptr<Connection>&) = 0;
    virtual void remove_newsgroup(std::shared_ptr<Connection>&) = 0;
    virtual void list_article(std::shared_ptr<Connection>&) = 0;
    virtual void create_article(std::shared_ptr<Connection>&) = 0;
    virtual void delete_article(std::shared_ptr<Connection>&) = 0;
    virtual void get_article(std::shared_ptr<Connection>&) = 0;
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
    struct Article{
        std::string title; //there is a limitation, although 2047 chars i likely enough
        std::string author;
        std::string text;
        unsigned int id; //unique and non-reusable
        time_t created; // make this so it's invoked when the struct is created;
    };
    struct Newsgroup{
        std::string name; //there is a limitation, although 2047 chars i likely enough
        unsigned int id; //unique and non-reusable
        time_t created; // make this so it's invoked when the struct is created
        std::vector<Article> articles;
    };

    private:
};
#endif