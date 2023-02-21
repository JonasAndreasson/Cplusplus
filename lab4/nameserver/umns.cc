#include <unordered_map>
#include <utility>
#include <algorithm>
#include "nameserverinterface.h"
#include "umns.h"
UMNS::UMNS(){
    
}
void UMNS::insert(const HostName& name, const IPAddress& ip){
    data.insert({name, ip});
}
bool UMNS::remove(const HostName& name){
    auto pos = data.erase(name);
    return pos;
}
IPAddress UMNS::lookup(const HostName& name) const{
    auto it = data.find(name);
    if (it == data.end()){
        return NAME_SERVER_INTERFACE_H::NON_EXISTING_ADDRESS;
    }
    return std::get<1>(*it);
}