# TortLanguage
## What is it?

TortLanguage is a project to create my programming language.
In this project, a compiler is written that uses the LLVM backend.

## Features
Upon completion of its work, the program outputs the LLVM IR code to the console, which you can later compile using clang or llc.
## Example
        def sum(int a,int b)
        {
            return a+b;
        }
        
        def double div(double a,double b);

        def main()
        {
            int a = 10,b =5;
            print a;
            print b;
            b = sum(5,8);
            print a + b * 10;
            double c = div(5,2);
            return c;
        }
        def div(double a,double b)
        {
            return a / b ;
        }
Generates this llvm IR code:

    @0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
    @1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
    @2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
    
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
    
    define double @main() {
    entry:
      %a = alloca i32, align 4
      store i32 10, ptr %a, align 4
      %b = alloca i32, align 4
      store i32 5, ptr %b, align 4
      %0 = load i32, ptr %a, align 4
      %1 = call i32 (ptr, ...) @printf(ptr @0, i32 %0)
      %2 = load i32, ptr %b, align 4
      %3 = call i32 (ptr, ...) @printf(ptr @1, i32 %2)
      %calltmp = call i32 @sum(i32 5, i32 8)
      store i32 %calltmp, ptr %b, align 4
      %4 = load i32, ptr %a, align 4
      %5 = load i32, ptr %b, align 4
      %multmp = mul i32 %5, 10
      %addtmp = add i32 %4, %multmp
      %6 = call i32 (ptr, ...) @printf(ptr @2, i32 %addtmp)
      %c = alloca double, align 8
      %calltmp1 = call double @div(i32 5, i32 2)
      store double %calltmp1, ptr %c, align 8
      %7 = load double, ptr %c, align 8
      ret double %7
    }
    
    declare i32 @printf(ptr, ...)
Output:

    10
    5
    140
## TODO
- Add branches
- Add loops
- add types: str,bool
- implement pipeline compilation
