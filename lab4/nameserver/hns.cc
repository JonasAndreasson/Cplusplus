#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include "nameserverinterface.h"
#include "hns.h"


int hash_func(const HostName& key, long unsigned int n){
    std::hash<HostName> hashCode;
    return hashCode(key)%n;
}


HNS::HNS(const unsigned long int& size){
    data.resize(size);
}
void HNS::insert(const HostName& name, const IPAddress& ip){
    auto index = hash_func(name, data.size());
    std::pair<HostName, IPAddress> p(name, ip);
    data[index].push_back(p);
}
bool HNS::remove(const HostName& name){
    auto index = hash_func(name, data.size());
    std::vector<std::pair<HostName, IPAddress>>::iterator it = std::find_if(data[index].begin(), data[index].end(), [&name](const std::pair<HostName, IPAddress>& x){
        return x.first==name;
    });
    if (it == data[index].end()){
        return false;
    }
    data[index].erase(it);
    return true;
}


IPAddress HNS::lookup(const HostName& name) const{
    auto index = hash_func(name, data.size());
    auto it = std::find_if(data[index].begin(), data[index].end(), [&name](const std::pair<HostName, IPAddress>& x){
        return x.first==name;
    });
    if (it == data[index].end()){
        return NAME_SERVER_INTERFACE_H::NON_EXISTING_ADDRESS;
    }
    return (*it).second;
}