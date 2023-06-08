#include "SymbolTableManager.h"
SymbolTableManager& SymbolTableManager::getInstance()
{
	static SymbolTableManager instance;
	return instance;
}
std::shared_ptr<SymbolTable> SymbolTableManager::getSymbolTable()
{
	return m_symbolTable;
}

void SymbolTableManager::setSymbolTable(std::shared_ptr<SymbolTable> symbolTable)
{
	m_symbolTable = symbolTable;
}