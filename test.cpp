#include "underscore.hpp"
#include "ctest.h"

#include <vector>
#include <iostream>
#include <fstream>

using namespace underscore;

void t01_create(){
	INIT_LOCAL();

	std::vector<int> v{1,2,3,4,5};
	const auto vv = _(std::move(v));
	
	END_LOCAL();
}

void t02_ops(){
	INIT_LOCAL();
	
	const auto vv = _({1,2,3,4,5});
	
	FAIL_IF_NOT_EQUAL_STRING(vv.join() , "1, 2, 3, 4, 5");
	FAIL_IF_NOT_EQUAL_STRING(vv.filter([](int v){ return v%2; }).join(), "1, 3, 5");
	FAIL_IF_NOT_EQUAL_STRING(vv.remove(5).join(), "1, 2, 3, 4");
	FAIL_IF_NOT_EQUAL_STRING(vv.map<int>([](int v){ return v*2; }).join(), "2, 4, 6, 8, 10");
	FAIL_IF_NOT_EQUAL_STRING(vv.reverse().join(), "5, 4, 3, 2, 1");
	FAIL_IF_NOT_EQUAL_STRING(_({5,3,2,1,4}).sort().join(), vv.join());
	FAIL_IF_NOT_EQUAL_STRING(
		vv.
			filter([](int v)           { return v!=3; }).
			map<std::string>([](int v) { return std::string("+")+std::to_string(v)+std::string("+"); }).
			reverse().
			join("][")
		,"+5+][+4+][+2+][+1+");
	
	END_LOCAL();
}

void t03_slice(){
	INIT_LOCAL();
	const auto vv = _({1,2,3,4,5});

	FAIL_IF_NOT_EQUAL_STRING(vv.head(2).join(),"1, 2");
	FAIL_IF_NOT_EQUAL_STRING(vv.head(-2).join(),"1, 2, 3");
	FAIL_IF_NOT_EQUAL_STRING(vv.tail(2).join(),"3, 4, 5");
	FAIL_IF_NOT_EQUAL_STRING(vv.tail(-2).join(),"4, 5");
	FAIL_IF_NOT_EQUAL_STRING(vv.slice(1,2).join(),"2");
	END_LOCAL();
}
void t04_terminal_ops(){
	INIT_LOCAL();
	
	FAIL_IF_NOT_EQUAL_INT(_({1,2,3,4,5,6}).reduce<int>([](int a, int b){ return a+b; }, 0), 21);
	FAIL_IF_NOT_EQUAL_INT(_({1,2,3,4,5,6}).max(), 6);
	FAIL_IF_NOT_EQUAL_INT(_({1,2,3,4,5,6}).min(), 1);

	END_LOCAL();
}

void t05_iterators(){
	INIT_LOCAL();
	const auto vv = _({1,2,3,4,5});

	int sum=0;
	for(auto &v: vv.sort()){
		sum+=v;
	}
	
	FAIL_IF_NOT_EQUAL_INT(sum, 15);
	
	END_LOCAL();
}

void t06_zip(){
	INIT_LOCAL();
	auto zipped_uneven_a_from_vector_and_standard_map=
		zip(std::vector<int>{1,2,3},std::vector<char>{'a','b','c','d'})
			.map<std::string>([](const std::tuple<int,char> &p){ return std::to_string(std::get<0>(p))+"--"+char(std::get<1>(p)); })
			.join();
	FAIL_IF_NOT_EQUAL_STRING(zipped_uneven_a_from_vector_and_standard_map, "1--a, 2--b, 3--c, 0--d");
	
	auto zipped_uneven_b_from_vector_and_standard_map=zip(std::vector<int>{1,2,3,4},std::vector<char>{'a','b','c'})
			.map<std::string>([](const std::tuple<int,char> &p){ 
				int a; char b;
				std::tie(a,b) = p;
				return std::to_string(a)+"--"+char(b); 
			})
			.join();
	// Need the +char(0) because if not the size is not correct (C++ strings, you know, \0 can be part of it)
	FAIL_IF_NOT_EQUAL_STRING(zipped_uneven_b_from_vector_and_standard_map, std::string("1--a, 2--b, 3--c, 4--")+char(0));
	
	auto zipped_event_il_and_tuple_map=zip({1,2,3,4},std::vector<char>{'a','b','c','d'})
			.map<std::string, int, char>([](int a, char b){ return std::to_string(a)+"--"+char(b); })
			.join();
	FAIL_IF_NOT_EQUAL_STRING(zipped_event_il_and_tuple_map, "1--a, 2--b, 3--c, 4--d");
	
	auto zipped_from_std_initialization_lists_and_tuple_map=zip({1,2,3,4},{'a','b','c','d'})
 			.map<std::string, int, char>([](int a, char b){ return std::to_string(a)+"--"+char(b); })
 			.join();
 	FAIL_IF_NOT_EQUAL_STRING(zipped_from_std_initialization_lists_and_tuple_map, "1--a, 2--b, 3--c, 4--d");
	
	END_LOCAL();
}

void t07_istream(){
	INIT_LOCAL();
	auto first_5_services_sorted=__(std::ifstream("/etc/services"))
								.filter([](const std::string &s){ return !s.empty() && s[0]!='#'; })
								.map<std::string>([](const std::string &s){ return s.substr(0,s.find(" ")); })
								.sort()
								.head(5);
	FAIL_IF_NOT_EQUAL_INT(first_5_services_sorted.size(),5);
								
	auto b=__("Hello, world").join("\n");
	FAIL_IF_NOT_EQUAL_STRING(b, "Hello\n world");
	
	END_LOCAL();
}

void t08_range(){
	INIT_LOCAL();
	
	std::vector<int> a{1,2,3,4,5};

//	auto r=range<std::vector<int>::iterator>(std::begin(a), std::end(a));
	auto a_=_(std::begin(a), std::end(a));
	FAIL_IF_NOT_EQUAL_STRING(a_.join(), "1, 2, 3, 4, 5");
	
	auto b_=_(1,6);
	FAIL_IF_NOT_EQUAL_STRING(b_.join(), "1, 2, 3, 4, 5");
	
	END_LOCAL();
}

void t09_initialized_with_cstrings(){
	INIT_LOCAL();
	
	FAIL_IF_NOT_EQUAL_STRING(
		_({"red","green","blue"}).join(),
		"red, green, blue"
	);
	
	END_LOCAL();
};

int main(int argc, char **argv){
	START();
	
	t01_create();
	t02_ops();
	t03_slice();
	t04_terminal_ops();
	t05_iterators();
	t06_zip();
	t07_istream();
	t08_range();
	t09_initialized_with_cstrings();
	
	END();
}
