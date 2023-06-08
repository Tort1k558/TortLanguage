#pragma once

#include<iostream>
#include<string>
#include<variant>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>
#include"LLVMManager.h"
class SymbolTable
{
	struct NodeVarTable
	{
		std::string name;
		llvm::Type* type;
		llvm::Value* varValue;
		llvm::Type* containedType;
	};
	struct NodeFuncTable
	{
		std::string name;
		llvm::Type* returnType;
		llvm::Function* function;
	};

public:
	SymbolTable()
		:m_symbolTable(std::vector<std::variant<NodeVarTable, NodeFuncTable>>()) {};
	void addVar(const std::string& name, llvm::Value* value, llvm::Type* containedType = nullptr);
	void addFunction(const std::string& name, llvm::Function* func);
	void addFunctionReturnType(const std::string& name, llvm::Type* returnType);
	void addVarType(const std::string& name, llvm::Type* type, llvm::Type* containedType = nullptr);
	llvm::Value* getValueVar(const std::string& name);
	llvm::Value* getPtrVar(const std::string& name);
	llvm::Type* getTypeVar(const std::string& name);
	llvm::Type* getContainedTypeVar(const std::string& name);
	llvm::Type* getFunctionReturnType(const std::string& name);
	void extend(SymbolTable* table);
private:
	std::vector<std::variant<NodeVarTable, NodeFuncTable>> m_symbolTable;
	std::variant<NodeVarTable, NodeFuncTable>* findNode(std::string name);
};