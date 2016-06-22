#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<tins/tins.h>

using namespace std;
using namespace Tins;
typedef struct ThreadArgs{
    NetworkInterface iface;
    IPv4Address gw, victim;
    NetworkInterface::Info info;
}THREADARGS;

void *doArpSpoofing(void* args)
{
    THREADARGS &targ = (THREADARGS&)args;

    return 0;
}

int main(int argc, char* argv[])
{

    pthread_t thread;
    IPv4Address gw, victim;
    if(argc != 3){
        cout << "[-] Usage " << argv[0] << " <Gateway> <Victim>" << endl;
        return 1;
    }
    try {
        gw = argv[1];
        victim = argv[2];
    }
    catch(...){
        cout << "[!] Wrong IP Finded!" << endl;
        return 2;
    }
    NetworkInterface iface;
    NetworkInterface::Info info;
    try {
        iface = gw;
        info = iface.info();
    }
    catch(runtime_error& e){
        cout << "[!] " << e.what() << endl;
        return 3;
    }
    try {
        THREADARGS targ;
        targ.iface = iface;
        targ.gw = gw;
        targ.victim = victim;
        targ.info = info;
        pthread_create(&thread,NULL,doArpSpoofing,(void*)&targ);
        pthread_detach(thread);
    }
    catch(runtime_error& e){
        cout << "[!] " << e.what() << endl;
        return 4;
    }

    return 0;
}
