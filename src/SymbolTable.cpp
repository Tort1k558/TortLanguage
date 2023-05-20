#include "SymbolTable.h"

void SymbolTable::addVar(const std::string& name, llvm::Value* value)
{
	NodeTable node;
	node.name = name;
	node.value = value;
	node.type = value->getType();
	auto it = std::find_if(m_symbolTable.begin(), m_symbolTable.end(), [&name](const NodeTable& node)
		{
			return node.name == name;
		});
	if (it == m_symbolTable.end())
	{
		m_symbolTable.push_back(node);
	}
	else
	{
		it->name = node.name;
		it->value = node.value;
		it->type = node.type;
	}
}
void SymbolTable::addVarType(const std::string& name, llvm::Type* type)
{
	NodeTable node;
	node.name = name;
	node.type = type;
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
	if (it->type->isPointerTy())
	{
		varType = llvm::dyn_cast<llvm::AllocaInst>(it->value)->getAllocatedType();
		var = builder->CreateLoad(varType, it->value);
	}
	else
	{
		varType = it->type;
		var = it->value;
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
llvm::Type* SymbolTable::getTypeVar(const std::string& name)
{
	auto it = std::find_if(m_symbolTable.begin(), m_symbolTable.end(), [&name](const NodeTable& node)
		{
			return node.name == name;
		});
	if (it == m_symbolTable.end())
	{
		std::cerr << "ERROR Var is not defined!" << std::endl;
		return nullptr;
	}
	return it->type;
}
void SymbolTable::extend(SymbolTable* table)
{
	for (const auto& node : table->m_symbolTable)
	{
		m_symbolTable.emplace(m_symbolTable.begin(), node);
	}
}