clang -S -emit-llvm -Xclang -disable-O0-optnone test.cpp
opt -load build/skeleton/libSkeletonPass.* -skeleton -S test.ll -o test_opt.ll
