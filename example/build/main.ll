; ModuleID = 'Tort'
source_filename = "Tort"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

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

define void @main() {
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
  br label %mergeblock

mergeblock:                                       ; preds = %elseblock, %ifblock
  %4 = load i32, ptr %c, align 4
  ret void

elseblock:                                        ; preds = %entry
  %5 = load i32, ptr %b, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @1, i32 %5)
  br label %mergeblock
}

declare i32 @printf(ptr, ...)