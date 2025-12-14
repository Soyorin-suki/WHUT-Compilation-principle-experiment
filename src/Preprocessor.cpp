#include "Preprocessor.hpp"

void Preprocessor::readTextFromIOManager(const IOManager&ioManager){
	text=ioManager.read();
};



void Preprocessor::doPreprocess(){
	std::string newText;
	for(int i=0;i<text.length();i++){
		if(text[i]=='\t')text[i]=' ';
		if(text[i]=='\r')continue;
		// else if(text[i]=='\n')text[i]=' ';
		// if(text[i]==' '){
		// 	if(i==0||text[i-1]==' '){
		// 		continue;
		// 	}
		// }
		newText+=text[i];
	}
	if(!newText.empty()&&newText.back()==' ')newText.pop_back();
	text=newText;
}

std::string Preprocessor::getText()const{
	return text;
}