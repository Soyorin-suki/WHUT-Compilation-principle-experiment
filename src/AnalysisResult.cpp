#include "AnalysisResult.hpp"
#include <sstream>

void AnalysisResult::setTokens(const std::vector<Token>tokens){
	this->tokens=tokens;
}

std::string AnalysisResult::print(){
	std::stringstream ss;
	for(int i=0;i<tokens.size();++i){
		ss<<tokens[i]<<std::endl;
	}
	return ss.str();
}

int AnalysisResult::isSuccess()const{
	for(int i=0;i<tokens.size();++i){
		if(tokens[i].type==TokenType::Unknown)return false;
	}
	return true;
}