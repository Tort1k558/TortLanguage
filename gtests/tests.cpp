#include<gtest/gtest.h>
#include"Compiler.h"

TEST(TestCompile,Print)
{
	Compiler compiler;
	compiler.setInputFile("tests/PrintTest/print.tt");
	try
	{
		compiler.compile();
		ASSERT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what();
	}
}

TEST(TestCompile, VarDecl)
{
	Compiler compiler;
	compiler.setInputFile("tests/VarDeclTest/vardecl.tt");
	try
	{
		compiler.compile();
		ASSERT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what();
	}
}

TEST(TestCompile, If)
{
	Compiler compiler;
	compiler.setInputFile("tests/IfTest/if.tt");
	try
	{
		compiler.compile();
		ASSERT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what();
	}
}

TEST(TestCompile, BinaryOp)
{
	Compiler compiler;
	compiler.setInputFile("tests/BinaryOpTest/binaryop.tt");
	try
	{
		compiler.compile();
		ASSERT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what();
	}
}

TEST(TestCompile, Function)
{
	Compiler compiler;
	compiler.setInputFile("tests/FuncTest/func.tt");
	try
	{
		compiler.compile();
		ASSERT_TRUE(true);
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what();
	}
}