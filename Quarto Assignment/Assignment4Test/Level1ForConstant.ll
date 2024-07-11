; ModuleID = 'Level1ForConstant.optimized.bc'
source_filename = "Level1ForConstant.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define dso_local void @foo(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) {
  br label %5

5:                                                ; preds = %18, %4
  %.01 = phi i32 [ 0, %4 ], [ %19, %18 ]
  %6 = icmp slt i32 %.01, 10
  br i1 %6, label %7, label %20

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
  br label %18

18:                                               ; preds = %7
  %19 = add nsw i32 %.01, 1
  br label %5, !llvm.loop !6

20:                                               ; preds = %5
  br label %21

21:                                               ; preds = %33, %20
  %.0 = phi i32 [ 0, %20 ], [ %34, %33 ]
  %22 = icmp slt i32 %.0, 10
  br i1 %22, label %23, label %35

23:                                               ; preds = %21
  %24 = sext i32 %.0 to i64
  %25 = getelementptr inbounds i32, ptr %0, i64 %24
  %26 = load i32, ptr %25, align 4
  %27 = sext i32 %.0 to i64
  %28 = getelementptr inbounds i32, ptr %2, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %26, %29
  %31 = sext i32 %.0 to i64
  %32 = getelementptr inbounds i32, ptr %3, i64 %31
  store i32 %30, ptr %32, align 4
  br label %33

33:                                               ; preds = %23
  %34 = add nsw i32 %.0, 1
  br label %21, !llvm.loop !8

35:                                               ; preds = %21
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
!8 = distinct !{!8, !7}
