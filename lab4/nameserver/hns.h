#ifndef HNS_H
#define HNS_H
#include <vector>
#include <string>
#include <utility>
#include "nameserverinterface.h"
class HNS: public NameServerInterface{
    public:
    HNS(const unsigned long int&);
	void insert(const HostName&, const IPAddress&);
	bool remove(const HostName&);
	IPAddress lookup(const HostName&) const;
    private:
    std::vector<std::vector<std::pair<HostName, IPAddress>>> data;
};
#endif