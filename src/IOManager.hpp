#pragma once
#include <iostream>
#include <string>

class IOManager{
private:

public:
	std::string readFromCmd()const;
	std::string readFromFile(const std::string &path) const;
};