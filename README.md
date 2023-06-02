# TortLanguage

## What is it?
Tort is a custom programming language that I created for learning purposes, because it was interesting how compilers for programming languages are written. The file with the Tort code must have an extension .tt . Syntax I made a mixture of languages like Python and C++.

## Compiler
##### The compiler uses LLVM as a backend.
Upon completion of this work, the compiler generates files such as:
- Token files - It describes each type of word and its affiliation
- Assembler file - Generated assembler code
- Object file, Which is then linked to an executable file using the lld linker
- Executable file

## Example

```

def sum(int a,int b)
{
	return a+b;
}

def fact(int a)
{
	if a == 0 or a == 1:
	{
		return 1;
	}
	else:
	{
		return fact(a-1) * a;
	}
}

def double div(double a,double b);

def main()
{
    int a = 2,b = -1;
    int c = cast<int>(a or b)+4;
	print(c);
    if a < 0:
    {
    	
    }
    else if b < 99:
    {
    	while b < 5:
		{
			print(b);
			b++;
		}
    }
    else:
	{
		print(000000);
	}
    print(fact(6));
	print(2|1);
	print(10.0 ** 5.0);
	print("Hello World!");
    return c;
}


def div(double a,double b)
{
	return a / b;
}

```
[Generates this llvm IR code without optimization](example/build/main.ll)

## Support
- Windows

## TODO
- Add types: void, float, int8, int16
- Add loop for 
- Add break, continue
- Add modularity
- Added classes