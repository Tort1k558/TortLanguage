; ModuleID = 'Tort'
source_filename = "Tort"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

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
  br i1 false, label %ifblock, label %elseblock

ifblock:                                          ; preds = %entry
  %2 = load i32, ptr %a, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
  br label %mergeblock2

elseblock:                                        ; preds = %entry
  %4 = load i32, ptr %b, align 4
  %5 = call i32 (ptr, ...) @printf(ptr @1, i32 %4)
  br i1 true, label %ifblock1, label %mergeblock

ifblock1:                                         ; preds = %elseblock
  %6 = call i32 (ptr, ...) @printf(ptr @2, i32 10)
  br label %mergeblock

mergeblock:                                       ; preds = %ifblock1, %elseblock
  br label %mergeblock2

mergeblock2:                                      ; preds = %mergeblock, %ifblock
  %7 = call i32 (ptr, ...) @printf(ptr @3, i32 5555555)
  br i1 true, label %ifblock3, label %elseblock4

ifblock3:                                         ; preds = %mergeblock2
  %8 = load i32, ptr %a, align 4
  %9 = call i32 (ptr, ...) @printf(ptr @4, i32 %8)
  br label %mergeblock5

elseblock4:                                       ; preds = %mergeblock2
  %10 = load i32, ptr %b, align 4
  %11 = call i32 (ptr, ...) @printf(ptr @5, i32 %10)
  br label %mergeblock5

mergeblock5:                                      ; preds = %elseblock4, %ifblock3
  %12 = load i32, ptr %c, align 4
  ret i32 %12
}

declare i32 @printf(ptr, ...)
