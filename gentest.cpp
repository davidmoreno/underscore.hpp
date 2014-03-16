#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <fstream>
#include <iostream>
#include <memory>

#include "underscore.hpp"
#include "string.hpp"
#include "generator.hpp"
#include "file.hpp"

using namespace underscore;


int main(void){
	std::vector<std::string> gl=vector({"Hola","Mundo!","Mundito","Que","Tal"})
						.filter([](const std::string &s){
							return s.length()>4;
						})
						.map([](const std::string &s){
							return s+"...";
							})
						.filter([](const std::string &s){
							return s.length()>4;
						})
						.map([](const std::string &s){
							return s;
							})
						.filter([](const std::string &s){
							return s.length()>0;
						})
						.map([](const std::string &s){
								return std::string("...")+s;
							})
		;

	std::cout<<typeid(gl).name()<<" [";
	for(auto &c: gl)
		std::cout<<c<<", ";
	std::cout<<"]"<<std::endl;
	
	
	int n=0;
	auto gen=file("/etc/services")
				.map([](const ::underscore::string &str) -> std::string{
// 					std::cout<<&str<<std::endl;
					if (str.contains('#'))
						return str.slice(0,str.index('#')).strip();
					return str;
				})
				.filter([](const ::underscore::string &str){ 
// 					std::cout<<&str<<std::endl;
					return !str.empty() && str.endswith("/tcp"); 
				})
				.map([](const ::underscore::string &str) -> std::string{
					return str.slice(0,-4);
				});
	for(auto &c: gen.slice(0,5)){
		n++;
	}
	std::cout<<n<<std::endl;
// 	for(auto &c: gen.slice(0,5)){
// 		n++;
// 	}
// 	std::cout<<n<<std::endl;
}
