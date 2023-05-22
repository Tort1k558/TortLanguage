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
