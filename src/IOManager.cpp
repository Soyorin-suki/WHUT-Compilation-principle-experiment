#include "IOManager.hpp"


std::string IOManager::readFromCmd()const{
	std::string text;
	char ch;
	while((ch=getchar())!=EOF){
		text+=ch;
	}
	return text;
}