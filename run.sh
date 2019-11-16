clang -S -emit-llvm -Xclang -disable-O0-optnone test.cpp
/usr/local/opt/llvm/bin/opt -load build/function-inlining/libFunctionInliningPass.* -function-inlining -S test.ll -o test_opt.ll
