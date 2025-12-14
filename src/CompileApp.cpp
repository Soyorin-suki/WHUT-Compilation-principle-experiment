#include "CompileApp.hpp"
#include "windows.h"

void CompileApp::start(){
	preprocessor.readTextFromIOManager(ioManager);
	preprocessor.doPreprocess();
	lexer.setText(preprocessor.getText());
	lexer.doLexer();
	analysisResult.setTokens(lexer.getTokens());
	std::string LexerResult=analysisResult.print();
	// std::cout << analysisResult.print();
	bool flg=false;
	ioManager.write("词法分析结果为：\n");
	flg=analysisResult.isSuccess();
	if(flg==false){
		ioManager.write("\n分析失败，存在未知Token\n");
	}else{
		ioManager.write("\n分析成功\n");
	}
	ioManager.write(LexerResult);



	// system("pause");
}



void CompileApp::run(){
	int option=1;
	while(1){
		manu();
		std::cin>>option;
		system("cls");
		if(option==1){
			ioManager.setInMode(1);
			std::cout<<"请输入想要进行词法分析的文本(输入Ctrl+Z停止):\n";
			start();
		}if(option==2){
			std::cout<<"请输入源文件路径：\n";
			std::string filePath;
			std::cin>>filePath;
			ioManager.setInMode(2);
			if(ioManager.checkFilePath(filePath)==0){
				std::cout<<"文件不存在或不可读\n";
				continue;
			};
			ioManager.setInFilePath(filePath);
			start();
		}if(option==3)break;
	}
	std::cout<<"再见\n";
}

void CompileApp::manu(){
	std::cout<<"*********************\n";
	std::cout<<"1. 从cmd中读取源文件\n";
	std::cout<<"2. 从文件中读取源文件\n";
	std::cout<<"3. 退出\n";
	std::cout<<"*********************\n";
}

void CompileApp::runWithFiles(const std::string &inPath, const std::string &outPath){
	if(!inPath.empty()){
		ioManager.setInMode(2);
		ioManager.setInFilePath(inPath);
	}else{
		ioManager.setInMode(1);
	}

	if(!outPath.empty()){
		ioManager.setOutMode(2);
		ioManager.setOutFilePath(outPath);
	}else{
		ioManager.setOutMode(1);
	}

	start();
}