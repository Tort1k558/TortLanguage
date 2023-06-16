# TortLanguage
[Documentation](https://tort1k558.github.io/TortLanguage/#/)
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
	return a + b;
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
    while b < 5:
	{
        if b == 2:
        {
            b++;
            continue;
        }
		print(b);
		b++;
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

## How to build a project?
- Clone this repository
    ```
    git clone https://github.com/Tort1k558/TortLanguage.git
    ```
- Create the llvmBuild folder in the compiler root folder
- build an LLVM with flag 
    ```
    -DLLVM_ENABLE_PROJECTS="llvm;lld"
    ```
- Go to the LLVM build folder
- run this command 
    ```
    cmake -DCMAKE_INSTALL_PREFIX="Path to the compiler project"/llvmBuild -P cmake_install.cmake
    ```
- At this stage, all dependencies are connected, you can start building the project using CMake
    ```
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```
Attention, the compiler assembly must be with the same config with which you built LLVM. For example, if you were building in debug LLVM, then the compiler should be built with the same config.
If you failed to connect LLVM in this way, you can connect LLVM in any way convenient for you

## Support
- Windows

## TODO
- Add types: float, int8, int16, pointers
- Add loop for 
- Add modularity
- Added classes