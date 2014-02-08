#include <fstream>
#include <map>
#include <iostream>

#include "../underscore.hpp"

using namespace underscore;

int main(int argc, char **argv){
	// Convert it all to a map. If several ports per service, only one.
	//std::map<std::string, int> 
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
