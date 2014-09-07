#include <iostream>
#include "JoshUtils.h"

using namespace std;
using namespace JoshUtil;

int main(int argc,char **argv) {
    TDMAEngine tdma;
    tdma.appendSlot(2);
    tdma.appendSlot(1);

    tdma.run();
    while(true) {
        tdma.testAdvance();
        //for (int i=0; i<50; i++) {
            cout << tdma.getCurrentSlot() << "  ";
            cout << tdma.getTimeWithinSlot() << "  ";
            cout << tdma.getTimeToNextSlot() << "  ";
            cout << tdma.getCycleCount() << "  ";
            cout << tdma.getCycleNumber() << endl;
            usleep(200000);
        //}
    }
}
