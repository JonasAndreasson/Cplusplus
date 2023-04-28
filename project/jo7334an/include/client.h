#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include "connection.h"
#include "protocol.h"
void send_N(const Connection&, unsigned int);
unsigned int read_N(const Connection&);
void send_string_p(const Connection&, std::string&);
void print_protocol_response(const PROTOCOL_H::Protocol);
PROTOCOL_H::Protocol create_newsgroup(const Connection&);
PROTOCOL_H::Protocol create_article(const Connection&);
PROTOCOL_H::Protocol delete_newsgroup(const Connection&);
void list_newsgroup(const Connection&);
void list_article(const Connection&);
void get_article(const Connection&);
int app(const Connection& conn);
Connection init(int, char*[]);
#endif