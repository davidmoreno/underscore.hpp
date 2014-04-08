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

#include "sequence.hpp"

namespace underscore{
	class string;
	
	template<typename I>
	class range;
	
	
	/**
	 * @short Creates an sequence container of the given value. It copies the data.
	 * 
	 * Example:
	 * 	const std::vector<int> v{1,2,3,4};
	 * 	auto a=_(v);
	 */
	template<typename T>
	inline sequence<T> _(const T &v){
		return sequence<T>(v);
	}
	/**
	 * @short Creates an sequence container of the given value. Perfect forwarding version (std::move)
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
	inline sequence<T> _(T &&v){
		return sequence<T>(std::forward<T>(v));
	}
	/**
	 * @short Creates an sequence container with a vector of the elements into the initializer list.
	 * 
	 * Allows creation directly as:
	 * 
	 * 	auto a=_({1,2,3,4})
	 */
	template<typename T>
	inline sequence<std::vector<T>> _(std::initializer_list<T> &&v){
		return sequence<std::vector<T>>(std::vector<T>(std::forward<std::initializer_list<T>>(v)));
	}
	
	/**
	 * @short Creates ansequence container from two ranges. Useful for subranges.
	 * 
	 * It needs the ability to copy iterators.
	 */
	template<typename I>
	inline sequence<range<I>> _(I &&begin, I &&end){
		return _(range<I>(begin, end));
	}
	
	/**
	 * @short Encapsulate a string.
	 */
	string _(std::string &&s);
	string _(const char *s);


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
	inline sequence<std::vector<std::tuple<typename A::value_type, typename B::value_type>>> zip(const A &a, const B &b){
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
	inline sequence<std::vector<std::tuple<A_t, typename B::value_type>>> zip(std::initializer_list<A_t> &&a, B &&b){
		return zip(a, b);
	}
	/**
	 * @short zip two lists into a list of tuples.
	 * 
	 * Specialization with the two as initializer list.
	 */
	template<typename A_t, typename B_t>
	inline sequence<std::vector<std::tuple<A_t, B_t>>> zip(std::initializer_list<A_t> &&a, std::initializer_list<B_t>  &&b){
		return zip(a, b);
	}
	/**
	 * @short zip two lists into a list of tuples.
	 * 
	 * Specialization with the second as a initializer list.
	 */
	template<typename A, typename B_t>
	inline sequence<std::vector<std::tuple<typename A::value_type, B_t>>> zip(A &&a, std::initializer_list<B_t>  &&b){
		return zip(a, b);
	}
};
