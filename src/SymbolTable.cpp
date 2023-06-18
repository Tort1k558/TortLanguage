#include "SymbolTable.h"
#include "AST.h"
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
		m_symbolTable.push_back(std::make_shared<NodeVar>(node));
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
		m_symbolTable.push_back(std::make_shared<NodeVarArray>(node));
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
		m_symbolTable.push_back(std::make_shared<NodeVar>(node));
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
		m_symbolTable.push_back(std::make_shared<NodeVarArray>(node));
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
		m_symbolTable.push_back(node);
	}
}

void SymbolTable::addFunction(const std::string& name, llvm::Function* func, std::vector<std::shared_ptr<VarDeclAST>> args)
{
	NodeFunction node;
	node.name = name;
	node.function = func;
	node.returnType = func->getType();
	node.args = args;
	std::vector<std::shared_ptr<ASTNode>> argsAST;
	for (const auto& arg : args)
	{
		argsAST.push_back(arg);
	}
	std::shared_ptr<NodeVar> findedNode = findFunction(name, argsAST);

	if (!findedNode)
	{
		m_symbolTable.push_back(std::make_shared<NodeFunction>(node));
		return;
	}

	std::shared_ptr<NodeFunction> nodeFunc = std::dynamic_pointer_cast<NodeFunction>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}
	nodeFunc->function = node.function;
	nodeFunc->returnType = node.returnType;
	nodeFunc->args = node.args;
}

void SymbolTable::addFunctionReturnType(const std::string& name, llvm::Type* returnType)
{
	NodeFunction node;
	node.name = name;
	node.function = nullptr;
	node.returnType = returnType;
	std::shared_ptr<NodeVar> findedNode = findNode(name);

	if (!findedNode)
	{
		m_symbolTable.push_back(std::make_shared<NodeFunction>(node));
		return;
	}
	std::shared_ptr<NodeFunction> nodeFunc = std::dynamic_pointer_cast<NodeFunction>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}
	nodeFunc->returnType = node.returnType;
}

llvm::Function* SymbolTable::getFunction(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
{
	std::shared_ptr<NodeFunction> findedNode = findFunction(name, args);
	if (!findedNode)
	{
		return nullptr;
	}
	std::shared_ptr<NodeFunction> nodeFunc = std::dynamic_pointer_cast<NodeFunction>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}
	if (!nodeFunc->function)
	{
		return nullptr;
	}
	return nodeFunc->function;
}

llvm::Type* SymbolTable::getFunctionReturnType(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
{
	std::shared_ptr<NodeFunction> findedNode = findFunction(name, args);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}
	std::shared_ptr<NodeFunction> nodeFunc = std::dynamic_pointer_cast<NodeFunction>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}

	return nodeFunc->returnType;
}

std::vector<std::shared_ptr<VarDeclAST>> SymbolTable::getFunctionArgs(const std::string& name)
{
	std::shared_ptr<NodeVar> findedNode = findNode(name);
	if (!findedNode)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not defined!");
	}
	std::shared_ptr<NodeFunction> nodeFunc = std::dynamic_pointer_cast<NodeFunction>(findedNode);
	if (!nodeFunc)
	{
		throw std::runtime_error("ERROR::Var " + name + " is not function!");
	}

	return nodeFunc->args;
}

std::shared_ptr<SymbolTable::NodeVar> SymbolTable::findNode(std::string name)
{
	for (const auto& node : m_symbolTable)
	{
		if (node->name == name)
		{
			return node;
		}
	}
	return nullptr;
}
std::shared_ptr<SymbolTable::NodeFunction> SymbolTable::findFunction(std::string name, std::vector<std::shared_ptr<ASTNode>> args)
{
	for (const auto& node : m_symbolTable)
	{
		if (node->name == name)
		{
			std::shared_ptr<NodeFunction> nodeFunc = std::dynamic_pointer_cast<NodeFunction>(node);
			if (nodeFunc)
			{
				bool isSimilar = true;
				for (size_t i = 0; i < nodeFunc->args.size(); i++)
				{
					if (nodeFunc->args[i]->llvmType != args[i]->llvmType)
					{
						isSimilar = false;
						break;
					}
				}
				if (isSimilar)
				{
					return nodeFunc;
				}
			}
		}
	}
	return nullptr;
}