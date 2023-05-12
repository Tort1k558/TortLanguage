#pragma once
#include <llvm/IR/IRBuilder.h>
#include"SymbolTable.h"
class SymbolTableManager
{
public:
	static SymbolTableManager& getInstance()
	{
		static SymbolTableManager instance;
		return instance;
	}

	std::shared_ptr<SymbolTable> getSymbolTable()
	{
		return m_symbolTable;
	}
	void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable)
	{
		m_symbolTable = symbolTable;
	}
private:
	SymbolTableManager()
		: m_symbolTable(){}
	SymbolTableManager(const LLVMManager&) = delete;
	SymbolTableManager operator=(const LLVMManager&) = delete;
	std::shared_ptr<SymbolTable> m_symbolTable;
};