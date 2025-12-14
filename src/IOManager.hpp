#pragma once
#include <iostream>
#include <string>

class IOManager{
private:
	// 1-readFromCmd
	// 2-readFromFile
	int inMode;
	// 1-outToCmd
	// 2-outToFile
	int outMode;

	std::string inFilePath;
	std::string outFilePath;

	std::string readFromCmd()const;
	std::string readFromFile() const;

	bool writeToCmd(const std::string &s)const;
	bool writeToFile(const std::string &s)const;

public:
	IOManager();
	std::string read()const;
	bool write(const std::string &s)const;
	void setInMode(int inMode);
	void setOutMode(int outMode);
	void setInFilePath(const std::string &inFilePath);
	void setOutFilePath(const std::string &outFilePath);
	int getInMode()const;
	int getOutMode()const;
	int checkFilePath(const std::string &filePath) const;
};