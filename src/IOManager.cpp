#include "IOManager.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>


IOManager::IOManager(){
    inMode=1;
    outMode=1;
    
}

std::string IOManager::readFromFile() const{
    std::ifstream ifs(inFilePath, std::ios::in | std::ios::binary);
    if(!ifs) return std::string();
    std::ostringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    return ss.str();
}


std::string IOManager::readFromCmd()const{
	std::string text;
	char ch;
	while((ch=getchar())!=EOF){
		text+=ch;
	}
	return text;
}

std::string IOManager::read()const{
    if(inMode==1){
        return readFromCmd();
    }else if(inMode==2){
        return readFromFile();
    }
    return "";
}






bool IOManager::writeToCmd(const std::string &s)const{
    std::cout<<s;
    return true;
}


bool IOManager::writeToFile(const std::string &s) const {
    if(outFilePath.empty())return false;
    std::filesystem::path p(outFilePath);
    if(p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
    std::ofstream ofs(outFilePath, std::ios::out | std::ios::binary | std::ios::app);
    if(!ofs) return false;
    ofs << s;
    return ofs.good();
}



bool IOManager::write(const std::string &s)const{
    if(outMode==1){
        return writeToCmd(s);
    }else if(outMode==2){
        return writeToFile(s);
    }
    return false;
}


void IOManager::setInMode(int inMode){
    this->inMode=inMode;
}
void IOManager::setOutMode(int outMode){
    this->outMode=outMode;
}
void IOManager::setInFilePath(const std::string &inFilePath){
    this->inFilePath=inFilePath;
}
void IOManager::setOutFilePath(const std::string &outFilePath){
    this->outFilePath=outFilePath;
}
int IOManager::getInMode()const{
    return inMode;
}
int IOManager::getOutMode()const{
    return outMode;
}

int IOManager::checkFilePath(const std::string &filePath) const{
    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
    if(ifs) {
        return 1;  // 文件存在且可读
    }
    return 0;  // 文件不存在或无读权限
}
