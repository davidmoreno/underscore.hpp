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
#include "streams.hpp"
#include "underscore.hpp"

namespace underscore{
	/**
	 * @short Gets all data from a istream, and set one line per element into a vector of strings.
	 */
	underscore<std::vector<std::string>> file(std::istream &&input){
		std::vector<std::string> data;
		std::string str;
		while (!input.eof()){
			std::getline(input, str);
			data.push_back(str);
		}
		return data;
	}
};
