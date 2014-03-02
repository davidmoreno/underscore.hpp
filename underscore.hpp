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

namespace std{
	inline std::string to_string(const std::string &str){ return str; }; // Need to copy it anyway, so no const &.
	inline std::string to_string(const char c){ char tmp[]={c,0}; return std::string(tmp); }; // Need to copy it anyway, so no const &.
};

namespace underscore{
	class string;
	
	/**
	 * @short Range between two iterators. 
	 * 
	 * WARNING Not all iterators can be used. Until more concept checking is in place, use it at your own risk. std::vector::iterators are safe.
	 */
	template<typename I>
	class range{
		I _begin;
		I _end;
	public:
		typedef typename I::value_type value_type;
		typedef I iterator;
		typedef const I const_iterator;
		
		range(I begin, I end) : _begin(begin), _end(end) {}
		range(I &&begin, I &&end) : _begin(begin), _end(end) {}
		
		iterator begin() const { return _begin; }
		iterator end() const { return _end; }
		value_type &at(size_t p){ return *(_begin+p); }
		value_type at(size_t p) const { return *(_begin+p); }
		size_t size() const { return _end-_begin; }
		bool empty() const { return _begin==_end; }
	};
	
	/**
	 * @short Range between two numbers.
	 * 
	 * Allows easy create or runtime ranges:
	 * 
	 * 	auto a=_(0,1000).filter([](int n){ return n%2; }).join('<')
	 */
	template<>
	class range<int>{
	public:
		typedef int value_type;
		/**
		 * @short To allow normal range operations many int operations must be reimplemented to simulate an iterator.
		 */
		class iterator : public std::iterator<std::output_iterator_tag,int>{
			value_type i;
		public:
			iterator(value_type n) : i(n) {}
			value_type &operator*(){ return i; }
			iterator &operator--(){ --i; return *this; }
			iterator &operator++(){ ++i; return *this; }
			iterator &operator+=(value_type n){ i+=n; return *this; }
			iterator operator+(value_type n) const{ return iterator(i+n); }
			iterator operator-(value_type n) const{ return iterator(i-n); }
			
			ssize_t operator-(const iterator &o) const{ return (i-o.i); }
			bool operator==(const iterator &o) const{ return i==o.i; }
			bool operator!=(const iterator &o) const{ return i!=o.i; }
		};
		typedef const iterator const_iterator;
	private:
		iterator _begin;
		iterator _end;
	public:
		range(value_type begin, value_type end) : _begin(begin), _end(end){}
		iterator begin() const { return _begin; }
		iterator end() const { return _end; }
		value_type &at(size_t p){ return *(_begin+p); }
		value_type at(size_t p) const { return *(_begin+p); }
		size_t size() const { return _end-_begin; }
		bool empty() const { return _begin==_end; }
	};
	
	template<typename T>
	inline range<T> make_range(T &&begin, T &&end){
		return range<T>(std::forward(begin), std::forward(end));
	}
	inline range<int> make_range(int begin, int end){
		return range<int>(begin, end);
	}
	
	/**
	 * @short Wraps any container and add the underscore methods
	 * 
	 * With the underscore methods it is much easier to reason about the operations to do on the lists, 
	 * and chain operations. For example, to print a list:
	 * 
	 * 	std::cout<<_({1,2,3,4,5}).join()<<std::endl;
	 * 
	 * To create cats:
	 * 
	 * 	auto catlist=_({1,2,3,4}).map<Cat>([](int n){ return Cat(n); })
	 * 
	 * It it itself a container, so it can be feed into another underscore (underscore<underscore<std::vector<int>>>, for example).
	 * 
	 * All normal operations as begin, end, size, count and empty are implemented. Then it adds many more to ease list based programming.
	 */
	template<typename T>
	class underscore{
		T _data;
		typedef typename T::value_type value_type;
		typedef typename T::iterator iterator;
		typedef typename T::const_iterator const_iterator;
	public:
		underscore(const T &data) : _data(data) {}
		underscore(T &&data) : _data(data) {}
		underscore() {}

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
		underscore<T> filter(const std::function<bool (const value_type &)> &f) const{
			underscore<T> ret;
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
		underscore<T> remove(const value_type &v) const{
			underscore<T> ret;
			ret._data.reserve(_data.size());
			std::copy_if(_data.begin(), _data.end(), std::back_inserter(ret._data), [&v](const value_type &m){ return m!=v; });
			return ret;
		}

		/**
		 * @short Applies a mapping function to each element of the list, and return a new one.
		 */
		template<typename S>
		underscore<std::vector<S>> map(const std::function<S (const value_type &)> &f) const{
			std::vector<S> ret;
			ret.reserve(size());
			std::transform(_data.begin(),_data.end(), std::back_inserter(ret), f);
			return underscore<std::vector<S>>(std::move(ret));
		}
		/**
		 * @short Applies a mapping function to each element of the list, and return a new one. Tuple version.
		 * 
		 * The transformation function accepts 2 parameters, one for each element of the vector of tuples.
		 */
		template<typename S, typename A, typename B>
		underscore<std::vector<S>> map(const std::function<S (const A &a, const B &b)> &f) const{
			std::vector<S> ret;
			ret.reserve(size());
			std::transform(_data.begin(),_data.end(), std::back_inserter(ret), [&f](const std::tuple<A,B> &d){
				return f(std::get<0>(d),std::get<1>(d));
			});
			return underscore<std::vector<S>>(std::move(ret));
		}
		/**
		 * @short Applies a mapping function to each element of the list, and return a new one. Same value_type as source, simple transform.
		 */
		underscore<std::vector<value_type>> map(const std::function<value_type (const value_type &)> &f) const{
			return map<value_type>(f);
		}
		
		/**
		 * @short Just executes a function on each element. Returns the same list.
		 */
		underscore<T> each(const std::function<void (const value_type &)> &f) const{
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
		underscore<std::vector<typename S::value_type>> flatMap(const std::function<S (const value_type &)> &f){
			std::vector<typename S::value_type> ret;
			for (auto &v: map(f)){
				std::move(std::begin(v), std::end(v), std::back_inserter(ret));
			}
			return ret;
		}
		
		/**
		 * @short Sorts the elements using default < comparison.
		 */
		underscore<std::vector<value_type>> sort() const{
			std::vector<value_type> ret;
			ret.reserve(size());
			std::copy(_data.begin(), _data.end(), std::back_inserter(ret));
			std::sort(ret.begin(), ret.end());
			return ret;
		}
		
		/**
		 * @short Returns a list with the same elements only once, in the same order.
		 * 
		 * Complexity O(N²). 
		 * 
		 * If sorted pass the true parameter, and it will use O(N)
		 */
		underscore<std::vector<value_type>> unique(bool is_sorted=false) const{
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
		 * @short Returns a slice of the original list, starting at start until the end of the list.
		 */
		underscore<T> slice(ssize_t start) const{
			return slice(start, size());
		}

		/**
		 * @short Returns the first n elements
		 */
		underscore<T> head(ssize_t count) const{
			return slice(0,count);
		}
		/**
		 * @short Returns the tail of the list, starting at start_at.
		 */
		underscore<T> tail(ssize_t start_at) const{
			return slice(start_at,size());
		}

		/**
		 * @short Returns a slice of the list, starting at start until end.
		 * 
		 * Both can be negative numbers that means to use size()-start or size()-end,
		 * so that its possible to return operations with the size, without knowing it. 
		 * For example:
		 * 
		 * 	_({1,2,3,4,5,6}).head(-1) == {1,2,3,4,5}.
		 */
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

		/**
		 * @short Reverses the list.
		 */
		underscore<T> reverse() const{
			underscore<T> ret;
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
		std::tuple<underscore<std::vector<A_t>>, underscore<std::vector<B_t>>> unzip(){
			std::vector<A_t> A;
			std::vector<B_t> B;
			size_t s=size();
			
			A.reserve(s);
			B.reserve(s);

			for(auto &v: _data){
				A.push_back(std::get<0>(v));
				B.push_back(std::get<1>(v));
			}
			
			return std::make_tuple(underscore<std::vector<A_t>>(std::move(A)),underscore<std::vector<B_t>>(std::move(B)));
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
	
	/**
	 * @short Creates an underscore container of the given value. It copies the data.
	 * 
	 * Example:
	 * 	const std::vector<int> v{1,2,3,4};
	 * 	auto a=_(v);
	 */
	template<typename T>
	inline underscore<T> _(const T &v){
		return underscore<T>(v);
	}
	/**
	 * @short Creates an underscore container of the given value. Perfect forwarding version (std::move)
	 * 
	 * Example:
	 * 	auto v=std::vector<int>{1,2,3,4};
	 * 	auto a=_(std::move(v))
	 * 
	 * Or:
	 * 
	 * 	auto a=_(std::vector<int>{1,2,3,4});
	 */
	template<typename T>
	inline underscore<T> _(T &&v){
		return underscore<T>(std::forward<T>(v));
	}
	/**
	 * @short Creates an underscore container with a vector of the elements into the initializer list.
	 * 
	 * Allows creation directly as:
	 * 
	 * 	auto a=_({1,2,3,4})
	 */
	template<typename T>
	inline underscore<std::vector<T>> _(std::initializer_list<T> &&v){
		return underscore<std::vector<T>>(std::vector<T>(std::forward<std::initializer_list<T>>(v)));
	}
	
	/**
	 * @short Creates anunderscore container from two ranges. Useful for subranges.
	 * 
	 * It needs the ability to copy iterators.
	 */
	template<typename I>
	inline underscore<range<I>> _(I &&begin, I &&end){
		return _(range<I>(begin, end));
	}
	
	/**
	 * @short Encapsulate a string.
	 */
	::underscore::string _(std::string &&s);
	::underscore::string _(const char *s);
	
	/**
	 * @short zips two lists into one of tuples
	 * 
	 * Example:
	 * 	zip({1,2,3,4}, {'a','b','c','d'}) == {{1,'a'},{2,'b'},{3,'c'},{4,'d'}}
	 * 
	 * This version allows two standard containers
	 * 
	 * If the lists are uneven (diferent sizes) it creates new elements of the necesary type for the side with less elements:
	 * 
	 * Example: 
	 * 	zip({1,2}, {'a','b','c','d'}) == {{1,'a'},{2,'b'},{0,'c'},{0,'d'}}
	 */
	template<typename A, typename B>
	inline underscore<std::vector<std::tuple<typename A::value_type, typename B::value_type>>> zip(const A &a, const B &b){
		typedef std::tuple<typename A::value_type, typename B::value_type> ret_t;
		std::vector<ret_t> ret;
		ret.reserve(std::max(a.size(), b.size()));
		
		auto ia=std::begin(a);
		auto ea=std::end(a);
		auto ib=std::begin(b);
		auto eb=std::end(b);
		while (ia!=ea || ib!=eb){
			if (ia==ea){
				auto va=typename A::value_type();
				ret.push_back(std::make_tuple(va, *ib));
				++ib;
			}
			else if (ib==eb){
				auto vb=typename B::value_type();
				ret.push_back(std::make_tuple(*ia, std::move(vb)));
				++ia;
			}
			else{
				ret.push_back(std::make_tuple(*ia, *ib));
				++ia;
				++ib;
			}
		}
		
		return ret;
	}
	/**
	 * @short zip two lists into a list of tuples.
	 * 
	 * Specialization with the first as a initializer list.
	 */
	template<typename A_t, typename B>
	inline underscore<std::vector<std::tuple<A_t, typename B::value_type>>> zip(std::initializer_list<A_t> &&a, B &&b){
		return zip(a, b);
	}
	/**
	 * @short zip two lists into a list of tuples.
	 * 
	 * Specialization with the two as initializer list.
	 */
	template<typename A_t, typename B_t>
	inline underscore<std::vector<std::tuple<A_t, B_t>>> zip(std::initializer_list<A_t> &&a, std::initializer_list<B_t>  &&b){
		return zip(a, b);
	}
	/**
	 * @short zip two lists into a list of tuples.
	 * 
	 * Specialization with the second as a initializer list.
	 */
	template<typename A, typename B_t>
	inline underscore<std::vector<std::tuple<typename A::value_type, B_t>>> zip(A &&a, std::initializer_list<B_t>  &&b){
		return zip(a, b);
	}
};
