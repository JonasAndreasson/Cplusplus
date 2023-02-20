#ifndef DNS_H
#define DNS_H
#include <vector>
#include <string>
#include <utility>
#include "nameserverinterface.h"
class VNS: public NameServerInterface{
    public:
    VNS();
	void insert(const HostName&, const IPAddress&);
	bool remove(const HostName&) = 0;
	IPAddress lookup(const HostName&) const = 0;
    private:
    std::vector<std::pair<HostName, IPAddress>> data;
};
#endif