#include <fstream>
#include <map>
#include <iostream>
#include <fstream>

#include "../underscore.hpp"
#include "../streams.hpp"

using namespace underscore;

void using_underscore();
void using_streams();
void manual();

int main(int argc, char **argv){
	// Convert it all to a map. If several ports per service, only one.
	//std::map<std::string, int> 
	//using_underscore();
	using_streams();
	manual();
}

void using_streams(){
	stream<std::string> s;
	
	
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
	auto services= 
		__(std::ifstream("/etc/services"))
			.map<std::string>([](const std::string &s){ // Remove comments, and trim
				auto nocom=s.substr(0, s.find_first_of('#'));
				auto i=nocom.find_first_not_of(' '); auto end=nocom.find_last_not_of(' ');
				if (i==end)
					return std::string();
				return nocom.substr(i,end+1);
			})
			.filter([](const std::string &s){ // Remove empty lines
				return !s.empty(); 
			})
			.map<std::tuple<std::string,std::string>>( // Prepare pairs, {service, port/type}
				[](const std::string &s){
					auto data=__(s,' ').remove("");
					return std::make_tuple(data[0], data[1]);
			})
 			.filter([](const std::tuple<std::string, std::string> &t){ // Only tcp ports
				if (std::get<0>(t).empty() || std::get<1>(t).empty())
					return false;
				std::string port=std::get<1>(t);
				return !port.empty() && port.substr(port.size()-4)=="/tcp";
			})
			.map<std::pair<std::string, int>>([](const std::tuple<std::string, std::string> &t){ // Prepare pairs
				std::string service=std::get<0>(t);
				std::string port=std::get<1>(t);
				return std::make_pair(service, stoi( port.substr(0,port.size()-4) ) );
			})
			.to_map<std::string, int>() // And convert to map.
			;
	
	for(auto &s: {"http","ssh","telnet"} )
		std::cout<<s<<" at tcp port "<<services[s]<<std::endl;
	;
}
