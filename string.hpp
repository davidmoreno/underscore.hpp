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
#include <string>
#include <vector>
#include <boost/concept_check.hpp>
#include "underscore.hpp"

namespace underscore{
	class string;
	
	typedef ::underscore::underscore<std::vector<string>> string_list;
	typedef std::vector<string> std_string_list;
	
	class string{
		std::string _str;
		
	public:
		
		string(std::string &&str) : _str(std::move(str)){};
		string(const std::string &str) : _str(str){};
		string(const char *str) : _str(str){};
		template<typename T> string(const T &v) : _str(std::to_string(v)){};
		string() : _str(){};

		string_list split(const char &sep=',', bool insert_empty_elements=false) const {
			std_string_list v;
			std::string el;
			
			std::string::const_iterator I=_str.begin(), endI=_str.end();
			std::string::const_iterator p;
			do{
				p=std::find(I, endI, sep);
				
				el=std::string(I, p);
				if (insert_empty_elements || !el.empty())
					v.push_back(std::move(el));
				I=p+1;
			}while(p!=endI);
			
			return string_list(std::move(v));
		}
		
		string_list split(const std::string &sep, bool insert_empty_elements=false) const {
			std_string_list v;
			std::string el;
			
			auto I=_str.begin(), endI=_str.end(), sepBegin=sep.begin(), sepEnd=sep.end();
			std::string::const_iterator p;
			
			do{
				p=std::search(I, endI, sepBegin, sepEnd);
				
				el=std::string(I, p);
				if (insert_empty_elements || !el.empty())
					v.push_back(el);
				I=p+sep.size();
			}while(p!=endI);
			
			return string_list(std::move(v));
		}
		
		string lower() const{
			std::string ret;
			ret.reserve(_str.size());
			for (auto c: _str)
				ret.push_back(::tolower(c));
			return string(std::move(ret));
		};
		string upper() const{
			std::string ret;
			ret.reserve(_str.size());
			for (auto c: _str)
				ret.push_back(::toupper(c));
			return string(std::move(ret));
		};
		
		bool startswith(const std::string &starting) const {
			if (_str.size()<starting.size())
				return false;
			return (_str.substr(0,starting.size())==starting);
		}
		
		bool endswith(const std::string &ending) const {
			if (_str.size()<ending.size())
				return false;
			return (_str.substr(_str.size()-ending.size())==ending);
		}
		bool contains(const std::string &substr) const {
			return _str.find(substr)!=std::string::npos;
		}
		
		string replace(const std::string &orig, const std::string &replace_with){
			std::string ret=_str;
			size_t pos = 0;
			size_t orig_length = orig.length();
			size_t replace_with_length = replace_with.length();
			while((pos = ret.find(orig, pos)) != std::string::npos)
			{
				ret.replace(pos, orig_length, replace_with);
				pos += replace_with_length;
			}
			return ret;
		}
		
		/**
		 * @short Removes all spaces, new lines and tabs from begining and end.
		 */
		string strip() const{
			auto begin=_str.find_first_not_of(" \n\t");
			auto end=_str.find_last_not_of(" \n\t");
			return slice(begin, end+1);
		}
		
		operator std::string() const{
			return _str;
		}
		
		size_t size() const { 
			return _str.size();
		}
		
		size_t length() const{
			return size();
		}
		
		bool empty() const{
			return _str.empty();
		}
		
		string slice(ssize_t start, ssize_t end) const{
			ssize_t s=size();
			if (end<0)
				end=s+end+1;
			if (end<0)
				return std::string();
			if (end-s>0)
				end=s;
			if (start<0)
				start=s+start;
			if (start<0)
				start=0;
			if (start>s)
				return std::string();
			if (start==0 && end==s)
				return *this;
			return _str.substr(start, end-start);
		}
		
		string format(std::initializer_list<string> &&str_list){
			auto ulist=string_list(str_list);
// 			std::cout<<"as ulist "<<ulist.join(", ")<<std::endl;
			return format(ulist);
		}
		
		class invalid_format : public std::exception{
		public:
			const char *what() const throw(){ return "Invalid string format"; };
		};
		class invalid_format_require_more_arguments : public invalid_format{
		public:
			const char *what() const throw(){ return "Invalid string format, requires more arguments."; };
		};
		class invalid_format_too_many_arguments : public invalid_format{
		public:
			const char *what() const throw(){ return "Invalid string format, it has too many arguments."; };
		};
		
		string format(const string_list &sl){
			std::string ret;
			auto res=size() + sl.reduce<size_t>([](const string &str, size_t s){ return s+str.size(); }, 0);
// 			std::cout<<"reserve "<<res<<std::endl;
			ret.reserve(res);
			
			signed long n=0;
			signed long sl_size=sl.size();
			for (auto &part: split("{}", true)){
				ret+=part;
				if (n<sl_size){
					ret+=sl[n];
				}
				else if (n>sl_size){
					throw invalid_format_require_more_arguments();
				}
				// else is just enought aguments, so ok.
				n++;
// 				std::cout<<"format "<<ret<<std::endl;
			}
			if (n<=sl_size){
				throw invalid_format_too_many_arguments();
			}
			return ret;
		}
		
		long to_long() const {
			size_t n;
			long l=stol(_str, &n);
			if (n!=_str.size())
				throw std::invalid_argument(_str);
			return l;
		}
		double to_double() const {
			size_t n;
			double f=stod(_str, &n);
			if (n!=_str.size())
				throw std::invalid_argument(_str);
			return f;
		}
		float to_float() const {
			size_t n;
			float f=stof(_str, &n);
			if (n!=_str.size())
				throw std::invalid_argument(_str);
			return f;
		}
		
		friend std::ostream& operator <<(std::ostream &output, const string &str) {
			output << "" << str._str;
			return output;
		}

	};
	
	inline string _(std::string &&s){
		return string(std::move(s));
	}
	inline string _(const char *s){
		return string(std::string(s));
	}

};