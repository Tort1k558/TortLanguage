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
	struct NodeVar
	{
		virtual ~NodeVar() {};
		std::string name;
		llvm::Type* type;
		llvm::Value* varValue;
	};
	struct NodeVarArray : public NodeVar
	{
		llvm::Type* containedType;
		std::vector<llvm::Value*> sizesVLA;
		std::vector<int> indexesVLA;
	};
	struct NodeFuncTable : public NodeVar
	{
		llvm::Type* returnType;
		llvm::Function* function;

	};

public:
	SymbolTable()
		: m_symbolTable({}) {};
	void addVar(const std::string& name, llvm::Value* value);
	void addVarArray(const std::string& name, llvm::Value* value, llvm::Type* containedType, std::vector<llvm::Value*> sizesVLA = {}, std::vector<int> indexesVLA = {});
	void addFunction(const std::string& name, llvm::Function* func);
	void addFunctionReturnType(const std::string& name, llvm::Type* returnType);
	void addVarType(const std::string& name, llvm::Type* type);
	void addVarArrayType(const std::string& name, llvm::Type* type, llvm::Type* containedType = nullptr);
	llvm::Value* getValueVar(const std::string& name);
	llvm::Value* getPtrVar(const std::string& name);
	llvm::Type* getTypeVar(const std::string& name);
	llvm::Type* getContainedTypeVar(const std::string& name);
	std::vector<llvm::Value*> getSizeArrayVLA(const std::string& name);
	std::vector<int> getIndexesVLA(const std::string& name);
	llvm::Type* getFunctionReturnType(const std::string& name);
	void extend(SymbolTable* table);
private:
	std::vector<std::shared_ptr<NodeVar>> m_symbolTable;
	std::shared_ptr<NodeVar> findNode(std::string name);
};