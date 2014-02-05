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
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <boost/concept_check.hpp>

namespace std{
	std::string to_string(std::string str){ return str; }; // Need to copy it anyway, so no const &.
};

namespace underscore{
	template<typename T>
	class underscore{
		T _data;
		typedef typename T::value_type value_type;
		typedef typename T::iterator iterator;
	public:
		underscore(const T &data) : _data(data) {}
		underscore(T &&data) : _data(data) {}
		underscore() {}

		iterator begin(){ return _data.begin(); }
		iterator end(){ return _data.end(); }
		
		bool empty() const { return _data.empty(); }
		size_t size() const { return _data.size(); }
		
		std::string join(const std::string sep=", ") const{
			if (empty())
				return "";
			std::string ret;
			ret.reserve(size()*(std::to_string(_data.at(0)).size()+sep.size()));
			auto I=_data.begin();
			ret=std::to_string(*I);
			++I;
			ret=std::accumulate(I, _data.end(), ret, [&sep](std::string &acc, const typename T::value_type &v){
				acc+=sep;
				acc+=std::to_string(v);
				return acc;
			});
			return ret;
		}
		
		underscore<T> filter(const std::function<bool (const value_type &)> &f) const{
			underscore<T> ret;
			ret._data.reserve(_data.size());
			std::copy_if(_data.begin(), _data.end(), std::back_inserter(ret._data), f);
			return ret;
		}
		underscore<T> remove(const value_type &v) const{
			underscore<T> ret;
			ret._data.reserve(_data.size());
			std::copy_if(_data.begin(), _data.end(), std::back_inserter(ret._data), [&v](const value_type &m){ return m!=v; });
			return ret;
		}

		template<typename S>
		underscore<std::vector<S>> map(const std::function<S (const value_type &)> &f) const{
			std::vector<S> ret;
			ret.reserve(size());
			std::transform(_data.begin(),_data.end(), std::back_inserter(ret), f);
			return underscore<std::vector<S>>(std::move(ret));
		}
		
		underscore<T> sort() const{
			std::vector<value_type> ret;
			ret.reserve(size());
			std::copy(_data.begin(), _data.end(), std::back_inserter(ret));
			std::sort(ret.begin(), ret.end());
			return ret;
		}

		underscore<T> slice(ssize_t start) const{
			return slice(start, size());
		}

		underscore<T> head(ssize_t end) const{
			return slice(0,end);
		}
		underscore<T> tail(ssize_t end) const{
			return slice(end,size());
		}

		underscore<T> slice(ssize_t start, ssize_t end) const{
			auto s=size();
			std::vector<value_type> ret;
			if (end<0)
				end=s+end;
			if (end<0)
				return ret;
			if (end>s)
				end=s;
			if (start<0)
				start=s+start;
			if (start<0)
				start=0;
			if (start>s)
				return ret;
			if (start==0 && end==s)
				return *this;
			
			ret.reserve(end-start);
			std::copy(_data.begin()+start, _data.begin()+end, std::back_inserter(ret));
			return ret;
		}

		underscore<T> reverse() const{
			underscore<T> ret;
			ret._data.reserve(size());
			
			std::copy(_data.rbegin(), _data.rend(), std::back_inserter(ret._data));
			
			return ret;
		}
		
		template<typename S>
		S reduce(const std::function<S (const value_type &, const S &)> &f, S initial=S()) const{
			for(auto &v:_data){
				initial=f(v,initial);
			}
			return initial;
		}
		
		value_type max() const{
			return reduce<value_type>([](const value_type &a, const value_type &b){ return std::max(a,b); });
		}
		value_type min() const{
			return reduce<value_type>([](const value_type &a, const value_type &b){ return std::min(a,b); }, _data[0]);
		}
		
		int find(const value_type &v, int first=0) const{
			int s=size();
			int i;
			for(i=first;i<s;++i)
				if (_data[i]==v)
					return i;
			return -1;
		}
		
		bool any(const std::function<bool(const value_type &)> &f) const{
			for(auto &v:_data)
				if (f(v))
					return true;
			return false;
		}
		bool any(const value_type &v){
			for(auto &i: _data)
				if (i==v)
					return true;
			return false;
		}
		
		bool all(const std::function<bool(const value_type &)> &f) const{
			for(auto &v:_data)
				if (!f(v))
					return false;
			return true;
		}
		bool all(const value_type &v) const{
			for(auto &i:_data)
				if (i!=v)
					return false;
			return true;
		}
	};
	
	underscore<std::vector<std::string>> __(const std::string &orig, const char &sep=','){
		std::vector<std::string> v;
		std::stringstream ss(orig);
		std::string el;
		while (std::getline(ss, el, sep)){
			v.push_back(el);
		}
		return underscore<std::vector<std::string>>(std::move(v));
	}
	
	underscore<std::vector<std::string>> __(std::istream &&input){
		std::vector<std::string> data;
		std::string str;
		while (!input.eof()){
			std::getline(input, str);
			data.push_back(str);
		}
		return underscore<std::vector<std::string>>(std::move(data));
	}
	
	template<typename T>
	underscore<T> _(const T &v){
		return underscore<T>(v);
	}
	template<typename T>
	underscore<T> _(T &&v){
		return underscore<T>(std::forward<T>(v));
	}
};
