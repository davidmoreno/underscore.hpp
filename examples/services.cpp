#include <fstream>
#include <map>
#include <iostream>
#include <fstream>

#include "../underscore.hpp"
#include "../generator.hpp"
#include "../file.hpp"

using namespace underscore;

void using_underscore();
void using_generator();
void manual();

int main(int argc, char **argv){
	using_underscore();
	using_generator();
	manual();
}

void using_generator(){
	
	
}


void manual(){
	std::ifstream input("/etc/services", std::ifstream::in);
	std::map<std::string, int> map;
	std::string str;
	while (!input.eof()){
		std::getline(input, str); // Get line
		
		str=str.substr(0, str.find_first_of('#')); // Remove comments, and trim
		auto i=str.find_first_not_of(' '); auto end=str.find_last_not_of(' ');
		if (i==end) 
			str=std::string();
		else
			str=str.substr(i,end+1);
		
		if (str.empty()) // do not process empty lines
			continue; 
		
		auto t=std::make_tuple<std::string,std::string>(str.substr(0,str.find_first_of(' ')), str.substr(str.find_last_of(' '))); // Create tuples <service, port/prot>
		
		if (std::get<0>(t).length()==0 || std::get<1>(t).length()==0) // Only tcp ports
			continue; 
		std::string port=std::get<1>(t);
		if (port.size()<4 || port.substr(port.size()-4)!="/tcp") 
			continue;
		
		auto p=std::make_tuple(std::get<0>(t), std::stoi( port.substr(0,port.size()-4) )); // Create <service, port> Pair
		
		map[std::get<0>(p)]=std::get<1>(p); // Create map.
	}
	
	
	for(auto &s: {"http","ssh","telnet"} )
		std::cout<<s<<" at tcp port "<<map[s]<<std::endl;
	;
}


void using_underscore(){
	std::map<std::string, int> services;
	file(std::ifstream("/etc/services"))
		.map([](const string &s) -> string{ // Remove comments, and trim
			auto r=s.split('#',true);
			if (r.count()==0)
				return string();
			return r[0];
		})
		.filter([](const string &s){ // Remove empty lines and lines without /tcp
			return s.contains("/tcp"); 
		})
		.map<string_list>([](const string &s){  // Prepare pairs, {service, port/type}
				return s.split(' ');
		})
		.filter([](const string_list &t){ // Only tcp ports
			return t[1].endswith("/tcp");
		})
		.each([&services](const string_list &t){ // Prepare pairs
			services[t[0]]=t[1].slice(0,-5).to_long();
		});
	
	for(auto &s: {"http","ssh","telnet"} )
		std::cout<<s<<" at tcp port "<<services[s]<<std::endl;
	;
}
