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

namespace underscore{
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
}
