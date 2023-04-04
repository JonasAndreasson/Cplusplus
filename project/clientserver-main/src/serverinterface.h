#include <vector>
#include "connection.h"
enum MessageHandler: unsigned char {
/* Command Codes, client -> server */
COM_LIST_NG = 0x01,
COM_CREATE_NG = 0x02,
COM_DELETE_NG = 0x03,
COM_LIST_ART = 0x04,
COM_CREATE_ART = 0x05,
COM_DELETE_ART = 0x06,
COM_GET_ART = 0x07,
COM_END = 0x10,
/* Answer Codes, server -> client */
ANS_LIST_NG = 0x11,
ANS_CREATE_NG = 0x12,
ANS_DELETE_NG = 0x13,
ANS_LIST_ART = 0x14,
ANS_CREATE_ART = 0x15,
ANS_DELETE_ART = 0x16,
ANS_GET_ART = 0x17,
ANS_END = 0x20,
ANS_ACK = 0x21,
ANS_NAK = 0x22,
/* Error Codes */
ERR_NG_DOES_NOT_EXIST = 0x30,
ERR_NG_ALREADY_EXIST = 0x31,
ERR_ART_DOES_NOT_EXIST = 0x40,
/* Parameters */
PAR_STRING = 0xf0,
PAR_NUM = 0xff,
};
class ServerInterface{
    public:
    virtual bool isReady() = 0;
    virtual void serve_one() = 0;
    virtual void send_newsgroup(std::shared_ptr<Connection>&) = 0;
    virtual void list_newsgroup(std::shared_ptr<Connection>&) = 0;
    virtual void create_newsgroup(std::shared_ptr<Connection>&) = 0;
    void process_request(std::shared_ptr<Connection>& conn){
        unsigned char byte1 = conn->read();
        switch(byte1){
            case MessageHandler::COM_LIST_NG:
                list_newsgroup(conn);
                break;
            case MessageHandler::COM_CREATE_NG:
                break;
            case MessageHandler::COM_DELETE_NG:
                break;
            case MessageHandler::COM_LIST_ART:
                break;
            case MessageHandler::COM_CREATE_ART:
                break;
            case MessageHandler::COM_DELETE_ART:
                break;
            case MessageHandler::COM_GET_ART:
                break;
            default:
        }
    }
    struct Article{
        std::string title; //there is a limitation, although 2047 chars i likely enough
        std::string author;
        int id; //unique and non-reusable
        time_t created; // make this so it's invoked when the struct is created;
    };
    struct Newsgroup{
        std::string name; //there is a limitation, although 2047 chars i likely enough
        int id; //unique and non-reusable
        time_t created; // make this so it's invoked when the struct is created
        std::vector<Article> articles;
    };

    private:

};