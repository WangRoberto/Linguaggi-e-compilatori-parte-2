; ModuleID = 'LICM.c'
source_filename = "LICM.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  store i32 9, ptr %5, align 4
  store i32 0, ptr %7, align 4
  store i32 0, ptr %8, align 4
  store i32 0, ptr %10, align 4
  store i32 0, ptr %11, align 4
  br label %12

12:                                               ; preds = %35, %2
  %13 = load i32, ptr %4, align 4
  %14 = add nsw i32 %13, 1
  store i32 %14, ptr %4, align 4
  %15 = load i32, ptr %3, align 4
  %16 = add nsw i32 %15, 3
  store i32 %16, ptr %11, align 4
  %17 = load i32, ptr %3, align 4
  %18 = add nsw i32 %17, 7
  store i32 %18, ptr %9, align 4
  %19 = load i32, ptr %4, align 4
  %20 = icmp slt i32 %19, 5
  br i1 %20, label %21, label %26

21:                                               ; preds = %12
  %22 = load i32, ptr %5, align 4
  %23 = add nsw i32 %22, 2
  store i32 %23, ptr %5, align 4
  %24 = load i32, ptr %3, align 4
  %25 = add nsw i32 %24, 3
  store i32 %25, ptr %6, align 4
  br label %35

26:                                               ; preds = %12
  %27 = load i32, ptr %5, align 4
  %28 = sub nsw i32 %27, 1
  store i32 %28, ptr %5, align 4
  %29 = load i32, ptr %3, align 4
  %30 = add nsw i32 %29, 4
  store i32 %30, ptr %6, align 4
  %31 = load i32, ptr %4, align 4
  %32 = icmp sge i32 %31, 10
  br i1 %32, label %33, label %34

33:                                               ; preds = %26
  br label %44

34:                                               ; preds = %26
  br label %35

35:                                               ; preds = %34, %21
  %36 = load i32, ptr %11, align 4
  %37 = add nsw i32 %36, 7
  store i32 %37, ptr %7, align 4
  %38 = load i32, ptr %6, align 4
  %39 = add nsw i32 %38, 2
  store i32 %39, ptr %8, align 4
  %40 = load i32, ptr %3, align 4
  %41 = add nsw i32 %40, 7
  store i32 %41, ptr %11, align 4
  %42 = load i32, ptr %9, align 4
  %43 = add nsw i32 %42, 5
  store i32 %43, ptr %10, align 4
  br label %12

44:                                               ; preds = %33
  %45 = load i32, ptr %5, align 4
  %46 = load i32, ptr %6, align 4
  %47 = load i32, ptr %7, align 4
  %48 = load i32, ptr %8, align 4
  %49 = load i32, ptr %9, align 4
  %50 = load i32, ptr %10, align 4
  %51 = load i32, ptr %11, align 4
  %52 = load i32, ptr %4, align 4
  %53 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %45, i32 noundef %46, i32 noundef %47, i32 noundef %48, i32 noundef %49, i32 noundef %50, i32 noundef %51, i32 noundef %52)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  call void @foo(i32 noundef 0, i32 noundef 4)
  call void @foo(i32 noundef 0, i32 noundef 12)
  ret i32 0
}

;attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 17.0.6"}
