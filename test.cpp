#include "underscore.hpp"
#include "ctest.h"
#include "generator.hpp"
#include "string.hpp"
#include "file.hpp"

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
	FAIL_IF_NOT_EQUAL_STRING(vv.map([](int v){ return v*3; }).join(), "3, 6, 9, 12, 15");
	FAIL_IF_NOT_EQUAL_STRING(vv.reverse().join(), "5, 4, 3, 2, 1");
	FAIL_IF_NOT_EQUAL_STRING(_({5,3,2,1,4}).sort().join(), vv.join());
	
	FAIL_IF_NOT_EQUAL_STRING(_({1,2,3,4,5,2,3,4,5,6,1,3,2}).unique().join(),"1, 2, 3, 4, 5, 6");
	FAIL_IF_NOT_EQUAL_STRING(_({1,2,3,4,5,2,3,4,5,6,1,3,2}).sort().unique(true).join(),"1, 2, 3, 4, 5, 6");
	
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
	
	auto zipped_event_il=zip({1,2,3,4},std::vector<char>{'a','b','c','d'});
	auto zipped_event_il_and_tuple_map=zipped_event_il
			.map<std::string, int, char>([](int a, char b){ return std::to_string(a)+"--"+char(b); })
			.join();
	FAIL_IF_NOT_EQUAL_STRING(zipped_event_il_and_tuple_map, "1--a, 2--b, 3--c, 4--d");
	
	auto zipped_from_std_initialization_lists_and_tuple_map=zip({1,2,3,4},{'a','b','c','d'})
 			.map<std::string, int, char>([](int a, char b){ return std::to_string(a)+"--"+char(b); })
 			.join();
 	FAIL_IF_NOT_EQUAL_STRING(zipped_from_std_initialization_lists_and_tuple_map, "1--a, 2--b, 3--c, 4--d");
	
	
	auto two_lists = zipped_event_il.unzip<int,char>();
	auto first = std::get<0>(two_lists);
	auto second = std::get<1>(two_lists);
	
	FAIL_IF_NOT_EQUAL_STRING(first.join(), "1, 2, 3, 4");
	FAIL_IF_NOT_EQUAL_STRING(second.join(), "a, b, c, d");
	
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

void t10_flatmap(){
	INIT_LOCAL();

	auto str=_({1,2,3,4,5})
		.flatMap<range<int>>([](int i){ 
			return make_range(0,i);
		})
		.join();
	
	FAIL_IF_NOT_EQUAL_STRING(str, "0, 0, 1, 0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 4");

	auto str2=_({"Hello","world"})
		.flatMap<std::string>([](const std::string &i){ 
			return i;
		})
		.join();
	
	FAIL_IF_NOT_EQUAL_STRING(str2, "H, e, l, l, o, w, o, r, l, d");

	
	END_LOCAL();
};

void g01_generator(){
	INIT_LOCAL();
	
	auto s=generator({"¡","Hola","Mundo","!"})
		.filter([](const std::string &str){
			return str.length()>2;
		})
		.map([](const std::string &str){
			return std::string("Test ")+str;
		});

	auto more=s.map([](const std::string &str){
		return str+"!";
	});
	
	s.filter([](const std::string &str){
		return false;
	});

	for(auto v: s)
		std::cout<<v<<std::endl;
	for(auto v: more)
		std::cout<<v<<std::endl;
	
	END_LOCAL();
}

void st01_strings(){
	INIT_LOCAL();
	
	auto a=_("Hello world");
	auto av=a.split(' ');
	
	FAIL_IF_NOT_EQUAL_STRING(av[0], "Hello");
	FAIL_IF_NOT_EQUAL_STRING(av[1], "world");
		
	av=_("   Hello    world   ").split(' ');
	FAIL_IF_NOT_EQUAL_STRING(av[0], "Hello");
	FAIL_IF_NOT_EQUAL_STRING(av[1], "world");

	av=_("   Hello    world").split(' ');
	FAIL_IF_NOT_EQUAL_STRING(av[0], "Hello");
	FAIL_IF_NOT_EQUAL_STRING(av[1], "world");

	av=_("Hello,world").split();
	FAIL_IF_NOT_EQUAL_STRING(av[0], "Hello");
	FAIL_IF_NOT_EQUAL_STRING(av[1], "world");

	av=_("Hello, world").split(", ");
	FAIL_IF_NOT_EQUAL_STRING(av[0], "Hello");
	FAIL_IF_NOT_EQUAL_STRING(av[1], "world");

	av=_("Hello  world").split(' ', true);
	FAIL_IF_NOT_EQUAL_INT(av.size(), 3);
	FAIL_IF_NOT_EQUAL_STRING(av[0], "Hello");
	FAIL_IF_NOT_EQUAL_STRING(av[1], "");
	FAIL_IF_NOT_EQUAL_STRING(av[2], "world");
	
	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").lower(), "hello, world");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").upper(), "HELLO, WORLD");
	
	FAIL_IF_NOT_EQUAL(_("Hello, world").startswith("Hello"), true);
	FAIL_IF_NOT_EQUAL(_("Hello, world").startswith("Hella"), false);
	FAIL_IF_NOT_EQUAL(_("Hello, world").startswith(""), true);
	FAIL_IF_NOT_EQUAL(_("Hello, world").endswith(""), true);
	FAIL_IF_NOT_EQUAL(_("Hello, world").endswith("ad"), false);
	FAIL_IF_NOT_EQUAL(_("Hello, world").endswith("world"), true);

	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").slice(0,-1), "Hello, world");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").slice(0,-6), "Hello, ");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").slice(-7,-6), ", ");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").slice(-5,-1), "world");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello, world").slice(-5,12), "world");

	END_LOCAL();
};

void st02_strings_underscore(){
	INIT_LOCAL();
	
	auto v=_("Hello, world").split(", ").map<int>([](const std::string &s){
		return s.size();
	});
	
	FAIL_IF_NOT_EQUAL_STRING(v.join(", "), "5, 5"); // Count words on the splitted list.
	
	END_LOCAL();
};

void st03_strings_to(){
	INIT_LOCAL();
	
	FAIL_IF_NOT_EQUAL(_("123").to_long(), 123);
	FAIL_IF_NOT_EQUAL(_("123.0").to_float(), 123.0);
	FAIL_IF_NOT_EQUAL(_("123").to_float(), 123);
	FAIL_IF_NOT_EQUAL(_("123.5").to_float(), 123.5);
	FAIL_IF_NOT_EQUAL(_("123.5").to_double(), 123.5);
	
	FAIL_IF_NOT_EXCEPTION( _("er").to_long() );
	FAIL_IF_NOT_EXCEPTION( _("123 er").to_long() );
	FAIL_IF_NOT_EXCEPTION( _("").to_long() );
	FAIL_IF_NOT_EXCEPTION( _("123.0 es").to_float() );
	FAIL_IF_NOT_EXCEPTION( _("es").to_float() );
	FAIL_IF_NOT_EXCEPTION( _("").to_float() );
	FAIL_IF_NOT_EXCEPTION( _("123.0 es").to_double() );
	FAIL_IF_NOT_EXCEPTION( _("es").to_double() );
	FAIL_IF_NOT_EXCEPTION( _("").to_double() );
	
	END_LOCAL();
};

void st04_strip(){
	INIT_LOCAL();
	
	FAIL_IF_NOT_EQUAL_STRING(_("  Hello").strip(), "Hello");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello").strip(), "Hello");
	FAIL_IF_NOT_EQUAL_STRING(_("  Hello  ").strip(), "Hello");
	FAIL_IF_NOT_EQUAL_STRING(_("Hello  ").strip(), "Hello");
	
	END_LOCAL();
}

void f01_istream(){
	INIT_LOCAL();
	auto first_5_services_sorted=file(std::ifstream("/etc/services"))
								.filter([](const std::string &s){ return !s.empty() && s[0]!='#'; })
								.map<std::string>([](const std::string &s){ return s.substr(0,s.find(" ")); })
								.sort()
								.head(5);
	FAIL_IF_NOT_EQUAL_INT(first_5_services_sorted.size(),5);
								
	END_LOCAL();
}

int main(int argc, char **argv){
	START();
	
	t01_create();
	t02_ops();
	t03_slice();
	t04_terminal_ops();
	t05_iterators();
	t06_zip();
	t08_range();
	t09_initialized_with_cstrings();
	t10_flatmap();
	
	g01_generator();
	
	st01_strings();
	st02_strings_underscore();
	st03_strings_to();
	st04_strip();

	f01_istream();
	
	END();
}
