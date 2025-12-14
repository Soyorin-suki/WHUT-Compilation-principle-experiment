#pragma once
#include <vector>
#include "Token.hpp"
#include <string>

class AnalysisResult{
private:
	std::vector<Token>tokens;
public:
	void setTokens(const std::vector<Token>tokens);
	std::string print();
	int isSuccess()const;
};