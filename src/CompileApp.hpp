#pragma once
#include "IOManager.hpp"
#include "Lexer.hpp"
#include "Preprocessor.hpp"
#include "AnalysisResult.hpp"
#include "Parser.hpp"
#include "SemanticAnalyzer.hpp"
#include "TACGenerator.hpp"


class CompileApp{
	private:
	IOManager ioManager;
	Lexer lexer;
	Preprocessor preprocessor;
	AnalysisResult analysisResult;
	Parser parser;
	SemanticAnalyzer semanticAnalyzer;
	TACGenerator tacGenerator;

	public:
	void manu();
	void start();
	void run();
	void runWithFiles(const std::string &inPath, const std::string &outPath);

};