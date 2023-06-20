#include"BlockAST.h"

#include"IfAST.h"
#include"WhileAST.h"

void BlockAST::doSemantic()
{
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<ReturnAST> retAST = std::dynamic_pointer_cast<ReturnAST>(stmt);
        if (retAST)
        {
            continue;
        }
        stmt->doSemantic();
    }
}

void BlockAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();

    for (size_t i = 0; i < m_statements.size(); i++) {
        if (std::dynamic_pointer_cast<BlockAST>(m_statements[i]))
        {
            manager.setCurrentBlock(std::dynamic_pointer_cast<BlockAST>(m_statements[i]));
        }
        m_statements[i]->codegen();

        //No need to generate code after return,break,continue
        if (std::dynamic_pointer_cast<ReturnAST>(m_statements[i]) || std::dynamic_pointer_cast<BreakAST>(m_statements[i]) || std::dynamic_pointer_cast<ContinueAST>(m_statements[i]))
        {
            break;
        }
    }
    llvm::BasicBlock* lastBB = builder->GetInsertBlock();
    if (lastBB->empty())
    {
        lastBB->removeFromParent();
    }
}

void BlockAST::addStatement(std::shared_ptr<ASTNode> stmt) 
{
    m_statements.push_back(std::move(stmt));
}

void BlockAST::addStatementBeforeReturn(std::shared_ptr<ASTNode> stmt) 
{
    std::vector<std::shared_ptr<ReturnAST>> returns = getReturns();
    std::vector<std::shared_ptr<BreakAST>> breaks = getBreaks();
    std::vector<std::shared_ptr<ContinueAST>> continuations = getContinuations();

    for (const auto& ret : returns)
    {
        ret->addStatementBeforeCompleting(stmt);
    }
    for (const auto& brk : breaks)
    {
        brk->addStatementBeforeCompleting(stmt);
    }
    for (const auto& cont : continuations)
    {
        cont->addStatementBeforeCompleting(stmt);
    }
    for (const auto& stmtBlock : m_statements)
    {
        if (std::dynamic_pointer_cast<ReturnAST>(stmtBlock))
        {
            return;
        }
    }
    m_statements.push_back(stmt);
}

void BlockAST::extendSymbolTable(std::shared_ptr<SymbolTable> symbolTable)
{
    m_symbolTable->extend(symbolTable.get());
}

std::vector<std::shared_ptr<ReturnAST>> BlockAST::getReturns()
{
    std::vector<std::shared_ptr<ReturnAST>> returns;
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<ReturnAST> returnAST = std::dynamic_pointer_cast<ReturnAST>(stmt);
        if (returnAST)
        {
            returns.push_back(returnAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST)
        {
            for (const auto& ret : blockAST->getReturns())
            {
                returns.push_back(ret);
            }
        }

        std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST)
        {
            auto ifBlockAST = ifAST->getIfBlock();
            auto elseIfsBlockAST = ifAST->getElseIfs();
            auto elseBlockAST = ifAST->getElseBlock();
            for (const auto& ret : ifBlockAST->getReturns())
            {
                returns.push_back(ret);
            }
            if (!elseIfsBlockAST.empty())
            {
                for (size_t i = 0; i < elseIfsBlockAST.size(); i++)
                {
                    for (const auto& ret : elseIfsBlockAST[i].second->getReturns())
                    {
                        returns.push_back(ret);
                    }
                }
            }
            if (elseBlockAST)
            {
                for (const auto& ret : elseBlockAST->getReturns())
                {
                    returns.push_back(ret);
                }
            }
        }

        std::shared_ptr<WhileAST> whileAST = std::dynamic_pointer_cast<WhileAST>(stmt);
        if (whileAST)
        {
            for (const auto& ret : whileAST->getWhileBlock()->getReturns())
            {
                returns.push_back(ret);
            }
        }
    }
    return returns;
}

std::vector<std::shared_ptr<BreakAST>> BlockAST::getBreaks()
{
    std::vector<std::shared_ptr<BreakAST>> breaks;
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<BreakAST> breakAST = std::dynamic_pointer_cast<BreakAST>(stmt);
        if (breakAST)
        {
            breaks.push_back(breakAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST)
        {
            for (const auto& brk : blockAST->getBreaks())
            {
                breaks.push_back(brk);
            }
        }

        std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST)
        {
            auto ifBlockAST = ifAST->getIfBlock();
            auto elseIfsBlockAST = ifAST->getElseIfs();
            auto elseBlockAST = ifAST->getElseBlock();
            for (const auto& brk : ifBlockAST->getBreaks())
            {
                breaks.push_back(brk);
            }
            if (!elseIfsBlockAST.empty())
            {
                for (size_t i = 0; i < elseIfsBlockAST.size(); i++)
                {
                    for (const auto& brk : elseIfsBlockAST[i].second->getBreaks())
                    {
                        breaks.push_back(brk);
                    }
                }
            }
            if (elseBlockAST)
            {
                for (const auto& brk : elseBlockAST->getBreaks())
                {
                    breaks.push_back(brk);
                }
            }
        }
    }
    return breaks;
}

std::vector<std::shared_ptr<ContinueAST>> BlockAST::getContinuations()
{
    std::vector<std::shared_ptr<ContinueAST>> continuations;
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<ContinueAST> continueAST = std::dynamic_pointer_cast<ContinueAST>(stmt);
        if (continueAST)
        {
            continuations.push_back(continueAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST)
        {
            for (const auto& cont : blockAST->getContinuations())
            {
                continuations.push_back(cont);
            }
        }

        std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST)
        {
            auto ifBlockAST = ifAST->getIfBlock();
            auto elseIfsBlockAST = ifAST->getElseIfs();
            auto elseBlockAST = ifAST->getElseBlock();
            for (const auto& cont : ifBlockAST->getContinuations())
            {
                continuations.push_back(cont);
            }
            if (!elseIfsBlockAST.empty())
            {
                for (size_t i = 0; i < elseIfsBlockAST.size(); i++)
                {
                    for (const auto& cont : elseIfsBlockAST[i].second->getContinuations())
                    {
                        continuations.push_back(cont);
                    }
                }
            }
            if (elseBlockAST)
            {
                for (const auto& cont : elseBlockAST->getContinuations())
                {
                    continuations.push_back(cont);
                }
            }
        }
    }
    return continuations;
}
