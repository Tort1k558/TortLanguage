#pragma once
#include"Token.h"
#include"Lexer.h"
#include<llvm/Support/SMloc.h>

class TokenStream
{
private:
	struct StreamNode
	{
		StreamNode(StreamNode* next,StreamNode* prev):
			next(next),
			prev(prev)
		{}

		Token token;
		StreamNode* prev;
		StreamNode* next;
	};
	class TokenStreamIterator
	{
	public:
		TokenStreamIterator(const TokenStreamIterator& other)
			: m_currentStream(other.m_currentStream),
			m_currentPos(other.m_currentPos) {}
		TokenStreamIterator& operator=(const TokenStreamIterator& other) {
			if (&other != this) {
				m_currentStream = other.m_currentStream;
				m_currentPos = other.m_currentPos;
			}

			return *this;
		}
		TokenType type() const
		{
			return m_currentPos->token.type;
		}
		bool empty() const
		{
			return (m_currentStream == nullptr && m_currentPos == nullptr);
		}
		bool operator==(TokenType type) const
		{
			return m_currentPos->token.type == type;
		}
		bool operator!=(TokenType type) const
		{
			return m_currentPos->token.type != type;
		}
		const Token& operator&()
		{
			return m_currentPos->token;
		}
		const Token* operator*()
		{
			return &m_currentPos->token;
		}
		TokenStreamIterator operator++(int)
		{
			TokenStreamIterator tmp = *this;
			++(*this);
			return tmp;
		}
		TokenStreamIterator &operator++()
		{
			m_currentPos = m_currentStream->next(m_currentPos);
			return *this;
		}
		TokenStreamIterator operator --(int) {
			TokenStreamIterator tmp = *this;
			--(*this);
			return tmp;
		}

		TokenStreamIterator& operator --() {
			if (m_currentPos->prev) {
				m_currentPos = m_currentPos->prev;
			}
			return *this;
		}
		TokenStreamIterator operator +(int count) {
			TokenStreamIterator tmp = *this;
			while (count--) {
				++tmp;
			}
			return tmp;
		}

		TokenStreamIterator operator -(int count) {
			TokenStreamIterator tmp = *this;
			while (count--) {
				--tmp;
			}
			return tmp;
		}
		friend class TokenStream;
	private:
		TokenStream* m_currentStream;
		StreamNode* m_currentPos;
		TokenStreamIterator(TokenStream* source = nullptr, StreamNode* currentPos = nullptr)
			: m_currentStream(source),
			  m_currentPos(currentPos) {}
	};
public:
	typedef TokenStreamIterator TokenIterator;
	TokenStream(Lexer* lexer);
	~TokenStream();

	TokenStreamIterator begin();
private:
		Lexer* m_lexer;
		StreamNode* m_head;
		StreamNode* m_tail;
		bool m_scanDone;
		StreamNode* next(StreamNode* currentPos);
};

