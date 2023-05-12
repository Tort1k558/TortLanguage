#pragma once

#include<iostream>
#include<map>
#include<string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>
#include"LLVMManager.h"
class SymbolTable
{
	struct NodeTable
	{
		std::string name;
		llvm::Value* value;
		llvm::Type* type;
	};

public:
	SymbolTable()
		:m_symbolTable(std::vector<NodeTable>()) {};
	void addVar(const std::string& name, llvm::Value* value);
	void addVarType(const std::string& name, llvm::Type* type);
	llvm::Value* getValueVar(const std::string& name);
	llvm::Value* getPtrVar(const std::string& name);
	llvm::Type* getTypeVar(const std::string& name);
	void extend(SymbolTable* table);
private:
	std::vector<NodeTable> m_symbolTable;
};