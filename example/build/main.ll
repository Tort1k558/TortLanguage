; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str = private unnamed_addr constant [13 x i8] c"Hello World!\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
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
  br label %mergeblock10

elseifblockhelp:                                  ; preds = %mergeblock
  %5 = load i32, ptr %b, align 4
  %lesstmp2 = icmp slt i32 %5, 99
  br i1 %lesstmp2, label %elseifblock, label %elseblock

elseifblock:                                      ; preds = %elseifblockhelp
  br label %whileexprblock

whileexprblock:                                   ; preds = %mergeblock7, %ifblock4, %elseifblock
  %6 = load i32, ptr %b, align 4
  %lesstmp3 = icmp slt i32 %6, 5
  br i1 %lesstmp3, label %whileblock, label %mergeblock9

whileblock:                                       ; preds = %whileexprblock
  %7 = load i32, ptr %b, align 4
  %equaltmp = icmp eq i32 %7, 0
  br i1 %equaltmp, label %ifblock4, label %mergeblock5

ifblock4:                                         ; preds = %whileblock
  %8 = load i32, ptr %b, align 4
  %incrementtmp = add i32 %8, 1
  store i32 %incrementtmp, ptr %b, align 4
  br label %whileexprblock

mergeblock5:                                      ; preds = %whileblock
  %9 = load i32, ptr %b, align 4
  %10 = call i32 (ptr, ...) @printf(ptr @1, i32 %9)
  %11 = load i32, ptr %b, align 4
  %greatertmp = icmp sgt i32 %11, 4
  br i1 %greatertmp, label %ifblock6, label %mergeblock7

ifblock6:                                         ; preds = %mergeblock5
  br label %mergeblock9

mergeblock7:                                      ; preds = %mergeblock5
  %12 = load i32, ptr %b, align 4
  %incrementtmp8 = add i32 %12, 1
  store i32 %incrementtmp8, ptr %b, align 4
  br label %whileexprblock

mergeblock9:                                      ; preds = %ifblock6, %whileexprblock
  br label %mergeblock10

elseblock:                                        ; preds = %elseifblockhelp
  %13 = call i32 (ptr, ...) @printf(ptr @2, i32 0)
  br label %mergeblock10

mergeblock10:                                     ; preds = %elseblock, %mergeblock9, %ifblock
  %calltmp = call i32 @fact(i32 6)
  %14 = call i32 (ptr, ...) @printf(ptr @3, i32 %calltmp)
  %15 = call i32 (ptr, ...) @printf(ptr @4, i32 3)
  %powtmp = call double @llvm.pow.f64(double 1.000000e+01, double 5.000000e+00)
  %16 = call i32 (ptr, ...) @printf(ptr @5, double %powtmp)
  %17 = call i32 (ptr, ...) @printf(ptr @6, ptr @.str)
  %18 = call i32 (ptr, ...) @printf(ptr @7, i32 0)
  %autoTypeVar = alloca double, align 8
  store double 1.000000e+00, ptr %autoTypeVar, align 8
  %19 = call i32 (ptr, ...) @printf(ptr @8, i32 1)
  %20 = load i32, ptr %c, align 4
  ret i32 %20
}

declare i32 @printf(ptr, ...)

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.pow.f64(double, double) #0

attributes #0 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
