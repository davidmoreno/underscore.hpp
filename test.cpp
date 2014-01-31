#include "underscore.hpp"
#include <vector>
#include <iostream>
#include <fstream>

using namespace underscore;

int main(){
	std::vector<int> v{1,2,3,4,5};
	
	auto vv = _(std::move(v));
	
	std::cout<<"join "<<vv.join()<<std::endl;
	std::cout<<"filter join "<<vv.filter([](int v){ return v%2; }).join()<<std::endl;
	std::cout<<"map join "<<vv.map<int>([](int v){ return v*2; }).join()<<std::endl;
	std::cout<<"reverse join "<<vv.reverse().join()<<std::endl;
	std::cout<<"sort join "<<vv.sort().join()<<std::endl;
	std::cout<<"filter map reverse ["<<vv.
		filter([](int v)           { return v!=3; }).
		map<std::string>([](int v) { return std::string("+")+std::to_string(v)+std::string("+"); }).
		reverse().
		join("][")<<"]"<<std::endl;
		
	std::cout<<"head join "<<vv.head(2).join()<<std::endl;
	std::cout<<"head- join "<<vv.head(-2).join()<<std::endl;
	std::cout<<"tail- join "<<vv.tail(-2).join()<<std::endl;
	std::cout<<"tail join "<<vv.tail(2).join()<<std::endl;
	
	std::cout<<__(std::ifstream("/etc/services"))
								.filter([](const std::string &s){ return !s.empty() && s[0]!='#'; })
								.map<std::string>([](const std::string &s){ return s.substr(0,s.find(" ")); })
								.sort()
								.head(5)
								.join("\n")<<std::endl;
								
	std::cout<<__("Hola, mundo").join("\n")<<std::endl;
}
