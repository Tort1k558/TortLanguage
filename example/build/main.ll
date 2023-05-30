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
  br label %mergeblock9

elseifblockhelp:                                  ; preds = %mergeblock
  %5 = load i32, ptr %b, align 4
  %lesstmp = icmp slt i32 %5, 99
  br i1 %lesstmp, label %elseifblock, label %elseifblockhelp2

elseifblock:                                      ; preds = %elseifblockhelp
  br label %mergeblock9

elseifblockhelp2:                                 ; preds = %elseifblockhelp
  %6 = load i32, ptr %a, align 4
  %i32toi1tmp4 = icmp ne i32 %6, 0
  br i1 %i32toi1tmp4, label %mergeblock7, label %rhsblock5

elseifblock3:                                     ; preds = %mergeblock7
  %7 = call i32 (ptr, ...) @printf(ptr @1, i32 9)
  br label %mergeblock9

rhsblock5:                                        ; preds = %elseifblockhelp2
  %8 = load i32, ptr %b, align 4
  %i32toi1tmp6 = icmp ne i32 %8, 0
  br label %mergeblock7

mergeblock7:                                      ; preds = %rhsblock5, %elseifblockhelp2
  %orresult8 = phi i1 [ %i32toi1tmp4, %elseifblockhelp2 ], [ %i32toi1tmp6, %rhsblock5 ]
  br i1 %orresult8, label %elseifblock3, label %elseblock

elseblock:                                        ; preds = %mergeblock7
  %9 = call i32 (ptr, ...) @printf(ptr @2, i32 99999)
  br label %mergeblock9

mergeblock9:                                      ; preds = %elseblock, %elseifblock3, %elseifblock, %ifblock
  %calltmp = call i32 @fact(i32 5)
  %10 = call i32 (ptr, ...) @printf(ptr @3, i32 %calltmp)
  %11 = call i32 (ptr, ...) @printf(ptr @4, i32 3)
  %12 = load i32, ptr %c, align 4
  ret i32 %12
}

declare i32 @printf(ptr, ...)
