#include "LexicalAnalyzerApp.hpp"
#include "windows.h"

void LexicalAnalyzerApp::start(){
	std::cout<<"请输入想要进行词法分析的文本(输入Ctrl+Z停止):\n";
	preprocessor.readTextFromIOManager(ioManager);
	lexer.setText(preprocessor.getText());
	lexer.doLexer();
	analysisResult.setTokens(lexer.getTokens());
	analysisResult.print();
	// system("pause");
}

void LexicalAnalyzerApp::run(){
	int option=1;
	while(1){
		manu();
		std::cin>>option;
		system("cls");
		if(option==1)start();
		if(option==2)break;
	}
	std::cout<<"再见\n";
}

void LexicalAnalyzerApp::manu(){
	std::cout<<"*********************\n";
	std::cout<<"1. 开始进行词法分析\n";
	std::cout<<"2. 退出\n";
	std::cout<<"*********************\n";
}