; ModuleID = 'Tort'
source_filename = "Tort"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

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
  %calltmp1 = call double @div(double 5.000000e+00, double 2.000000e+00)
  store double %calltmp1, ptr %c, align 8
  %7 = load double, ptr %c, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @3, double %7)
  %9 = load double, ptr %c, align 8
  ret double %9
}

declare i32 @printf(ptr, ...)
