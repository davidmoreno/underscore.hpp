#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace std{
	std::string to_string(std::string str){ return str; }; // Need to copy it anyway, so no const &.
};

namespace underscore{
	template<typename T>
	class underscore{
		T _data;
	public:
		underscore(const T &data) : _data(data) {}
		underscore(T &&data) : _data(data) {}
		underscore() {}

		typename T::iterator begin(){ return _data.begin(); }
		typename T::iterator end(){ return _data.end(); }
		
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
		
		underscore<T> filter(const std::function<bool (typename T::value_type)> &f) const{
			underscore<T> ret;
			ret._data.reserve(_data.size());
			std::copy_if(_data.begin(), _data.end(), std::back_inserter(ret._data), f);
			return ret;
		}

		template<typename S>
		underscore<std::vector<S>> map(const std::function<S (typename T::value_type)> &f) const{
			std::vector<S> ret;
			ret.reserve(size());
			std::transform(_data.begin(),_data.end(), std::back_inserter(ret), f);
			return underscore<std::vector<S>>(std::move(ret));
		}
		
		underscore<T> sort() const{
			std::vector<typename T::value_type> ret;
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
			std::vector<typename T::value_type> ret;
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
