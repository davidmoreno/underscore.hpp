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
#include <iostream>
#include <memory>

namespace underscore{
// 	template<typename T>
	class basic_stream{
	public:
		typedef std::string value_type;
		typedef std::function<bool (const value_type &)> filter_f;
		typedef std::function<value_type (const value_type &)> map_f;
		
		class plan_item{
		public:
			std::shared_ptr<plan_item> next;
			
			virtual ~plan_item() {};
			
			plan_item &push_back(std::shared_ptr<plan_item> next_plan){
				if (!next){
					next=next_plan;
					return *this;
				}
				else
					return next->push_back(next_plan);
			}
			
			virtual std::shared_ptr<plan_item> copy() = 0;
			virtual void operator()(const basic_stream::value_type &v) = 0;
		};
		
		class plan_filter : public plan_item{
			basic_stream::filter_f f;
		public:
			plan_filter(basic_stream::filter_f _f) : f(_f){}
			
			void operator()(const basic_stream::value_type &v){
				bool do_next=f(v);
				if (do_next && next)
				(*next)(v);
			}
			virtual std::shared_ptr<plan_item> copy() {
				auto ret=std::make_shared<plan_filter>(f);
				if (next)
					ret->push_back(next->copy());
				return ret;
			};
		};
		class plan_map : public plan_item{
			basic_stream::map_f f;
		public:
			plan_map(basic_stream::map_f _f) : f(_f){}
			
			void operator()(const basic_stream::value_type &v){
				auto nv=f(v);
				if (next)
					(*next)(nv);
				else{
					std::cerr<<"What for? nothing farther"<<std::endl;
					throw(std::exception());
				}
			};
			virtual std::shared_ptr<plan_item> copy() {
				auto ret=std::make_shared<plan_map>(f);
				if (next)
					ret->push_back(next->copy());
				return ret;
			};
		};
	public:
		std::shared_ptr<plan_item> plan;
		
		basic_stream(){};
		virtual basic_stream &operator=(basic_stream &o){
			plan=o.plan->copy();
			return *this;
		}
		
		basic_stream &filter(const filter_f &f){
			std::shared_ptr<plan_item> next_plan=std::make_shared<plan_filter>(f);
			return push_back( next_plan );
		}
		basic_stream &map(const map_f &f){
			std::shared_ptr<plan_item> next_plan=std::make_shared<plan_map>(f);
			return push_back( next_plan );
		}
		
		basic_stream &push_back(std::shared_ptr<plan_item> next_plan){
			if (plan)
				plan->push_back(next_plan);
			else
				plan=next_plan;
			return *this;
		}
		
		void do_plan(const value_type &v){
			if (plan)
				(*plan)(v);
		}
	};
	
	class stream : public basic_stream{
	public:
		class iterator{
			value_type _val;
			std::shared_ptr<plan_item> _plan;
			std::vector<value_type>::iterator _current;
			std::vector<value_type>::iterator _end;
			bool _computed;
			
			void compute_value(){
				_computed=false;
				while(_current!=_end){
					(*_plan)(*_current);
					if (_computed)
						return;
					++_current;
				}
			};
		public:
			iterator(const std::shared_ptr<plan_item> &plan, std::vector<value_type>::iterator &&current, std::vector<value_type>::iterator &&end) : 
					_plan(plan->copy()), _current(current), _end(end), _computed(false)
			{ 
				_plan->push_back(std::make_shared<plan_filter>([this](const value_type &v) -> bool{
					this->_computed=true;
					this->_val=v;
					return false;
				}));
				compute_value(); 
				
			}
			
			value_type operator*(){ return _val; }
			iterator &operator++(){ ++_current; compute_value(); return *this; }
			bool operator!=(const iterator &o){
				return _current!=o._current;
			};
		};
	private:
		std::vector<value_type> _data;
		
		stream push_back(std::shared_ptr<plan_item> item) const {
			stream ret(_data);
			if (plan){
				ret.plan=plan->copy();
				ret.plan->push_back( item );
			}
			else{
				ret.plan=item;
			}
			return ret;
		}
	public:
		stream() = delete;
		stream(const std::initializer_list<value_type> &data) : _data(data){};
		stream(const std::vector<value_type> &data) : _data(data){};

		virtual basic_stream &operator=(stream &o){
			plan=o.plan->copy();
			return *this;
		}

		iterator begin(){ return stream::iterator(plan, _data.begin(), _data.end()); }
		iterator end(){ return stream::iterator(plan, _data.end(), _data.end()); }
		
		stream filter(const filter_f &f) const{
			return push_back(std::make_shared<plan_filter>(f));
		};
		stream map(const map_f &f) const{
			return push_back(std::make_shared<plan_map>(f));
		};
		
		// Finally performs to convert to vector
		std::vector<value_type> to_vector(){
			std::vector<value_type> ret;
			filter( [&ret](const value_type &v) -> bool{
				ret.push_back(v);
				return false;
			} );
			for (const auto &v: _data)
				do_plan(v);
			return ret;
		}
	};
};

