#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <boost/graph/graph_concepts.hpp>

#include "underscore.hpp"
#include "string.hpp"

class eog : public std::exception{
};

template<typename Prev>
class genmap;

template<typename Prev>
class genfilter;

template<typename T>
class genbase{
public:
	using gen_type=T;
};

class gen{
public:
	typedef std::function<std::string (std::string &&)> map_f;
	typedef std::function<bool (const std::string &)> filter_f;
};

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
	
	genmap<genlist> map(gen::map_f &&f);
	genfilter<genlist> filter(gen::filter_f &&f);
};


template<typename Prev>
class genmap{
	Prev _prev;
	gen::map_f _f;
	typedef genmap<Prev> gen_type;
public:
	genmap(const gen::map_f &f, Prev &&prev) : _prev(std::forward<Prev>(prev)), _f(f){};
	genmap(gen_type &&o) : _prev(std::move(o._prev)), _f(std::move(o._f)){};
	genmap(const gen_type &o) : _prev(o._prev), _f(o._f){};
	
	bool eog(){
		return _prev.eog();
	}
	std::string get_next(){
		return _f( this->_prev.get_next() );
	}
	genmap<gen_type> map(gen::map_f &&f){
		return genmap<gen_type>(std::forward<gen::map_f>(f), std::move(*this));
	}
	genfilter<gen_type> filter(gen::filter_f &&f){
		return genfilter<gen_type>(std::forward<gen::filter_f>(f), std::move(*this));
	}
};

template<typename Prev>
class genfilter{
	Prev _prev;
	gen::filter_f _f;
	typedef genfilter<Prev> gen_type;
public:
	genfilter(gen::filter_f &&f, Prev &&prev) : _prev(std::forward<Prev>(prev)), _f(f){}
	genfilter(gen_type &&o) : _prev(std::move(o._prev)), _f(std::move(o._f)){};
	genfilter(const gen_type &o) : _prev(o._prev), _f(o._f){};
	
	bool eog(){
		return _prev.eog();
	}
	std::string get_next(){
		while(!eog()){
			auto v=_prev.get_next();
			if (_f(v))
				return v;
		}
		throw ::eog();
	};
	genmap<genfilter<Prev>> map(gen::map_f &&f){
		return genmap<genfilter<Prev>>(std::forward<gen::map_f>(f), std::move(*this));
	}
	genfilter<genfilter<Prev>> filter(gen::filter_f &&f){
		return genfilter<genfilter<Prev>>(std::forward<gen::filter_f>(f), std::move(*this));
	}
};

genmap<genlist> genlist::map(gen::map_f &&f)
{
		return genmap<genlist>(std::forward<gen::map_f>(f), std::move(*this));
}
genfilter<genlist> genlist::filter(gen::filter_f &&f)
{
	return genfilter<genlist>(std::forward<gen::filter_f>(f), std::move(*this));
}

template<typename T>
std::vector<std::string> to_vector(T &gm){
	std::vector<std::string> r;
	while(!gm.eog()){
		try{
			r.push_back(gm.get_next());
		}
		catch(const eog &g){
			return r;
		}
	}
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
						.map([](std::string &&s){
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
// 			return str.empty();
// 		})
// 	;
	auto v=to_vector(gl);
	
	std::cout<<typeid(gl).name()<<" [";
	for(auto &c: v)
		std::cout<<c<<", ";
	std::cout<<"]"<<std::endl;
}
