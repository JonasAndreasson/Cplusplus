#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <utility>
#include "client.h"
#include "protocol.h"
#include "connection.h"
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
void send_N(const Connection& conn, unsigned int N){
    unsigned char byte1 = N % 256;
    N/=256;
    unsigned char byte2 = N % 256;
    N/=256;
    unsigned char byte3 = N % 256;
    N/=256;
    unsigned char byte4 = N % 256;
    conn.write(byte4); // <- 0x00 0x00 0x00 0xN
    conn.write(byte3); // <- 0x00 0x00 0x00 0xN
    conn.write(byte2); // <- 0x00 0x00 0x00 0xN
    conn.write(byte1); // <- 0x00 0x00 0x00 0xN
}
unsigned int read_N(const Connection& conn){
    unsigned char byte1 = conn.read();
    unsigned char byte2 = conn.read();
    unsigned char byte3 = conn.read();
    unsigned char byte4 = conn.read();
    unsigned int N = 0x00;
    N = (N << 8) | byte1;
    N = (N << 8) | byte2;
    N = (N << 8) | byte3;
    N = (N << 8) | byte4;
    return N;
}
void send_string_p(const Connection& conn, std::string& s){
    conn.write((unsigned char)Protocol::PAR_STRING);
    send_N(conn,s.size());
    for (char &c : s){
        conn.write(c);
    }
}
std::string read_string_p(const Connection& conn){
    if ((Protocol) conn.read() != Protocol::PAR_STRING){
        return "";
    }
    unsigned int N = read_N(conn);
    std::string sb="";
    for (unsigned int i = 0; i < N; ++i){
        sb+=conn.read();
    }
    return sb;
}
void print_protocol_response(const Protocol p){
    switch (p)
    {
    case Protocol::ANS_ACK:
        cout << "Command executed succesfully!\n";
        break;
    case Protocol::ERR_NG_ALREADY_EXISTS:
        cout << "A newsgroup with that name already exists\n";
        break;
    case Protocol::ERR_NG_DOES_NOT_EXIST:
        cout << "A newsgroup with that name doesn't exists" << endl;
        break;
    case Protocol::ERR_ART_DOES_NOT_EXIST:
        cout << "That article does not exist!" << endl;
        break;
    case Protocol::UNDEFINED:
        cout << "Something went wrong, terminating!";
        exit(1);
    default:
        cout << (unsigned char)p ;
        break;
    }
}
Protocol create_newsgroup(const Connection& conn){
    cout << "Enter title of the group:";
    std::string title;
    std::string buffer;
    cin >> buffer;
    std::getline(cin,title);
    if (title != ""){
        title = buffer + title;
    } else {
        title = buffer;
    }
    conn.write((unsigned char) Protocol::COM_CREATE_NG);
    send_string_p(conn, title);
    conn.write((unsigned char) Protocol::COM_END);
    if ((Protocol)conn.read() != Protocol::ANS_CREATE_NG)
    {
        return Protocol::UNDEFINED;
    }
    Protocol response = (Protocol)conn.read();
    if (response == Protocol::ANS_ACK){
        if ((Protocol)conn.read() != Protocol::ANS_END){
            return Protocol::UNDEFINED;
        }
        return Protocol::ANS_ACK;
    }
    if (response == Protocol::ANS_NAK){
        if ((Protocol)conn.read() != Protocol::ERR_NG_ALREADY_EXISTS){
            return Protocol::UNDEFINED;
        }
        if ((Protocol)conn.read() != Protocol::ANS_END){
            return Protocol::UNDEFINED;
        }
        return Protocol::ERR_NG_ALREADY_EXISTS;
    }
}
Protocol create_article(const Connection& conn){
    //GETTTING ALL INFO
    std::string newsgroup_name;
    std::string buffer;
    cout << "Please enter newsgroup name:" << endl;
    cin >> buffer;
    std::getline(cin,newsgroup_name);
    if (newsgroup_name != ""){
        newsgroup_name = buffer + newsgroup_name;
    } else {
        newsgroup_name = buffer;
    }
    std::string title;
    cout << "Please enter the title:" << endl;
    cin >> buffer;
    std::getline(cin,title);
    if (title != ""){
        title = buffer + title;
    } else {
        title = buffer;
    }
    std::string author;
    cout << "Please enter the author:" << endl;
    cin >> buffer;
    std::getline(cin,author);
    if (author != ""){
        author = buffer + author;
    } else {
        author = buffer;
    }
    std::string text;
    cout << "Please enter the text:" << endl;
    cin >> buffer;
    std::getline(cin,text);
    if (text != ""){
        text = buffer + text;
    } else {
        text = buffer;
    }
    //SENDING
    conn.write((unsigned char) Protocol::COM_CREATE_ART);
    conn.write((unsigned char) Protocol::PAR_NUM);
    send_N(conn, (uint32_t)std::hash<std::string>{}(newsgroup_name));
    send_string_p(conn, title);
    send_string_p(conn, author);
    send_string_p(conn, text);
    conn.write((unsigned char) Protocol::COM_END);
    if ((Protocol)conn.read() != Protocol::ANS_CREATE_ART){
        return Protocol::UNDEFINED;
    }
    Protocol response = (Protocol) conn.read();
    if (response == Protocol::ANS_ACK) {
       if ((Protocol) conn.read() != Protocol::ANS_END){
        return Protocol::UNDEFINED;
       }
       return Protocol::ANS_ACK;
    }   else if (response == Protocol::ANS_NAK )
    {
        if ((Protocol) conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST ) return Protocol::UNDEFINED;
        if ((Protocol) conn.read() != Protocol::ANS_END) return Protocol::UNDEFINED;
        return Protocol::ERR_NG_DOES_NOT_EXIST;
    } else {
        return Protocol::UNDEFINED;
    }
    
    
}
void list_newsgroup(const Connection& conn){
    std::vector<std::pair<std::string, uint32_t>> pairs;
    conn.write((unsigned char) Protocol::COM_LIST_NG);
    conn.write((unsigned char) Protocol::COM_END);
    if ((Protocol)conn.read() != Protocol::ANS_LIST_NG) {
        cout << "Reponse wasn't ANS_LIST_NG";
        return;
    }
    if ((Protocol)conn.read() != Protocol::PAR_NUM){
        cout << "Reponse wasn't PAR_NUM";
        return;
    }
    unsigned int N = read_N(conn); //N
    cout << N;
    for (unsigned int i = 0; i < N; ++i){
        if ((Protocol)conn.read() != Protocol::PAR_NUM){
            return;
        }
        uint32_t id = read_N(conn); // ID for group
        std::string title = read_string_p(conn);
        pairs.push_back(std::make_pair(title, id));
    }
    if ((Protocol)conn.read() != Protocol::ANS_END){
        return;
    }
    cout << "Number of newsgroup: " << N << endl;
    for (auto& p : pairs){
        cout << p.first <<":" << p.second <<endl;
    }
}
void list_article(const Connection& conn){
    std::vector<std::pair<std::string, uint32_t>> pairs;
    std::string newsgroup_name;
    std::string buffer;
    cout << "Please enter newsgroup name:" << endl;
    cin >> buffer;
    std::getline(cin,newsgroup_name);
    if (newsgroup_name != ""){
        newsgroup_name = buffer + newsgroup_name;
    } else {
        newsgroup_name = buffer;
    }
    conn.write((unsigned char) Protocol::COM_LIST_ART);
    conn.write((unsigned char) Protocol::PAR_NUM);
    send_N(conn, (uint32_t)std::hash<std::string>{}(newsgroup_name));
    conn.write((unsigned char) Protocol::COM_END);
    if ((Protocol)conn.read() != Protocol::ANS_LIST_ART) {
        cout << "Reponse wasn't ANS_LIST_ART";
        return;
    }
    Protocol response = (Protocol)conn.read();
    if (response == Protocol::ANS_ACK){
        if ((Protocol)conn.read() != Protocol::PAR_NUM){
        cout << "Reponse wasn't PAR_NUM";
        return;
    }
    unsigned int N = read_N(conn); //N
    for (unsigned int i = 0; i < N; ++i){
        if ((Protocol)conn.read() != Protocol::PAR_NUM){
            return;
        }
        uint32_t id = read_N(conn); // ID for article
        std::string title = read_string_p(conn);
        pairs.push_back(std::make_pair(title, id));
    }
    if ((Protocol)conn.read() != Protocol::ANS_END){
        return;
    }
    cout << "Number of articles in newsgroup: " << N << endl;
    if (N=!0){
        cout << "Title" <<":" << "id" <<endl;
    }
    for (auto& p : pairs){
        cout << p.first <<":" << p.second <<endl;
    }
    } else if (response == Protocol::ANS_NAK){
        if ((Protocol)conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
            cout << "Something went wrong! Terminating!";
            exit(1);
        }
        if ((Protocol)conn.read() != Protocol::ANS_END){
            cout << "Something went wrong! Terminating!";
            exit(1);
        }
        print_protocol_response(Protocol::ERR_NG_DOES_NOT_EXIST);
    } else {
        cout << "Something went wrong! Terminating!";
        exit(1);
    }
}
Protocol delete_newsgroup(const Connection& conn){
    cout << "Enter newsgroup id:"<<endl;
    uint32_t id;
    cin >> id;
    conn.write((unsigned char) Protocol::COM_DELETE_NG);
    conn.write((unsigned char) Protocol::PAR_NUM);
    send_N(conn, id);
    conn.write((unsigned char) Protocol::COM_END);

    // REPONSE

    if ((Protocol)conn.read() != Protocol::ANS_DELETE_NG){
        return Protocol::UNDEFINED;
    }
    Protocol response = (Protocol)conn.read();
    if (response == Protocol::ANS_ACK) {
        if ((Protocol)conn.read() != Protocol::ANS_END){
        return Protocol::UNDEFINED;
        }
        return Protocol::ANS_ACK;
    } else if (response == Protocol::ANS_NAK) {
        if ((Protocol)conn.read() != Protocol::ERR_NG_DOES_NOT_EXIST){
        return Protocol::UNDEFINED;
        }
        if ((Protocol)conn.read() != Protocol::ANS_END){
        return Protocol::UNDEFINED;
        }
        return Protocol::ERR_NG_DOES_NOT_EXIST;
    }
    else {
        return Protocol::UNDEFINED;
    }
}
void get_article(const Connection& conn){
    std::string newsgroup_name;
    std::string buffer;
    cout << "Please enter newsgroup name:" << endl;
    cin >> buffer;
    std::getline(cin,newsgroup_name);
    if (newsgroup_name != ""){
        newsgroup_name = buffer + newsgroup_name;
    } else {
        newsgroup_name = buffer;
    }
    cout << "Please enter article id:" << endl;
    uint32_t article_id;
    cin >> article_id;
    uint32_t newsgroup_id = (uint32_t)std::hash<std::string>{}(newsgroup_name);
    //Sending data
    conn.write((unsigned char)Protocol::COM_GET_ART);
    conn.write((unsigned char)Protocol::PAR_NUM);
    send_N(conn, newsgroup_id);
    conn.write((unsigned char)Protocol::PAR_NUM);
    send_N(conn, article_id);
    conn.write((unsigned char)Protocol::COM_END);
    //Recieving data
    if ((Protocol) conn.read() != Protocol::ANS_GET_ART){
        //Something went wrong
        return;
    }
    Protocol response = (Protocol) conn.read();
    if (response == Protocol::ANS_ACK){
        std::string title = read_string_p(conn);
        std::string author = read_string_p(conn);
        std::string text = read_string_p(conn);
        if((Protocol) conn.read() != Protocol::ANS_END){
            //Something went wrong
            return;
        }
        // Print out the text
        cout << "Title: " << title <<endl;
        cout << "Author: " << author << endl;
        cout << text << endl;
    }else if (response == Protocol::ANS_NAK)
    {
        response = (Protocol) conn.read();
        if (response == Protocol::ERR_NG_DOES_NOT_EXIST || response == Protocol::ERR_ART_DOES_NOT_EXIST){
            print_protocol_response(response);
        } else {
            //something went wrong
            return;
        }
    } else {
        //SOMETHING WENT WRONG
        return;
    }
    
}
int app(const Connection& conn){
    std::string command;
    std::string subcommand;
    cout << "Type help for commands" <<'\n';
    while(cin >> command){
        if (command == "help"){
            cout << "list newsgroup -- lists all newsgroup\n";
            cout << "create newsgroup -- creates a newsgroup\n";
            cout << "delete newsgroup  -- deletes a newsgroup\n";
            cout << "list article  -- lists all articles in a newsgroup\n";
            cout << "create article  -- creates an article\n";
            cout << "delete article  -- deletes an article\n";
            cout << "get article  -- returns the title, author and text of an article\n";
            cout << "help -- shows this text \n";
        }
        else if (command == "list") {
            std::string subcommand;
            cin >> subcommand;
            if (subcommand == "newsgroup"){
                cout << "Listing newsgroup" << endl;
                list_newsgroup(conn);
                cout << "Done listing the newsgroup" << endl;
            } else 
            if (subcommand == "article"){
                list_article(conn);
            } else {
                cout << "list " <<subcommand << " doesn't exist!\n";
            }
        }
        else if (command == "create"){
            cin >> subcommand;
            if (subcommand == "newsgroup"){
                print_protocol_response(create_newsgroup(conn));
            } else if (subcommand == "article")
            {
                print_protocol_response(create_article(conn));
            } else {
                cout << "The command \"create "<< subcommand << "\" doesn't exist" << endl;
            }
        }
        else if (command == "delete"){
            cin >> subcommand;
            if (subcommand == "newsgroup"){
                print_protocol_response(delete_newsgroup(conn));
            } else if (subcommand == "article")
            {
                /* code */
            } else {
                
            }
            
        }
        else if (command == "get"){
            cin >> subcommand;
            if (subcommand == "article"){
                get_article(conn);
            } else {
                cout << "The command \"get "<< subcommand << "\" doesn't exist" << endl;
            }
        } else {
        cout << command << " is not a valid command!\n";
        }
    }
    return 0;
}

Connection init(int argc, char* argv[])
{
        if (argc != 3) {
                cerr << "Usage: myclient host-name port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[2]);
        } catch (std::exception& e) {
                cerr << "Wrong port number. " << e.what() << endl;
                exit(2);
        }

        Connection conn(argv[1], port);
        if (!conn.isConnected()) {
                cerr << "Connection attempt failed" << endl;
                exit(3);
        }

        return conn;
}
int main(int argc, char* argv[]){
        Connection conn = init(argc, argv);
        return app(conn);
}