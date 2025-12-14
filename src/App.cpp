#include <iostream>
#include "CompileApp.hpp"


int main(int argc,char** argv){
	CompileApp compileApp;
	if(argc==1){
		compileApp.run();
		return 0;
	}

	std::string inPath, outPath;
	for(int i=1;i<argc;i++){
		std::string arg = argv[i];
		if(arg=="-i" && i+1<argc){
			inPath = argv[++i];
		}else if(arg=="-o" && i+1<argc){
			outPath = argv[++i];
		}else{
			std::cerr << "Unknown or incomplete argument: " << arg << std::endl;
		}
	}

	if(inPath.empty() && outPath.empty()){
		compileApp.run();
	}else{
		compileApp.runWithFiles(inPath, outPath);
	}

	return 0;
}