; ModuleID = 'dust_prog'
source_filename = "dust_prog"

@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [19 x i8] c"From input to LLVM\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@4 = private unnamed_addr constant [6 x i8] c"false\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@6 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@7 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@8 = private unnamed_addr constant [21 x i8] c"froom boolean to str\00", align 1
@9 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@10 = private unnamed_addr constant [5 x i8] c"true\00", align 1
@11 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

define i64 @main() {
entrypoint:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @3, ptr @2)
  %2 = call i32 (ptr, ...) @printf(ptr @5, ptr @4)
  %3 = call i32 (ptr, ...) @printf(ptr @7, ptr @6)
  %4 = call i32 (ptr, ...) @printf(ptr @9, ptr @8)
  %5 = call i32 (ptr, ...) @printf(ptr @11, ptr @10)
  ret i64 122
  ret i64 0
}

declare i32 @printf(ptr, ...)