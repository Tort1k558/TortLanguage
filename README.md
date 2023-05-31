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
    print(fact(5));
	print(2|1);
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

define i32 @fact(i32 %arg0) {
entry:
  %retvar = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 %arg0, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %equaltmp = icmp eq i32 %0, 0
  br i1 %equaltmp, label %mergeblock, label %rhsblock

rhsblock:                                         ; preds = %entry
  %1 = load i32, ptr %a, align 4
  %equaltmp1 = icmp eq i32 %1, 1
  br label %mergeblock

mergeblock:                                       ; preds = %rhsblock, %entry
  %orresult = phi i1 [ %equaltmp, %entry ], [ %equaltmp1, %rhsblock ]
  br i1 %orresult, label %ifblock, label %elseblock

ifblock:                                          ; preds = %mergeblock
  store i32 1, ptr %retvar, align 4
  br label %returnblock

elseblock:                                        ; preds = %mergeblock
  %2 = load i32, ptr %a, align 4
  %subtmp = sub i32 %2, 1
  %calltmp = call i32 @fact(i32 %subtmp)
  %3 = load i32, ptr %a, align 4
  %multmp = mul i32 %calltmp, %3
  store i32 %multmp, ptr %retvar, align 4
  br label %returnblock

returnblock:                                      ; preds = %elseblock, %ifblock
  %4 = load i32, ptr %retvar, align 4
  ret i32 %4
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
  store i32 -1, ptr %b, align 4
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
  %lesstmp = icmp slt i32 %4, 0
  br i1 %lesstmp, label %ifblock, label %elseifblockhelp

ifblock:                                          ; preds = %mergeblock
  br label %mergeblock5

elseifblockhelp:                                  ; preds = %mergeblock
  %5 = load i32, ptr %b, align 4
  %lesstmp2 = icmp slt i32 %5, 99
  br i1 %lesstmp2, label %elseifblock, label %elseblock

elseifblock:                                      ; preds = %elseifblockhelp
  br label %whileexprblock

whileexprblock:                                   ; preds = %whileblock, %elseifblock
  %6 = load i32, ptr %b, align 4
  %lesstmp3 = icmp slt i32 %6, 5
  br i1 %lesstmp3, label %whileblock, label %mergeblock4

whileblock:                                       ; preds = %whileexprblock
  %7 = load i32, ptr %b, align 4
  %8 = call i32 (ptr, ...) @printf(ptr @1, i32 %7)
  %9 = load i32, ptr %b, align 4
  %incrementtmp = add i32 %9, 1
  store i32 %incrementtmp, ptr %b, align 4
  br label %whileexprblock

mergeblock4:                                      ; preds = %whileexprblock
  br label %mergeblock5

elseblock:                                        ; preds = %elseifblockhelp
  %10 = call i32 (ptr, ...) @printf(ptr @2, i32 0)
  br label %mergeblock5

mergeblock5:                                      ; preds = %elseblock, %mergeblock4, %ifblock
  %calltmp = call i32 @fact(i32 5)
  %11 = call i32 (ptr, ...) @printf(ptr @3, i32 %calltmp)
  %12 = call i32 (ptr, ...) @printf(ptr @4, i32 3)
  %13 = load i32, ptr %c, align 4
  ret i32 %13
}

declare i32 @printf(ptr, ...)

```

## Support
- Windows

## TODO
- Add loops
- Add types: str,bool
- Add modularity