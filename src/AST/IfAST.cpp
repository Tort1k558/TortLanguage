#include"IfAST.h"

#include"GotoAST.h"

void IfAST::doSemantic()
{
    m_ifExpr->doSemantic();
    m_ifBlock->doSemantic();
    for (const auto& block : m_elseIfs)
    {
        block.second->doSemantic();
    }
    if (m_elseBlock)
    {
        m_elseBlock->doSemantic();
    }
    llvmType = m_ifExpr->llvmType;
}

void IfAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    m_ifExpr->codegen();
    llvm::Value* condValue = m_ifExpr->getLValue();
    if (condValue->getType() != builder->getInt1Ty())
    {
        condValue = castType(condValue, builder->getInt1Ty());
    }

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* ifBB = llvm::BasicBlock::Create(*context, "ifblock", function);
    llvm::BasicBlock* elseifBBHelp = nullptr;
    llvm::BasicBlock* elseBB = nullptr;
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");
    if (!m_elseIfs.empty())
    {
        elseifBBHelp = llvm::BasicBlock::Create(*context, "elseifblockhelp");
        builder->CreateCondBr(condValue, ifBB, elseifBBHelp);
    }
    if (m_elseBlock)
    {
        elseBB = llvm::BasicBlock::Create(*context, "elseblock");
        if (m_elseIfs.empty())
        {
            builder->CreateCondBr(condValue, ifBB, elseBB);
        }
    }
    else if (!m_elseBlock && m_elseIfs.empty())
    {
        builder->CreateCondBr(condValue, ifBB, mergeBB);
    }

    //generate ifBlock
    builder->SetInsertPoint(ifBB);
    m_ifBlock->addStatement(std::make_shared<GotoAST>(mergeBB));
    m_ifBlock->codegen();
    ifBB = builder->GetInsertBlock();

    //generate elseIfBlocks
    if (!m_elseIfs.empty())
    {
        llvm::BasicBlock* nextElseifBBHelp = nullptr;
        for (size_t i = 0; i < m_elseIfs.size(); i++)
        {
            builder->SetInsertPoint(elseifBBHelp);
            function->insert(function->end(), elseifBBHelp);


            if (i == m_elseIfs.size() - 1)
            {
                if (m_elseBlock)
                {
                    nextElseifBBHelp = elseBB;
                }
                else
                {
                    nextElseifBBHelp = mergeBB;

                }
            }
            else
            {
                nextElseifBBHelp = llvm::BasicBlock::Create(*context, "elseifblockhelp");
            }

            llvm::BasicBlock* elseifBB = llvm::BasicBlock::Create(*context, "elseifblock");
            function->insert(function->end(), elseifBB);

            m_elseIfs[i].first->codegen();
            llvm::Value* elseifCondValue = m_elseIfs[i].first->getLValue();
            if (elseifCondValue->getType() != builder->getInt1Ty())
            {
                elseifCondValue = castType(elseifCondValue, builder->getInt1Ty());
            }
            builder->CreateCondBr(elseifCondValue, elseifBB, nextElseifBBHelp);
            elseifBBHelp = builder->GetInsertBlock();

            builder->SetInsertPoint(elseifBB);
            m_elseIfs[i].second->addStatement(std::make_shared<GotoAST>(mergeBB));
            m_elseIfs[i].second->codegen();
            elseifBB = builder->GetInsertBlock();

            elseifBBHelp = nextElseifBBHelp;
        }
    }


    //generate elseBlock
    if (m_elseBlock)
    {
        function->insert(function->end(), elseBB);
        builder->SetInsertPoint(elseBB);

        m_elseBlock->addStatement(std::make_shared<GotoAST>(mergeBB));
        m_elseBlock->codegen();
    }
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
    llvmValue = nullptr;
}

std::shared_ptr<BlockAST> IfAST::getIfBlock()
{
    return m_ifBlock;
}

std::shared_ptr<BlockAST> IfAST::getElseBlock()
{
    return m_elseBlock;
}

std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> IfAST::getElseIfs()
{
    return m_elseIfs;
}