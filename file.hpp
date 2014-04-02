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
#include <fstream>
#include <string>
#include <memory>
#include "generator.hpp"

namespace underscore{
	class file : public generator<file>{
		std::unique_ptr<std::ifstream> ifs; // Workaround no &&ifstream in gcc 4.8 as in http://stackoverflow.com/questions/12015899/why-are-move-semantics-for-a-class-containing-a-stdstringstream-causing-compil
		std::string next; // To reuse lines.
	public:
		file(const std::string &filename) : ifs(new std::ifstream(filename, std::ifstream::in)) { 
		}
		
		bool empty(){
			return !ifs->is_open() || ifs->eof();
		}
		std::string get_next(){
			if (empty())
				throw ::underscore::eog();
			std::getline(*ifs, next);
			return next;
		};
	};
};
