; ModuleID = 'main'
source_filename = "main"

define i32 @main() {
entry:
  %retvar = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 5, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %1 = zext i32 %0 to i64
  %2 = call ptr @llvm.stacksave()
  %3 = alloca [3 x [3 x i32]], i64 %1, align 4
  %4 = load i32, ptr %a, align 4
  %equaltmp = icmp eq i32 %4, 5
  br i1 %equaltmp, label %ifblock, label %mergeblock

ifblock:                                          ; preds = %entry
  store i32 10, ptr %retvar, align 4
  call void @llvm.stackrestore(ptr %2)
  br label %returnblock

mergeblock:                                       ; preds = %entry
  %ptrtoelementarray = getelementptr inbounds [3 x [3 x i32]], ptr %3, i64 2
  %ptrtoelementarray1 = getelementptr inbounds [3 x [3 x i32]], ptr %ptrtoelementarray, i64 0, i64 2
  %ptrtoelementarray2 = getelementptr inbounds [3 x i32], ptr %ptrtoelementarray1, i64 0, i64 2
  %5 = load i32, ptr %ptrtoelementarray2, align 4
  store i32 %5, ptr %retvar, align 4
  call void @llvm.stackrestore(ptr %2)
  br label %returnblock

returnblock:                                      ; preds = %mergeblock, %ifblock
  %6 = load i32, ptr %retvar, align 4
  ret i32 %6
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave() #0

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore(ptr) #0

attributes #0 = { nocallback nofree nosync nounwind willreturn }
