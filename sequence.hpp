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
#include <tuple>
#include <map>
#include <functional>

namespace std{
	inline std::string to_string(const std::string &str){ return str; }; // Need to copy it anyway, so no const &.
	inline std::string to_string(const char c){ char tmp[]={c,0}; return std::string(tmp); }; // Need to copy it anyway, so no const &.
};

namespace underscore{
	class string;
	
	/**
	 * @short Wraps any container and add the sequence methods
	 * 
	 * With the sequence methods it is much easier to reason about the operations to do on the lists, 
	 * and chain operations. For example, to print a list:
	 * 
	 * 	std::cout<<_({1,2,3,4,5}).join()<<std::endl;
	 * 
	 * To create cats:
	 * 
	 * 	auto catlist=_({1,2,3,4}).map<Cat>([](int n){ return Cat(n); })
	 * 
	 * It it itself a container, so it can be feed into another sequence (sequence<sequence<std::vector<int>>>, for example).
	 * 
	 * All normal operations as begin, end, size, count and empty are implemented. Then it adds many more to ease list based programming.
	 */
	template<typename T>
	class sequence{
		T _data;
		
		ssize_t _wrap_position(ssize_t p) const{
			ssize_t s=size();
			if (p>s)
				return s;
			if (p<0){
				p=s+p;
				if (p<0)
					return 0;
				return p;
			}
			return p;
		}
	public:
		typedef typename T::value_type value_type;
		typedef typename T::iterator iterator;
		typedef typename T::const_iterator const_iterator;
	public:
		sequence(const T &data) : _data(data) {}
		sequence(T &&data) : _data(data) {}
		sequence() {}

		iterator begin(){ return std::begin(_data); }
		iterator end(){ return std::end(_data); }
		
		const_iterator begin() const { return _data.begin(); }
		const_iterator end() const{ return _data.end(); }
		
		bool empty() const { return _data.empty(); }
		size_t size() const { return _data.size(); }
		size_t count() const { return _data.size(); }
		
		value_type &operator[](int p){ return _data[p]; }
		value_type operator[](int p) const{ return _data[p]; }
		
		/**
		 * @short Joins all elements of the list into a string
		 * 
		 * Each element pass by a std::to_string, and is separated by ", ".
		 * 
		 * Example:
		 * 
		 * 	_(1,6).join() == "1, 2, 3, 4, 5".
		 * 
		 * @param sep The separator string to use.
		 */
		std::string join(const std::string sep=", ") const{
			if (empty())
				return "";
			std::string ret;
			ret.reserve(size()*(std::to_string(_data.at(0)).size()+sep.size()));
			auto I=_data.begin();
			ret=std::to_string(*I);
			++I;
			ret=std::accumulate(I, _data.end(), ret, [&sep](std::string &acc, const value_type &v){
				acc+=sep;
				acc+=std::to_string(v);
				return acc;
			});
			return ret;
		}
		
		/**
		 * @short Filters out all the elements that do no comply to the condition.
		 * 
		 * The condition is a callable with signature "bool (const value_type &)". For example:
		 * 
		 * 	_({"red","green","blue"}).filter([](const std::string &s){ return !s.contains('r'); }) == {"blue"}
		 */
		sequence<T> filter(const std::function<bool (const value_type &)> &f) const{
			sequence<T> ret;
			ret._data.reserve(_data.size());
			std::copy_if(_data.begin(), _data.end(), std::back_inserter(ret._data), f);
			return ret;
		}
		
		/**
		 * @short Removes an element from the list
		 * 
		 * Uses value_type comparison to check if the elements are the same. If so, its removed from the list.
		 * 
		 * Returns a new list.
		 */
		sequence<T> remove(const value_type &v) const{
			sequence<T> ret;
			ret._data.reserve(_data.size());
			std::copy_if(_data.begin(), _data.end(), std::back_inserter(ret._data), [&v](const value_type &m){ return m!=v; });
			return ret;
		}

		/**
		 * @short Applies a mapping function to each element of the list, and return a new one.
		 */
		template<typename S>
		sequence<std::vector<S>> map(const std::function<S (const value_type &)> &f) const{
			std::vector<S> ret;
			ret.reserve(size());
			std::transform(_data.begin(),_data.end(), std::back_inserter(ret), f);
			return sequence<std::vector<S>>(std::move(ret));
		}
		/**
		 * @short Applies a mapping function to each element of the list, and return a new one. Tuple version.
		 * 
		 * The transformation function accepts 2 parameters, one for each element of the vector of tuples.
		 */
		template<typename S, typename A, typename B>
		sequence<std::vector<S>> map(const std::function<S (const A &a, const B &b)> &f) const{
			std::vector<S> ret;
			ret.reserve(size());
			std::transform(_data.begin(),_data.end(), std::back_inserter(ret), [&f](const std::tuple<A,B> &d){
				return f(std::get<0>(d),std::get<1>(d));
			});
			return sequence<std::vector<S>>(std::move(ret));
		}
		/**
		 * @short Applies a mapping function to each element of the list, and return a new one. Same value_type as source, simple transform.
		 */
		sequence<std::vector<value_type>> map(const std::function<value_type (const value_type &)> &f) const{
			return map<value_type>(f);
		}
		
		/**
		 * @short Just executes a function on each element. Returns the same list.
		 */
		sequence<T> each(const std::function<void (const value_type &)> &f) const{
			for(const auto &v: _data)
				f(v);
			return *this;
		}
		
		/**
		 * @short Flattens the application of a map that returns lists.
		 * 
		 * Example:
		 * 	a=_({"Hello","world"}).flatMap<std::string>([](const std::string &s){ return _(s); }) == {'H','e','l','l','o',','w','o','r','l','d'}
		 */
		template<typename S>
		sequence<std::vector<typename S::value_type>> flatMap(const std::function<S (const value_type &)> &f){
			std::vector<typename S::value_type> ret;
			for (auto &v: map(f)){
				std::move(std::begin(v), std::end(v), std::back_inserter(ret));
			}
			return ret;
		}
		
		/**
		 * @short Sorts the elements using default < comparison.
		 */
		sequence<std::vector<value_type>> sort() const{
			std::vector<value_type> ret;
			ret.reserve(size());
			std::copy(_data.begin(), _data.end(), std::back_inserter(ret));
			std::sort(ret.begin(), ret.end());
			return ret;
		}
		
		/**
		 * @short Sorts the elements using default < comparison.
		 */
		sequence<std::vector<value_type>> sort(const std::function<bool (const value_type &,const value_type &)> &lessThan) const{
			std::vector<value_type> ret;
			ret.reserve(size());
			std::copy(_data.begin(), _data.end(), std::back_inserter(ret));
			std::sort(ret.begin(), ret.end(), lessThan);
			return ret;
		}
		
		/**
		 * @short Returns a list with the same elements only once, in the same order.
		 * 
		 * Complexity O(N²). 
		 * 
		 * If sorted pass the true parameter, and it will use O(N)
		 */
		sequence<std::vector<value_type>> unique(bool is_sorted=false) const{
			std::vector<value_type> ret;
			if (is_sorted){
				std::unique_copy(std::begin(_data), std::end(_data), std::back_inserter(ret));
			}
			else{
				for(auto &v: _data){
					if (std::find(std::begin(ret), std::end(ret), v) == std::end(ret))
						ret.push_back(v);
				}
			}
			return ret;
		}

		/**
		 * @short Returns a slice of the list, starting at start until end.
		 * 
		 * Both can be negative numbers that means to use size()-start or size()-end,
		 * so that its possible to return operations with the size, without knowing it. 
		 * For example:
		 * 
		 * 	_({1,2,3,4,5,6}).slice(-1) == {1,2,3,4,5}.
		 */
		sequence<T> slice(ssize_t start, ssize_t end=std::numeric_limits<ssize_t>::max()) const{
			start=_wrap_position(start);
			end=_wrap_position(end);
			auto s=size();
			if (start==0 && end==s)
				return *this;
			std::vector<value_type> ret;
			
			ret.reserve(end-start);
			std::copy(_data.begin()+start, _data.begin()+end, std::back_inserter(ret));
			return ret;
		}

		/**
		 * @short Reverses the list.
		 */
		sequence<T> reverse() const{
			sequence<T> ret;
			ret._data.reserve(size());
			
			std::copy(_data.rbegin(), _data.rend(), std::back_inserter(ret._data));
			
			return ret;
		}
		
		/**
		 * @short Reduces the list: Applies a function on each element starting on the first, and an accumulation value
		 * 
		 * This way, for example it easy to sum all elements:
		 * 
		 * 	_({1,2,3,4,5}).reduce<int>([](int a, int b){ return a+b; }, 0) == 15
		 * 
		 */
		template<typename S>
		S reduce(const std::function<S (const value_type &, const S &)> &f, S initial=S()) const{
			for(auto &v:_data){
				initial=f(v,initial);
			}
			return initial;
		}

		/**
		 * @short Returns the maximum element of the list.
		 */
		value_type max() const{
			return reduce<value_type>([](const value_type &a, const value_type &b){ return std::max(a,b); });
		}
		/**
		 * @short Returns the minimum element of the list.
		 */
		value_type min() const{
			return reduce<value_type>([](const value_type &a, const value_type &b){ return std::min(a,b); }, _data[0]);
		}
		
		/**
		 * @short Finds the index where an element is. 
		 * 
		 * @returns index or -1 if not found.
		 */
		ssize_t find(const value_type &v, ssize_t first=0) const{
			auto s=size();
			auto i=first;
			auto I=begin()+first, endI=end();
			for(;I!=endI;++I, ++i)
				if (*I==v)
					return i;
			return -1;
		}

		/**
		 * @short Checks if any of the elements on the list satisfies the given condition.
		 */
		bool any(const std::function<bool(const value_type &)> &f) const{
			for(auto &v:_data)
				if (f(v))
					return true;
			return false;
		}
		/**
		 * @short Checks if any element of the list has the given value.
		 */
		bool any(const value_type &v){
			for(auto &i: _data)
				if (i==v)
					return true;
			return false;
		}
		
		/**
		 * @short Checks if all elements of the list satisfiy the condition function.
		 */
		bool all(const std::function<bool(const value_type &)> &f) const{
			for(auto &v:_data)
				if (!f(v))
					return false;
			return true;
		}
		/**
		 * @short Checks if all elements are equal to the given value.
		 */
		bool all(const value_type &v) const{
			for(auto &i:_data)
				if (i!=v)
					return false;
			return true;
		}

		/**
		* @short Unzips a list of tuples. Creates two lists, the first with all the first elements, the second with the second elements.
		* 
		* Example:
		* 	
		* 	auto ab=_({{1,'a'},{2,'b'},{3,'c'}})
		* 	auto a=get<0>( ab.unzip<int,char>() ); // == {1, 2, 3, 4}
		*/
		template<typename A_t, typename B_t>
		std::tuple<sequence<std::vector<A_t>>, sequence<std::vector<B_t>>> unzip(){
			std::vector<A_t> A;
			std::vector<B_t> B;
			size_t s=size();
			
			A.reserve(s);
			B.reserve(s);

			for(auto &v: _data){
				A.push_back(std::get<0>(v));
				B.push_back(std::get<1>(v));
			}
			
			return std::make_tuple(sequence<std::vector<A_t>>(std::move(A)),sequence<std::vector<B_t>>(std::move(B)));
		}
		

		template<typename A_t,typename B_t>
		std::map<A_t,B_t> to_map(){
			std::map<A_t,B_t> map;
			for(auto &t: _data){
				map[std::get<0>(t)]=std::get<1>(t);
			}
			return map;
		}
		
		operator std::vector<value_type>(){
			return _data;
		}

	};
};
