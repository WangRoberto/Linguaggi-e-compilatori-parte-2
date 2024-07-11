; ModuleID = 'Level2For.optimized.bc'
source_filename = "Level2For.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define dso_local void @foo(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3, i32 noundef %4, ...) {
  br label %6

6:                                                ; preds = %34, %5
  %.03 = phi i32 [ 0, %5 ], [ %35, %34 ]
  %7 = icmp slt i32 %.03, %4
  br i1 %7, label %8, label %36

8:                                                ; preds = %6
  br label %9

9:                                                ; preds = %31, %8
  %.02 = phi i32 [ 0, %8 ], [ %32, %31 ]
  %10 = icmp slt i32 %.02, %4
  br i1 %10, label %11, label %33

11:                                               ; preds = %9
  %12 = sext i32 %.03 to i64
  %13 = getelementptr inbounds ptr, ptr %1, i64 %12
  %14 = load ptr, ptr %13, align 8
  %15 = sext i32 %.02 to i64
  %16 = getelementptr inbounds i32, ptr %14, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = sdiv i32 1, %17
  %19 = sext i32 %.03 to i64
  %20 = getelementptr inbounds ptr, ptr %2, i64 %19
  %21 = load ptr, ptr %20, align 8
  %22 = sext i32 %.02 to i64
  %23 = getelementptr inbounds i32, ptr %21, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = mul nsw i32 %18, %24
  %26 = sext i32 %.03 to i64
  %27 = getelementptr inbounds ptr, ptr %0, i64 %26
  %28 = load ptr, ptr %27, align 8
  %29 = sext i32 %.02 to i64
  %30 = getelementptr inbounds i32, ptr %28, i64 %29
  store i32 %25, ptr %30, align 4
  br label %31

31:                                               ; preds = %11
  %32 = add nsw i32 %.02, 1
  br label %9, !llvm.loop !6

33:                                               ; preds = %9
  br label %34

34:                                               ; preds = %33
  %35 = add nsw i32 %.03, 1
  br label %6, !llvm.loop !8

36:                                               ; preds = %6
  br label %37

37:                                               ; preds = %65, %36
  %.01 = phi i32 [ 0, %36 ], [ %66, %65 ]
  %38 = icmp slt i32 %.01, %4
  br i1 %38, label %39, label %67

39:                                               ; preds = %37
  br label %40

40:                                               ; preds = %62, %39
  %.0 = phi i32 [ 0, %39 ], [ %63, %62 ]
  %41 = icmp slt i32 %.0, %4
  br i1 %41, label %42, label %64

42:                                               ; preds = %40
  %43 = add nsw i32 %.01, 1
  %44 = sext i32 %43 to i64
  %45 = getelementptr inbounds ptr, ptr %0, i64 %44
  %46 = load ptr, ptr %45, align 8
  %47 = sext i32 %.0 to i64
  %48 = getelementptr inbounds i32, ptr %46, i64 %47
  %49 = load i32, ptr %48, align 4
  %50 = sext i32 %.01 to i64
  %51 = getelementptr inbounds ptr, ptr %2, i64 %50
  %52 = load ptr, ptr %51, align 8
  %53 = sext i32 %.0 to i64
  %54 = getelementptr inbounds i32, ptr %52, i64 %53
  %55 = load i32, ptr %54, align 4
  %56 = add nsw i32 %49, %55
  %57 = sext i32 %.01 to i64
  %58 = getelementptr inbounds ptr, ptr %3, i64 %57
  %59 = load ptr, ptr %58, align 8
  %60 = sext i32 %.0 to i64
  %61 = getelementptr inbounds i32, ptr %59, i64 %60
  store i32 %56, ptr %61, align 4
  br label %62

62:                                               ; preds = %42
  %63 = add nsw i32 %.0, 1
  br label %40, !llvm.loop !9

64:                                               ; preds = %40
  br label %65

65:                                               ; preds = %64
  %66 = add nsw i32 %.01, 1
  br label %37, !llvm.loop !10

67:                                               ; preds = %37
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
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
