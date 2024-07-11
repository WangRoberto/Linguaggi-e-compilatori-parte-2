; ModuleID = 'Level1ForConstant.optimized.bc'
source_filename = "Level1ForConstant.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define dso_local void @foo(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) {
  br label %5

5:                                                ; preds = %18, %4
  %.01 = phi i32 [ 0, %4 ], [ %19, %18 ]
  %6 = icmp slt i32 %.01, 10
  br i1 %6, label %7, label %30

7:                                                ; preds = %5
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds i32, ptr %1, i64 %8
  %10 = load i32, ptr %9, align 4
  %11 = sdiv i32 1, %10
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, ptr %2, i64 %12
  %14 = load i32, ptr %13, align 4
  %15 = mul nsw i32 %11, %14
  %16 = sext i32 %.01 to i64
  %17 = getelementptr inbounds i32, ptr %0, i64 %16
  store i32 %15, ptr %17, align 4
  br label %20

18:                                               ; preds = %20
  %19 = add nsw i32 %.01, 1
  br label %5, !llvm.loop !6

20:                                               ; preds = %7
  %21 = sext i32 %.01 to i64
  %22 = getelementptr inbounds i32, ptr %0, i64 %21
  %23 = load i32, ptr %22, align 4
  %24 = sext i32 %.01 to i64
  %25 = getelementptr inbounds i32, ptr %2, i64 %24
  %26 = load i32, ptr %25, align 4
  %27 = add nsw i32 %23, %26
  %28 = sext i32 %.01 to i64
  %29 = getelementptr inbounds i32, ptr %3, i64 %28
  store i32 %27, ptr %29, align 4
  br label %18

30:                                               ; preds = %5
  ret void
}

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 17.0.6"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
