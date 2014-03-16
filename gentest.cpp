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

using namespace underscore;

class genlist : public genbase<genlist>{
	std::vector<std::string> v;
	int n;
public:
	genlist(const std::vector<std::string> &strl) : v(strl), n(0) {}
	genlist(genlist &&o) : v(std::move(o.v)), n(o.n) {}

	genlist(const gen_type &o) = delete;
	gen_type &operator=(gen_type &&o) = delete;
	gen_type &operator=(const gen_type &o) = delete;
	
	bool eog(){
		return (n>=v.size());
	}
	std::string get_next(){
		if (eog())
			throw ::eog();
		return v[n++];
	}
	void process(const std::function<void (const std::string &)> &_next){
		for(const auto &s: v)
			_next(s);
	}
	
	genmap<genlist> map(genbase<void>::map_f &&f);
	genfilter<genlist> filter(genbase<void>::filter_f &&f);
};

class genfile : public genbase<genfile>{
	std::unique_ptr<std::ifstream> ifs; // Workaround no &&ifstream in gcc 4.8 as in http://stackoverflow.com/questions/12015899/why-are-move-semantics-for-a-class-containing-a-stdstringstream-causing-compil
	std::string next; // To reuse lines.
public:
// 	genfile(std::ifstream &&_ifs) : ifs(_ifs){
// 	}
	
	genfile(const std::string &filename) : ifs(new std::ifstream(filename, std::ifstream::in)) { 
	}
	genfile(genfile &&o){
		ifs=std::move(o.ifs);
	}
	
	bool eog(){
		return ifs->eof();
	}
	std::string get_next(){
		if (eog())
			throw ::eog();
		std::getline(*ifs, next);
		return next;
	};
	genmap<genfile> map(genbase<void>::map_f &&f);
	genfilter<genfile> filter(genbase<void>::filter_f &&f);
};




genmap<genlist> genlist::map(genbase<void>::map_f &&f)
{
	return genmap<genlist>(std::forward<genbase<void>::map_f>(f), std::move(*this));
}

genfilter<genlist> genlist::filter(genbase<void>::filter_f &&f)
{
	return genfilter<genlist>(std::forward<genbase<void>::filter_f>(f), std::move(*this));
}

genmap<genfile> genfile::map(genbase<void>::map_f &&f)
{
	return genmap<genfile>(std::forward<genbase<void>::map_f>(f), std::move(*this));
}

genfilter<genfile> genfile::filter(genbase<void>::filter_f &&f)
{
	return genfilter<genfile>(std::forward<genbase<void>::filter_f>(f), std::move(*this));
}

template<typename T>
std::vector<std::string> to_vector(T &&gm){
	std::vector<std::string> r;
#if 0
	gm.process([&r](const std::string &s){
		r.push_back(s);
	});
#else
 	while(!gm.eog()){
		try{
			r.push_back(gm.get_next());
		}
		catch(::eog &e){
			return r;
		}
	}
#endif
	return r;
}

int main(void){
	auto gl=genlist({"Hola","Mundo!","Mundito","Que","Tal"})
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
// 	auto gl=genlist({"Hola","Mundo",""})
// 		.filter([](const std::string &str){
// 			return !str.empty();
// 		})
// 		.map([](const std::string &str){
// 			return str+"...";
// 		})
// 	;
	auto v=to_vector(gl);
	
	std::cout<<typeid(gl).name()<<" [";
	for(auto &c: v)
		std::cout<<c<<", ";
	std::cout<<"]"<<std::endl;
	
	
	for (auto &c:genfile("/etc/services")
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
				})
			){
		std::cout<<c<<std::endl;
	}
	
	
}
