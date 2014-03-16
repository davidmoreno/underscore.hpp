/*
 *	Copyright 2014 David Moreno Montero <dmoreno@coralbits.com>
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *			http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */

#pragma once
#include <algorithm>
#include <boost/concept_check.hpp>

namespace underscore{
	class eog : public std::exception{
	};
	
	template<typename Prev>
	class genmap;

	template<typename Prev>
	class genfilter;

	template<typename G>
	class iterator{
		G *parent;
		std::string current;
	public:
		iterator(G *_parent) : parent(_parent){}
		iterator() : parent(nullptr){}
		
		const std::string &operator*(){
			return current;
		}
		iterator<G> operator++(){
			try{
				if (parent)
					current=parent->get_next();
			}
			catch(eog &g){
				current="";
				parent=NULL;
			}
			return *this;
		}
		
		bool operator!=(const iterator<G> &other){
			return parent!=other.parent;
		}
	};
	
	template<typename T>
	class genbase{
	public:
		using gen_type=T;
		
		virtual bool eog() = 0;
		virtual std::string get_next() = 0;
		
		iterator<gen_type> begin(){ return iterator<gen_type>(this); }
		iterator<gen_type> end(){ return iterator<gen_type>(); }
		
		typedef std::function<std::string (const std::string &)> map_f;
		typedef std::function<bool (const std::string &)> filter_f;
	};


	template<typename Prev>
	class genmap{
		Prev _prev;
		typedef genmap<Prev> gen_type;
	public:
		genbase<void>::map_f _f;
		genmap(const genbase<void>::map_f &f, Prev &&prev) : _prev(std::forward<Prev>(prev)), _f(f){};
		genmap(gen_type &&o) : _prev(std::move(o._prev)), _f(std::move(o._f)){};
		
		iterator<gen_type> begin(){ return iterator<gen_type>(this); }
		iterator<gen_type> end(){ return iterator<gen_type>(); }
		
		bool eog(){
			return _prev.eog();
		}
		std::string get_next(){
			return _f( this->_prev.get_next() );
		}
		
		genmap<gen_type> map(genbase<void>::map_f &&f){
			return genmap<gen_type>(std::forward<genbase<void>::map_f>(f), std::move(*this));
		}
		genfilter<gen_type> filter(genbase<void>::filter_f &&f){
			return genfilter<gen_type>(std::forward<genbase<void>::filter_f>(f), std::move(*this));
		}
	};

	template<typename Prev>
	class genfilter{
		Prev _prev;
		typedef genfilter<Prev> gen_type;
	public:
		genbase<void>::filter_f _f;
		genfilter(genbase<void>::filter_f &&f, Prev &&prev) : _prev(std::forward<Prev>(prev)), _f(f){}
		genfilter(gen_type &&o) : _prev(std::move(o._prev)), _f(std::move(o._f)){};
	// 	genfilter(const gen_type &o) : _prev(o._prev), _f(o._f){};

		iterator<gen_type> begin(){ return iterator<gen_type>(this); }
		iterator<gen_type> end(){ return iterator<gen_type>(); }
		
		
		bool eog(){
			return _prev.eog();
		}
		std::string get_next(){
			while(!eog()){
				auto v=_prev.get_next();
				if (_f(v))
					return v;
			}
			throw ::underscore::eog();
		};
		
		genmap<genfilter<Prev>> map(genbase<void>::map_f &&f){
			return genmap<genfilter<Prev>>(std::forward<genbase<void>::map_f>(f), std::move(*this));
		}
		genfilter<genfilter<Prev>> filter(genbase<void>::filter_f &&f){
			return genfilter<genfilter<Prev>>(std::forward<genbase<void>::filter_f>(f), std::move(*this));
		}
	};
};

