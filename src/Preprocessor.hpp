#pragma once
#include <string>
#include "IOManager.hpp"

class Preprocessor{
private:
	std::string text;
public:
	void readTextFromIOManager(const IOManager &ioManager);
	void readTextFromFile(const IOManager &ioManager, const std::string &path);
	void doPreprocess();
	std::string getText()const;
};