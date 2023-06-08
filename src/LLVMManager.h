#pragma once
#include <llvm/IR/IRBuilder.h>

class BlockAST;

class LLVMManager
{
public:
	static LLVMManager& getInstance()
	{
		static LLVMManager instance;
		return instance;
	}
	
	std::shared_ptr<llvm::LLVMContext> getContext()
	{
		return m_context;
	}
	std::shared_ptr<llvm::IRBuilder<>> getBuilder()
	{
		return m_builder;
	}
	std::shared_ptr<llvm::Module> getModule()
	{
		return m_module;
	}
	std::shared_ptr<BlockAST> getCurrentBlock()
	{
		return m_currentBlock;
	}
	void setModule(std::shared_ptr<llvm::Module> module)
	{
 		m_module = module;
	}
	void setCurrentBlock(std::shared_ptr<BlockAST> block)
	{
		m_currentBlock = block;
	}
	~LLVMManager()
	{
		m_module.reset();
		m_builder.reset();
		m_context.reset();
		m_currentBlock.reset();
	}
private:
	LLVMManager()
	: m_context(std::make_shared<llvm::LLVMContext>()),
	  m_builder(std::make_shared<llvm::IRBuilder<>>(*m_context)),
	  m_module(nullptr) {}
	LLVMManager(const LLVMManager&) = delete;
	LLVMManager operator=(const LLVMManager&) = delete;
	std::shared_ptr<llvm::LLVMContext> m_context;
	std::shared_ptr<llvm::Module> m_module;
	std::shared_ptr<llvm::IRBuilder<>> m_builder;
	std::shared_ptr<BlockAST> m_currentBlock;
};