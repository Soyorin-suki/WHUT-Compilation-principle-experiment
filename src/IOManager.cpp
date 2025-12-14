#include "IOManager.hpp"
#include <fstream>
#include <sstream>

std::string IOManager::readFromCmd()const{
    std::string line;
    std::string result;
    // Read all lines from stdin until EOF
    while (std::getline(std::cin, line)){
        result += line;
        result += '\n';
    }
    return result;
}

std::string IOManager::readFromFile(const std::string &path) const{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(!ifs) return std::string();
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}


std::string IOManager::readFromCmd()const{
	std::string text;
	char ch;
	while((ch=getchar())!=EOF){
		text+=ch;
	}
	return text;
}