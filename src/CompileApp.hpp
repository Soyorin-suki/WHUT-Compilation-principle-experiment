#pragma once
#include "IOManager.hpp"
#include "Lexer.hpp"
#include "Preprocessor.hpp"
#include "AnalysisResult.hpp"

class CompileApp{
	private:
	IOManager ioManager;
	Lexer lexer;
	Preprocessor preprocessor;
	AnalysisResult analysisResult;

	public:
	void manu();
	void start();
	void run();
	void runWithFiles(const std::string &inPath, const std::string &outPath);

};