#include <map>
#include <utility>
#include <algorithm>
#include "nameserverinterface.h"
#include "mns.h"
MNS::MNS(){
    
}
void MNS::insert(const HostName& name, const IPAddress& ip){
    data.insert({name, ip});
}
bool MNS::remove(const HostName& name){
    auto pos = data.erase(name);
    return pos;
}
IPAddress MNS::lookup(const HostName& name) const{
    auto it = data.find(name);
    if (it == data.end()){
        return NAME_SERVER_INTERFACE_H::NON_EXISTING_ADDRESS;
    }
    return std::get<1>(*it);
}