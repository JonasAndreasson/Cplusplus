#include <vector>
#include <utility>
#include <algorithm>
#include "nameserverinterface.h"
#include "vns.h"
VNS::VNS(){
    
}
void VNS::insert(const HostName& name, const IPAddress& ip){
    std::pair<HostName, IPAddress> p(name, ip);
    data.push_back(p);
}
bool VNS::remove(const HostName& name){
    std::vector<std::pair<HostName, IPAddress>>::iterator it = std::find_if(data.begin(), data.end(), [this,name](std::pair<HostName, IPAddress> x){
        if (std::get<0>(x)==name){
            return true;
        }
        return false;
    });
    if (it == data.end()){
        return false;
    }
    data.erase(it);
    return true;
}
IPAddress VNS::lookup(const HostName& name) const{
    auto it = std::find_if(data.begin(), data.end(), [this,name](const std::pair<HostName, IPAddress>& x){
        return x.first==name;
    });
    if (it == data.end()){
        return NAME_SERVER_INTERFACE_H::NON_EXISTING_ADDRESS;
    }
    return (*it).second;
}