#pragma once

#include<iostream>
#include<string>
#include<unordered_map>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>
#include"LLVMManager.h"

class VarDeclAST;
class ASTNode;

class SymbolTable
{
public:
	SymbolTable()
		: m_symbolTable({}) {};
	void addNode(std::shared_ptr<ASTNode> node);
	std::shared_ptr<ASTNode> getNode(const std::string& name);
	void extend(SymbolTable* table);
private:
	std::vector<std::shared_ptr<ASTNode>> m_symbolTable;
};