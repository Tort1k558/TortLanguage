#pragma once
#include<map>
#include<string>
#include<llvm/IR/Value.h>
#include<llvm/IR/Instructions.h>
class Table
{
public:
	static std::map<std::string, llvm::AllocaInst*> symbolTable;
};