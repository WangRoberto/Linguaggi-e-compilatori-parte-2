define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = mul nsw i32 %1, 128
  %4 = mul nsw i32 %3, 6
  %5 = mul nsw i32 128, %4
  %6 = mul nsw i32 6, %5
  %7 = mul nsw i32 %1, 129
  %8 = mul nsw i32 %7, 6
  %9 = mul nsw i32 129, %8
  %10 = mul nsw i32 6, %9 
  %11 = mul nsw i32 %1, 127
  %12 = mul nsw i32 %11, 6
  %13 = mul nsw i32 127, %4
  %14 = mul nsw i32 6, %13 
  %15 = sdiv i32 %14, 16
  %16 = mul nsw i32 %15, 6
  %17 = sdiv i32 16, 16
  %18 = mul nsw i32 %17, 6
  %19 = sdiv i32 10, 17
  %20 = mul nsw i32 %19, 6
  ret i32 %20
}

