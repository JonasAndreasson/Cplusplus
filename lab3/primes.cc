#include <string>
#include <iostream>
using std::cout;
using std::string;
int main(){
    string primes(100001, 'P');
    primes.replace(0, 2, "CC");
    bool searching = true;
    unsigned int i = 2;
    unsigned int p = i;
    while(searching){
        unsigned long int loc = primes.find('P',i);
        if (loc != string::npos && loc != p && loc % p == 0){
            primes.replace(loc, 1, "C");
        }
        i+=p;
        if (i > 100000){
            p = primes.find('P', p+1);
            i = p;
            if (p > 100000){
                searching = false;
            }
        }
    }
    cout << primes.substr(0, 201) << "\n";
    cout << "Biggest prime less than 100000: "  <<primes.find_last_of('P') << std::endl;
}