#include <iostream>
#include "example.pb.h"

int main() {
	Person p1;
	p1.set_name("Josh");
	p1.set_id(1);
	std::cout << "Name: " << p1.name() << std::endl;
	std::cout << "ID: " << p1.id() << std::endl;
}
