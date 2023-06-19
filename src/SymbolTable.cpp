#include "SymbolTable.h"
#include "AST.h"


void SymbolTable::addNode(std::shared_ptr<ASTNode> node)
{
	std::shared_ptr<ASTNode> findedNode = getNode(node->getName());
	if (findedNode)
	{
		findedNode = node;
	}
	else
	{
		m_symbolTable.push_back(node);
	}
}

std::shared_ptr<ASTNode> SymbolTable::getNode(const std::string& name)
{
	for (const auto& node :m_symbolTable)
	{
		if (node->getName() == name)
		{
			return node;
		}
	}
	return nullptr;
}

void SymbolTable::extend(SymbolTable* table)
{
	for (const auto& node : table->m_symbolTable)
	{
		m_symbolTable.push_back(node);
	}
}
