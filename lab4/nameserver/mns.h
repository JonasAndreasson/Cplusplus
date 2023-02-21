#ifndef MNS_H
#define MNS_H
#include <map>
#include <string>
#include <utility>
#include "nameserverinterface.h"
class MNS: public NameServerInterface{
    public:
    MNS();
	void insert(const HostName&, const IPAddress&);
	bool remove(const HostName&);
	IPAddress lookup(const HostName&) const;
    private:
    std::map<HostName, IPAddress> data;
};
#endif