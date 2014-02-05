#include "underscore.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/concept_check.hpp>

using namespace underscore;

int main(){
	std::vector<int> v{1,2,3,4,5};
	
	const auto vv = _(std::move(v));
	
	std::cout<<"join "<<vv.join()<<std::endl;
	std::cout<<"filter join "<<vv.filter([](int v){ return v%2; }).join()<<std::endl;
	std::cout<<"remove join "<<vv.remove(5).join()<<std::endl;
	std::cout<<"map join "<<vv.map<int>([](int v){ return v*2; }).join()<<std::endl;
	std::cout<<"reverse join "<<vv.reverse().join()<<std::endl;
	std::cout<<"sort join "<<vv.sort().join()<<std::endl;
	std::cout<<"filter map reverse ["<<vv.
		filter([](int v)           { return v!=3; }).
		map<std::string>([](int v) { return std::string("+")+std::to_string(v)+std::string("+"); }).
		reverse().
		join("][")<<"]"<<std::endl;
		
	std::cout<<"---"<<std::endl;
	
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
	
	std::cout<<"---"<<std::endl;
	
	std::cout<<"sum "<<_({1,2,3,4,5,6}).reduce<int>([](int a, int b){ return a+b; }, 0)<<std::endl;
	std::cout<<"max "<<_({1,2,3,4,5,6}).max()<<std::endl;
	std::cout<<"min "<<_({1,2,3,4,5,6}).min()<<std::endl;

	std::cout<<"---"<<std::endl;

	const std::vector<int> v2{1,2,3,4,5};
	for(auto &i: v2){
		std::cout<<i<<std::endl;
	}

// 	for access. FIXME, no sort.
	for(auto &v: vv.sort()){
		std::cout<<v<<std::endl;
	}

	std::cout<<"---"<<std::endl;
	
	
	std::cout<<
		zip(std::vector<int>{1,2,3},std::vector<char>{'a','b','c','d'})
			.map<std::string>([](const std::tuple<int,char> &p){ return std::to_string(std::get<0>(p))+"--"+char(std::get<1>(p)); })
			.join()
			<<std::endl;
	std::cout<<
		zip(std::vector<int>{1,2,3,4},std::vector<char>{'a','b','c'})
			.map<std::string>([](const std::tuple<int,char> &p){ 
				int a; char b;
				std::tie(a,b) = p;
				return std::to_string(a)+"--"+char(b); 
			})
			.join()
			<<std::endl;
	std::cout<<
		zip({1,2,3,4},std::vector<char>{'a','b','c','d'})
			.map<std::string, int, char>([](int a, char b){ return std::to_string(a)+"--"+char(b); })
			.join()
			<<std::endl;
	std::cout<<
		zip({1,2,3,4},{'a','b','c','d'})
			.map<std::string, int, char>([](int a, char b){ return std::to_string(a)+"--"+char(b); })
			.join()
			<<std::endl;
}
