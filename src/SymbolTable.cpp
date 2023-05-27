#include "SymbolTable.h"

void SymbolTable::addVar(const std::string& name, llvm::Value* value)
{
	NodeVarTable node;
	node.name = name;
	node.varValue = value;
	node.type = value->getType();
	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);
	
	if (!variant)
	{
		m_symbolTable.push_back(node);
		return;
	}
	
	if (std::holds_alternative<NodeVarTable>(*variant)) {
		NodeVarTable& findNode = std::get<NodeVarTable>(*variant);
		findNode.varValue = node.varValue;
		findNode.type = node.type;
	}
}
void SymbolTable::addVarType(const std::string& name, llvm::Type* type)
{
	NodeVarTable node;
	node.name = name;
	node.type = type;
	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);

	if (!variant)
	{
		m_symbolTable.push_back(node);
		return;
	}

	if (std::holds_alternative<NodeVarTable>(*variant)) {
		NodeVarTable& findNode = std::get<NodeVarTable>(*variant);
		findNode.type = node.type;
	}
}
llvm::Value* SymbolTable::getValueVar(const std::string& name)
{
	LLVMManager& manager = LLVMManager::getInstance();
	std::shared_ptr<llvm::IRBuilder<>> builder = manager.getBuilder();

	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);
	NodeVarTable& node = std::get<NodeVarTable>(*variant);

	llvm::Type* varType;
	llvm::Value* var;
	if (node.type->isPointerTy())
	{
		varType = llvm::dyn_cast<llvm::AllocaInst>(node.varValue)->getAllocatedType();
		var = builder->CreateLoad(varType, node.varValue);
	}
	else
	{
		varType = node.type;
		var = node.varValue;
	}
	return var;
}

llvm::Value* SymbolTable::getPtrVar(const std::string& name)
{
	LLVMManager& manager = LLVMManager::getInstance();
	std::shared_ptr<llvm::IRBuilder<>> builder = manager.getBuilder();

	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);
	if (!variant)
	{
		throw std::runtime_error("ERROR::Var is not defined!");
	}

	NodeVarTable& node = std::get<NodeVarTable>(*variant);
	if (!node.type->isPointerTy())
	{
		throw std::runtime_error("ERROR::The " + name + " is not variable!");
	}
	return node.varValue;
}
llvm::Type* SymbolTable::getTypeVar(const std::string& name)
{
	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);
	if (!variant)
	{
		throw std::runtime_error("ERROR::Var is not defined!");
	}
	NodeVarTable& node = std::get<NodeVarTable>(*variant);
	return node.type;
}
void SymbolTable::extend(SymbolTable* table)
{
	for (const auto& node : table->m_symbolTable)
	{
		m_symbolTable.emplace(m_symbolTable.begin(), node);
	}
}

void SymbolTable::addFunction(const std::string& name, llvm::Function* func)
{

}

void SymbolTable::addFunctionReturnType(const std::string& name, llvm::Type* returnType)
{
	NodeFuncTable node;
	node.name = name;
	node.returnType = returnType;
	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);

	if (!variant)
	{
		m_symbolTable.push_back(node);
		return;
	}

	if (std::holds_alternative<NodeFuncTable>(*variant)) {
		NodeFuncTable& findNode = std::get<NodeFuncTable>(*variant);
		findNode.returnType = node.returnType;
	}
}

llvm::Type* SymbolTable::getFunctionReturnType(const std::string& name)
{
	std::variant<NodeVarTable, NodeFuncTable>* variant = findNode(name);
	if (!variant)
	{
		throw std::runtime_error("ERROR::Function is not defined!");
	}
	if (std::holds_alternative<NodeFuncTable>(*variant)) {
		return std::get<NodeFuncTable>(*variant).returnType;
	}
	return nullptr;
}

std::variant<SymbolTable::NodeVarTable, SymbolTable::NodeFuncTable>* SymbolTable::findNode(std::string name)
{
	auto it = std::find_if(m_symbolTable.begin(), m_symbolTable.end(), [&name](std::variant<NodeVarTable, NodeFuncTable>& node)
		{
			if (std::holds_alternative<NodeVarTable>(node)) {
				return std::get<NodeVarTable>(node).name == name;
			}
			else if (std::holds_alternative<NodeFuncTable>(node)) {
				return std::get<NodeFuncTable>(node).name == name;
			}
			return false;
		});
	if (it == m_symbolTable.end())
	{
		return nullptr;
	}
	return &(*it);
}