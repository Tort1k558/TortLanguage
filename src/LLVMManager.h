#pragma once
#include <llvm/IR/IRBuilder.h>

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
private:
	LLVMManager()
		: m_context(std::make_shared<llvm::LLVMContext>()),
		  m_module(std::make_shared<llvm::Module>("Tort", *m_context)),
		  m_builder(std::make_shared<llvm::IRBuilder<>>(*m_context)){}
	LLVMManager(const LLVMManager&) = delete;
	LLVMManager operator=(const LLVMManager&) = delete;
	std::shared_ptr<llvm::LLVMContext> m_context;
	std::shared_ptr<llvm::Module> m_module;
	std::shared_ptr<llvm::IRBuilder<>> m_builder;
};