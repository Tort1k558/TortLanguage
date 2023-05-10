#include "SymbolTable.h"

void SymbolTable::addVar(const std::string& name, llvm::Value* value)
{
	NodeTable node;
	node.name = name;
	node.value = value;
	node.type = value->getType();
	m_symbolTable.push_back(node);
}

llvm::Value* SymbolTable::getValueVar(const std::string& name)
{
	LLVMManager& manager = LLVMManager::getInstance();
	std::shared_ptr<llvm::IRBuilder<>> builder = manager.getBuilder();

	auto it = std::find_if(m_symbolTable.begin(), m_symbolTable.end(), [&name](const NodeTable& node)
		{
			return node.name == name;
		});
	if (it == m_symbolTable.end())
	{
		std::cerr << "ERROR Var is not defined!" << std::endl;
		return nullptr;
	}
	NodeTable node = *it;
	llvm::Type* varType;
	llvm::Value* var;
	if (node.type->isPointerTy())
	{
		varType = llvm::dyn_cast<llvm::AllocaInst>(node.value)->getAllocatedType();
		var = builder->CreateLoad(varType, node.value);
	}
	else
	{
		varType = node.type;
		var = node.value;
	}
	return var;
}

llvm::Value* SymbolTable::getPtrVar(const std::string& name)
{
	LLVMManager& manager = LLVMManager::getInstance();
	std::shared_ptr<llvm::IRBuilder<>> builder = manager.getBuilder();

	auto it = std::find_if(m_symbolTable.begin(), m_symbolTable.end(), [&name](const NodeTable& node)
		{
			return node.name == name;
		});
	if (it == m_symbolTable.end())
	{
		std::cerr << "ERROR Var is not defined!" << std::endl;
		return nullptr;
	}
	NodeTable node = *it;

	if (!node.type->isPointerTy())
	{
		std::cerr << "ERROR::The " << name << " is not variable!" << std::endl;
		return nullptr;
	}
	return node.value;
}

void SymbolTable::extend(SymbolTable* table)
{
	for (const auto& node : table->m_symbolTable)
	{
		m_symbolTable.emplace(m_symbolTable.begin(), node);
	}
}