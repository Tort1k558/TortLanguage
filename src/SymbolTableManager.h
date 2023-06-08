#pragma once
#include <llvm/IR/IRBuilder.h>
#include"SymbolTable.h"
class SymbolTableManager
{
public:
	static SymbolTableManager& getInstance();
	std::shared_ptr<SymbolTable> getSymbolTable();
	void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable);
private:
	SymbolTableManager()
		: m_symbolTable(){}
	SymbolTableManager(const LLVMManager&) = delete;
	SymbolTableManager operator=(const LLVMManager&) = delete;
	std::shared_ptr<SymbolTable> m_symbolTable;
};