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
#include "underscore.hpp"

namespace underscore{
	class eog : public std::exception{
	};
	
	template<typename Prev>
	class genmap;

	template<typename Prev>
	class genfilter;

	template<typename T>
	class generator{
	public:
		class iterator{
		public:
			using parent_type=generator<T>;
		private:
			parent_type *parent;
			std::string current;
		public:
			iterator(parent_type *_parent) : parent(_parent){}
			iterator() : parent(nullptr){}
			
			const std::string &operator*(){
				return current;
			}
			iterator &operator++(){
				try{
					if (parent)
						current=parent->get_next();
				}
				catch(::underscore::eog &g){
					current="";
					parent=NULL;
				}
				return *this;
			}
			
			bool operator!=(const iterator &other){
				return parent!=other.parent;
			}
		};

		using gen_type=T;
		typedef std::function<std::string (const std::string &)> map_f;
		typedef std::function<bool (const std::string &)> filter_f;
		
		virtual bool empty() = 0;
		virtual std::string get_next() = 0;
		
		iterator begin(){ return iterator(this); }
		iterator end(){ return iterator(); }
		
		genmap<T> map(map_f &&f);
		genfilter<T> filter(filter_f &&f);
		
		
		/// Going to list world.
		operator std::vector<std::string>(){
			std::vector<std::string> r;
			try{
				while(!empty()){
					r.push_back(get_next());
				}
			}
			catch(::underscore::eog &e){
			}
			return r;
		}
		
		::underscore::underscore<std::vector<std::string>> sort(){
			std::vector<std::string> v=*this;
			std::sort(std::begin(v), std::end(v));
			return v;
		}
		
	};


	template<typename Prev>
	class genmap : public generator<genmap<Prev>>{
		Prev _prev;
	public:
		generator<void>::map_f _f;
		genmap(const generator<void>::map_f &f, Prev &&prev) : _prev(std::forward<Prev>(prev)), _f(f){};
		genmap(genmap<Prev> &&o) : _prev(std::move(o._prev)), _f(std::move(o._f)){};

		bool empty(){
			return _prev.empty();
		}
		std::string get_next(){
			return _f( this->_prev.get_next() );
		}
	};

	template<typename Prev>
	class genfilter : public generator<genfilter<Prev>>{
		Prev _prev;
	public:
		generator<void>::filter_f _f;
		genfilter(generator<void>::filter_f &&f, Prev &&prev) : _prev(std::forward<Prev>(prev)), _f(f){}
		genfilter(genfilter<Prev> &&o) : _prev(std::move(o._prev)), _f(std::move(o._f)){};
		
		bool empty(){
			return _prev.empty();
		}
		std::string get_next(){
			while(!empty()){
				auto v=_prev.get_next();
				if (_f(v))
					return v;
			}
			throw ::underscore::eog();
		};
	};
	
	
	template<typename T>
	genmap<T> generator<T>::map(map_f &&f){
		return genmap<T>(std::forward<map_f>(f), std::move(*reinterpret_cast<T*>(this)));
	}
	template<typename T>
	genfilter<T> generator<T>::filter(filter_f &&f){
		return genfilter<T>(std::forward<filter_f>(f), std::move(*reinterpret_cast<T*>(this)));
	}
	
	
	/// specific generators
	class vector : public generator<vector>{
		std::vector<std::string> v;
		int n;
	public:
		vector(const std::vector<std::string> &strl) : v(strl), n(0) {}
		vector(vector &&o) : v(std::move(o.v)), n(o.n) {}

		bool empty(){
			return (n>=v.size());
		}
		std::string get_next(){
			if (empty())
				throw ::underscore::eog();
			return v[n++];
		}
	};
	
	
};

