#include<iostream>
#include<unistd.h>
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
    THREADARGS *targ = (THREADARGS*)args;
    PacketSender send;
    EthernetII::address_type gw_hw,victim_hw;

    gw_hw = Utils::resolve_hwaddr(targ->iface,targ->gw,send);
    victim_hw = Utils::resolve_hwaddr(targ->iface,targ->victim,send);

    cout << "[*] Using Gateway HW Address   " << gw_hw << endl;
    cout << "[*] Using Victim HW Address    " << victim_hw << endl;
    cout << "[*] Using Own HW Address       " << targ->info.hw_addr << endl;
    exit(0);
}

int main(int argc, char* argv[])
{
    pid_t pid;
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
    cout << "[-] Starting ARP Spoofing..." << endl;
    THREADARGS targ;
    targ.iface = iface;
    targ.gw = gw;
    targ.victim = victim;
    targ.info = info;
    try {
        pid = fork();
        if(pid == 0)
            doArpSpoofing((void*)&targ);
    }
    catch(runtime_error& e){
        cout << "[!] " << e.what() << endl;
        return 4;
    }

    return 0;
}
