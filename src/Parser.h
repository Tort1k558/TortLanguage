#pragma once
#include"TokenStream.h"
#include"Token.h"
#include<map>
#include<memory>
#include<llvm/IR/IRBuilder.h>
#include<llvm/IR/IntrinsicsXCore.h>
#include<llvm/IR/LLVMContext.h>
#include"AST.h"
class Parser
{
public:
	Parser(TokenStream *stream);
	void check(TokenType type);
	void parse();
private:
	TokenStream *m_src;
	TokenStream::TokenIterator m_currentPos;
	void parseFunction();
};

