#include "TokenStream.h"

TokenStream::TokenStream(Lexer* lex)
    : m_lexer(lex),
    m_head(nullptr),
    m_tail(nullptr),
    m_scanDone(false) {
}
TokenStream::~TokenStream()
{
    StreamNode* p = m_head;

    while (p) {
        StreamNode* tmp = p;
        p = p->next;
        delete tmp;
    }
}
TokenStream::TokenStreamIterator TokenStream::begin()
{
    if (!m_head)
    {
        StreamNode* p = new StreamNode(nullptr, nullptr);
        p->token = m_lexer->getNextToken();
        m_scanDone = p->token.type == TokenType::EndOfFile;
        m_tail = m_head = p;
    }
    return(TokenStreamIterator(this, m_head));
}
TokenStream::StreamNode* TokenStream::next(StreamNode* currentPos)
{
    if (!currentPos->next)
    {
        if (m_scanDone)
        {
            return currentPos;
        }
        StreamNode* p = new StreamNode(nullptr, m_tail);
        p->token = m_lexer->getNextToken();
        m_scanDone = p->token.type == TokenType::EndOfFile;
        m_tail->next = p;
        m_tail = p;
        return m_tail;
    }
    else
    {
        return currentPos->next;
    }
}