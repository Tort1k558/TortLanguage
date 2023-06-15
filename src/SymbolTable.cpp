#include "SymbolTable.h"

void SymbolTable::addVar(const std::string& name, llvm::Value* value)
{
	NodeVar node;
	node.name = name;
	node.varValue = value;
	node.type = llvm::dyn_cast<llvm::AllocaInst>(value)->getAllocatedType();

	std::shared_ptr<NodeVar> findedNode = findNode(name);
	
	//If the variable does not exist, we change the data in the table
	if (!findedNode)
	{
		m_symbolTable.insert({ name,std::make_shared<NodeVar>(node) });
		return;
	}

	findedNode->varValue = node.varValue;
	findedNode->type = node.type;
}
void SymbolTable::addVarArray(const std::string& name, llvm::Value* value, llvm::Type* containedType, int dimension,std::vector<llvm::Value*> sizesVLA)
{
	NodeVarArray node;
	node.name = name;
	node.varValue = value;
	node.type = llvm::dyn_cast<llvm::AllocaInst>(value)->getAllocatedType();
	node.containedType = containedType;
	node.sizesVLA = sizesVLA;
	node.dimension = dimension;
	std::shared_ptr<NodeVar> findedNode = findNode(name);

	//If the variable does not exist, we change the data in the table
	if (!findedNode)
	{
		m_symbolTable.insert({ name,std::make_shared<NodeVarArray>(node) });
		return;
	}
	std::shared_ptr<NodeVarArray> nodeArray = std::dynamic_pointer_cast<NodeVarArray>(findedNode);

	nodeArray->varValue = node.varValue;
	nodeArray->type = node.type;
	nodeArray->containedType = node.containedType;
	nodeArray->sizesVLA = node.sizesVLA;
	nodeArray->dimension = node.dimension;
}
void SymbolTable::addVarType(const std::string& name, llvm::Type* type)
{
	NodeVar node;
	node.name = name;
	node.type = type;
	std::shared_ptr<NodeVar> findedNode = findNode(name);

	if (!findedNode)
	{
		m_symbolTable.insert({ name,std::make_shared<NodeVar>(node) });
		return;
	}

	findedNode->type = node.type;
}
void SymbolTable::addVarArrayType(const std::string& name, llvm::Type* type, llvm::Type* containedType, int dimension)
{
	NodeVarArray node;
	node.name = name;
	node.type = type;
	node.containedType = containedType;
	node.dimension = dimension;

	std::shared_ptr<NodeVar> findedNode = findNode(name);

	//If the variable does not exist, we change the data in the table
	if (!findedNode)
	{
		m_symbolTable.insert({ name,std::make_shared<NodeVarArray>(node) });
		return;
	}

	std::shared_ptr<NodeVarArray> nodeArray = std::dynamic_pointer_cast<NodeVarArray>(findedNode);
	nodeArray->type = node.type;
	nodeArray->containedType = node.containedType;
	nodeArray->dimension = node.dimension;
}
llvm::Value* SymbolTable::getValueVar(const std::string& name)
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto builder = manager.getBuilder();

	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}

	llvm::Type* varType;
	llvm::Value* var;
	if (findedNode->type->isPointerTy() || std::dynamic_pointer_cast<NodeVarArray>(findedNode))
	{
		var = findedNode->varValue;
	}
	else
	{
		varType = llvm::dyn_cast<llvm::AllocaInst>(findedNode->varValue)->getAllocatedType();
		var = builder->CreateLoad(varType, findedNode->varValue);
	}
	return var;
}

llvm::Value* SymbolTable::getPtrVar(const std::string& name)
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto builder = manager.getBuilder();

	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}

	return findedNode->varValue;
}

llvm::Type* SymbolTable::getTypeVar(const std::string& name)
{
	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}
	return findedNode->type;
}

llvm::Type* SymbolTable::getContainedTypeVar(const std::string& name)
{
	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}
	std::shared_ptr<NodeVarArray> nodeArray = std::dynamic_pointer_cast<NodeVarArray>(findedNode);
	if (!nodeArray)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not pointer!");
	}
	return nodeArray->containedType;
}
std::vector<llvm::Value*> SymbolTable::getSizeArrayVLA(const std::string& name)
{
	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}
	std::shared_ptr<NodeVarArray> nodeArray = std::dynamic_pointer_cast<NodeVarArray>(findedNode);
	if (!nodeArray)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not pointer!");
	}

	return nodeArray->sizesVLA;
}
int SymbolTable::getDimensionArray(const std::string& name)
{
	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " +name+ " is not defined!");
	}
	std::shared_ptr<NodeVarArray> nodeArray = std::dynamic_pointer_cast<NodeVarArray>(findedNode);
	if (!nodeArray)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not pointer!");
	}

	return nodeArray->dimension;
}
void SymbolTable::extend(SymbolTable* table)
{
	for (const auto& node : table->m_symbolTable)
	{
		m_symbolTable.insert(node);
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
	std::shared_ptr<NodeVar> findedNode = findNode(name);

	if (!findedNode)
	{
		m_symbolTable.insert({ name,std::make_shared<NodeFuncTable>(node) });
		return;
	}
	std::shared_ptr<NodeFuncTable> nodeFunc = std::dynamic_pointer_cast<NodeFuncTable>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}
	nodeFunc->returnType = node.returnType;
}

llvm::Type* SymbolTable::getFunctionReturnType(const std::string& name)
{
	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}
	std::shared_ptr<NodeFuncTable> nodeFunc = std::dynamic_pointer_cast<NodeFuncTable>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}

	return nodeFunc->returnType;
}

std::shared_ptr<SymbolTable::NodeVar> SymbolTable::findNode(std::string name)
{
	auto node = m_symbolTable.find(name);
	if (node == m_symbolTable.end())
	{
		return nullptr;
	}
	return node->second;
}