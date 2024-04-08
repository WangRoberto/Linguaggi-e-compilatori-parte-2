define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %1, 0
  %4 = mul nsw i32 %3, 2
  %5 = add nsw i32 0, %1
  %6 = mul nsw i32 2, %5
  %7 = sub nsw i32 %1, 0
  %8 = mul nsw i32 %7, 2
  %9 = sub nsw i32 0, %1
  %10 = mul nsw i32 2, %9
  %11 = mul nsw i32 %1, 1
  %12 = mul nsw i32 %11, 2
  %13 = mul nsw i32 1, %1
  %14 = mul nsw i32 2, %13
  %15 = sdiv i32 %1, 1
  %16 = mul nsw i32 %15, 2
  %17 = sdiv i32 1, %1
  %18 = mul nsw i32 2, %17
  %19 = add nsw i32 2, 0
  %20 = mul nsw i32 %19, 2
  %21 = add nsw i32 0, 2
  %22 = mul nsw i32 2, %21
  ret i32 %18
}

