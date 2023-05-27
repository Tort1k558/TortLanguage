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

def double div(double a,double b);

def main()
{
    int a = ,b = 1;
    int c = cast<int>(a or b)+4;
	print(c);
    if a > 0:
    {
    	print(a);
    }
    else if b < 99:
    {
    	print(10);
    }
    else if a or b:
    {
    	print(9);
    }
    else:
    {
    	print(99999);
    }
    print(true);
    return c;
}


def div(double a,double b)
{
	return a / b;
}
```
Generates this llvm IR code without optimization:
```
; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@5 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@6 = private unnamed_addr constant [6 x i8] c"true\0A\00", align 1
@7 = private unnamed_addr constant [7 x i8] c"false\0A\00", align 1

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
  store i32 0, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 1, ptr %b, align 4
  %c = alloca i32, align 4
  %0 = load i32, ptr %a, align 4
  %i32toi1tmp = icmp ne i32 %0, 0
  br i1 %i32toi1tmp, label %mergeblock, label %rhsblock

rhsblock:                                         ; preds = %entry
  %1 = load i32, ptr %b, align 4
  %i32toi1tmp1 = icmp ne i32 %1, 0
  br label %mergeblock

mergeblock:                                       ; preds = %rhsblock, %entry
  %orresult = phi i1 [ %i32toi1tmp, %entry ], [ %i32toi1tmp1, %rhsblock ]
  %i1toi32tmp = zext i1 %orresult to i32
  %addtmp = add i32 %i1toi32tmp, 4
  store i32 %addtmp, ptr %c, align 4
  %2 = load i32, ptr %c, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
  %4 = load i32, ptr %a, align 4
  %greatertmp = icmp sgt i32 %4, 0
  br i1 %greatertmp, label %ifblock, label %elseifblockhelp

ifblock:                                          ; preds = %mergeblock
  %5 = load i32, ptr %a, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @1, i32 %5)
  br label %mergeblock9

elseifblockhelp:                                  ; preds = %mergeblock
  %7 = load i32, ptr %b, align 4
  %lesstmp = icmp slt i32 %7, 99
  br i1 %lesstmp, label %elseifblock, label %elseifblockhelp2

elseifblock:                                      ; preds = %elseifblockhelp
  %8 = call i32 (ptr, ...) @printf(ptr @2, i32 10)
  br label %mergeblock9

elseifblockhelp2:                                 ; preds = %elseifblockhelp
  %9 = load i32, ptr %a, align 4
  %i32toi1tmp4 = icmp ne i32 %9, 0
  br i1 %i32toi1tmp4, label %mergeblock7, label %rhsblock5

elseifblock3:                                     ; preds = %mergeblock7
  %10 = call i32 (ptr, ...) @printf(ptr @3, i32 9)
  br label %mergeblock9

rhsblock5:                                        ; preds = %elseifblockhelp2
  %11 = load i32, ptr %b, align 4
  %i32toi1tmp6 = icmp ne i32 %11, 0
  br label %mergeblock7

mergeblock7:                                      ; preds = %rhsblock5, %elseifblockhelp2
  %orresult8 = phi i1 [ %i32toi1tmp4, %elseifblockhelp2 ], [ %i32toi1tmp6, %rhsblock5 ]
  br i1 %orresult8, label %elseifblock3, label %elseblock

elseblock:                                        ; preds = %mergeblock7
  %12 = call i32 (ptr, ...) @printf(ptr @4, i32 99999)
  br label %mergeblock9

mergeblock9:                                      ; preds = %elseblock, %elseifblock3, %elseifblock, %ifblock
  %13 = call i32 (ptr, ...) @printf(ptr @5, ptr @6)
  %14 = load i32, ptr %c, align 4
  ret i32 %14
}

declare i32 @printf(ptr, ...)

```

## Support
- Windows

## TODO
- Add loops
- Add types: str,bool
- Add modularity