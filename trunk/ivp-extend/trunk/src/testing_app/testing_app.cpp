#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc,char **argv)
{
	ifstream fs;

	unsigned long last_active=0, last_idle=0;

    while (true) {
    	fs.open("/proc/stat", ifstream::in);
    	char input[256];
    	fs.getline(input, 256);
    	cout << input << endl;
    	unsigned long user, nice, system, idle, iowait, irq, softirq;
    	sscanf(input,"%*s %lu %lu %lu %lu %lu %lu %lu",
    			&user,
    			&nice,
    			&system,
    			&idle,
    			&iowait,
    			&irq,
    			&softirq);

    	unsigned long total_active = user + nice + system + iowait + irq + softirq;

    	double percent = 100 * (total_active - last_active) / (total_active - last_active + idle - last_idle);
    	last_active = total_active;
    	last_idle = idle;

    	cout << "Percent: " << percent << endl;

    	fs.close();

    	fs.open("/proc/meminfo", ifstream::in);

    	unsigned int memtotal, memfree, buffers, cached;
    	int complete = 0;
    	while (complete < 4) {
    		fs.getline(input, 256);
//    		cout << input << endl;
    		if (fs.eof()) break;
    		if (sscanf(input, "MemTotal: %u kB", &memtotal) != 0)
    			complete++;
    		else if (sscanf(input, "MemFree: %u kB", &memfree) != 0)
    			complete++;
    		else if (sscanf(input, "Buffers: %u kB", &buffers) != 0)
    			complete++;
    		else if (sscanf(input, "Cached: %u kB", &cached) != 0)
    			complete++;
    	}
    	if (complete == 4) {
    		int total_free = memfree + buffers + cached;
    		double percent_use = 100 * (memtotal - total_free) / memtotal;
    		cout << percent_use << endl;
    	}
//    		cout << memtotal << "  " << memfree << "  " << buffers << "  " << cached << endl;
    	fs.close();

    	usleep(1000000);
    }

	return 0;
}
