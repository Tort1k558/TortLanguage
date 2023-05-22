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
			bool a = true;
			print(a);
			a = false;
			print(a);
			if 0:
			{
				print(1);
			}
			else if 2:
			{
				print(2);
			}
			else
			{
				print(3);
			}
			print(55555);
			return 0;
		}


		def div(double a,double b)
		{
			return a / b;
		}
Generates this llvm IR code:

		; ModuleID = 'Tort'
		source_filename = "Tort"

		@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
		@1 = private unnamed_addr constant [6 x i8] c"true\0A\00", align 1
		@2 = private unnamed_addr constant [7 x i8] c"false\0A\00", align 1
		@3 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
		@4 = private unnamed_addr constant [6 x i8] c"true\0A\00", align 1
		@5 = private unnamed_addr constant [7 x i8] c"false\0A\00", align 1
		@6 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		@7 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
		@8 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

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
		  %a = alloca i1, align 1
		  store i1 true, ptr %a, align 1
		  %0 = load i1, ptr %a, align 1
		  %1 = select i1 %0, ptr @1, ptr @2
		  %2 = call i32 (ptr, ...) @printf(ptr @0, ptr %1)
		  store i1 false, ptr %a, align 1
		  %3 = load i1, ptr %a, align 1
		  %4 = select i1 %3, ptr @4, ptr @5
		  %5 = call i32 (ptr, ...) @printf(ptr @3, ptr %4)
		  br i1 false, label %ifblock, label %elseifblockhelp

		ifblock:                                          ; preds = %entry
		  %6 = call i32 (ptr, ...) @printf(ptr @6, i32 1)
		  br label %mergeblock

		elseifblockhelp:                                  ; preds = %entry
		  br i1 true, label %elseifblock, label %elseblock

		elseifblock:                                      ; preds = %elseifblockhelp
		  %7 = call i32 (ptr, ...) @printf(ptr @7, i32 2)
		  br label %mergeblock

		elseblock:                                        ; preds = %elseifblockhelp
		  br label %mergeblock

		mergeblock:                                       ; preds = %elseblock, %elseifblock, %ifblock
		  %8 = call i32 (ptr, ...) @printf(ptr @8, i32 55555)
		  ret i32 0
		}

		declare i32 @printf(ptr, ...)


Output:

    true
	false
	2
	55555
	

## Support
- Windows

## TODO
- Add loops
- Add types: str
- Add modularity