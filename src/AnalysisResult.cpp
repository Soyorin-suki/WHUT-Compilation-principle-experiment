#include "AnalysisResult.hpp"


void AnalysisResult::setTokens(const std::vector<Token>tokens){
	this->tokens=tokens;
}

void AnalysisResult::print(){
	for(int i=0;i<tokens.size();++i){
		std::cout<<tokens[i]<<std::endl;
	}
}