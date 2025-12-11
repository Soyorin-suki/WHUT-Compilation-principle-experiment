#pragma once
#include "IOManager.hpp"
#include "Lexer.hpp"
#include "Preprocessor.hpp"
#include "AnalysisResult.hpp"

class LexicalAnalyzerApp{
	private:
	IOManager ioManager;
	Lexer lexer;
	Preprocessor preprocessor;
	AnalysisResult analysisResult;

	public:
	void manu();
	void start();
	void run();

};