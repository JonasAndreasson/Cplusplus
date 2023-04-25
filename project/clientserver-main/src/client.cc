#include <iostream>
#include <string.h>
#include <cctype>
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


int app(const Connection& conn){
    std::string command;
    while (cin >> command){
        conn.write((unsigned char) Protocol::COM_LIST_NG);
        conn.write((unsigned char) Protocol::COM_END);
    }
}
std::string read_string_p(const Connection& conn){
    unsigned int N = read_N(conn);
    std::string sb="";
    for (unsigned int i = 0; i < N; ++i){
        sb+=conn.read();
    }
    return sb;
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