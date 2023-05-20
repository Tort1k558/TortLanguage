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
        
		def sum(int a,int b)
		{
			return a+b;
		}
		
		def double div(double a,double b);
		
		def main()
		{
			int a = 2,b = 1;
			int c = a | b + 4;
			if 0:
			{
				print a;
			}
			else if 0:
			{
				print 10;
			}
			else if 1:
			{
				print 9;
			}
			else:
			{
				print 99999;
			}
			print 5555555;
			return c;
		}
		
		def div(double a,double b)
		{
			return a / b;
		}
Generates this llvm IR code:

		; ModuleID = 'Tort'
		source_filename = "Tort"
		
		@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		@4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		
		define i32 @sum(i32 %arg0, i32 %arg1) {
		entry:
		%a = alloca i32, align 4
		store i32 %arg0, ptr %a, align 4
		%b = alloca i32, align 4
		store i32 %arg1, ptr %b, align 4
		%0 = load i32, ptr %a, align 4
		%1 = load i32, ptr %b, align 4
		%addtmp = add i32 %0, %1
		ret i32 %addtmp
		}
		
		define double @div(double %arg0, double %arg1) {
		entry:
		%a = alloca double, align 8
		store double %arg0, ptr %a, align 8
		%b = alloca double, align 8
		store double %arg1, ptr %b, align 8
		%0 = load double, ptr %a, align 8
		%1 = load double, ptr %b, align 8
		%divtmp = fdiv double %0, %1
		ret double %divtmp
		}
		
		define i32 @main() {
		entry:
		%a = alloca i32, align 4
		store i32 2, ptr %a, align 4
		%b = alloca i32, align 4
		store i32 1, ptr %b, align 4
		%c = alloca i32, align 4
		%0 = load i32, ptr %a, align 4
		%1 = load i32, ptr %b, align 4
		%addtmp = add i32 %1, 4
		%bitortmp = or i32 %0, %addtmp
		store i32 %bitortmp, ptr %c, align 4
		br i1 false, label %ifblock, label %elseifblockhelp
		
		ifblock:                                          ; preds = %entry
		%2 = load i32, ptr %a, align 4
		%3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
		br label %mergeblock
		
		elseifblockhelp:                                  ; preds = %entry
		br i1 false, label %elseifblock, label %elseifblockhelp1
		
		elseifblock:                                      ; preds = %elseifblockhelp
		%4 = call i32 (ptr, ...) @printf(ptr @1, i32 10)
		br label %mergeblock
		
		elseifblockhelp1:                                 ; preds = %elseifblockhelp
		br i1 true, label %elseifblock2, label %elseblock
		
		elseifblock2:                                     ; preds = %elseifblockhelp1
		%5 = call i32 (ptr, ...) @printf(ptr @2, i32 9)
		br label %mergeblock
		
		elseblock:                                        ; preds = %elseifblockhelp1
		%6 = call i32 (ptr, ...) @printf(ptr @3, i32 99999)
		br label %mergeblock
		
		mergeblock:                                       ; preds = %elseblock, %elseifblock2, %elseifblock, %ifblock
		%7 = call i32 (ptr, ...) @printf(ptr @4, i32 5555555)
		%8 = load i32, ptr %c, align 4
		ret i32 %8
		}
		
		declare i32 @printf(ptr, ...)

Output:

    10
	5
	140
	2.500000
	

## Support
- Windows

## TODO
- Add loops
- Add types: str,bool
- Add modularity