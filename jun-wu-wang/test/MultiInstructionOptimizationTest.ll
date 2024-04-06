define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %1, 20
  %4 = sub nsw i32 %3, 20
  %5 = mul nsw i32 %4, 2
  %6 = sub nsw i32 %5, 20
  %7 = add nsw i32 %6, 20
  %8 = mul nsw i32 %7, 2
  %9 = add nsw i32 31, 20
  %10 = sub nsw i32 %9, 20
  %11 = mul nsw i32 %10, 2
  ret i32 %11
}
