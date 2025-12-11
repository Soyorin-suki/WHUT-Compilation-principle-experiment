#pragma once
#include <vector>
#include "Token.hpp"

class AnalysisResult{
private:
	std::vector<Token>tokens;
public:
	void setTokens(const std::vector<Token>tokens);
	void print();
};