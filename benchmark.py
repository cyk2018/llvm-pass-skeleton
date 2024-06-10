import os
import subprocess
import sys
import re

PASS_NAME = 'function-inlining'
PASS_PATH = os.path.expanduser(
    '~/Documents/5-postgraduation/function_inline/llvm-pass-skeleton/build/function-inlining/libFunctionInliningPass.so')
POLYBENCH_CODE_PATH = os.path.expanduser(
    '~/Documents/5-postgraduation/1-compiler-2024/llvm-test-suite/SingleSource/Benchmarks/Misc')
RUNS_PER_BENCHMARK = 5
BINARY_FILE = './a.out'


def emit_llvm_command(c_file, lib):
    return f'clang -Wno-implicit-int -lm -S -emit-llvm -Xclang -disable-O0-optnone {c_file}'
    # return f'clang -S -emit-llvm -Xclang -disable-O0-optnone -I {lib} {c_file}'


def opt_command(name):
    return f'opt -load-pass-plugin={PASS_PATH} -passes={PASS_NAME} -S {name}.ll -o {name}_opt.ll'


def clang_command(name, polybench_c):
    return f'clang {name} -lm -Wno-implicit-int -o {BINARY_FILE}'
    # return f'clang {name} {polybench_c} -o {BINARY_FILE}'


def benchmark(ll_file, polybench_c):
    result = subprocess.run(clang_command(ll_file, polybench_c).split())
    if(result.returncode != 0):
        return 0
    # assert result.returncode == 0, 'compiling program failed'
    result = subprocess.run(['time', BINARY_FILE], capture_output=True)
    assert result.returncode == 0, 'running program failed' + str(result)
    # print(result.stderr)
    match = re.search(r'(\d+):(\d+\.\d+)elapsed', result.stderr.decode())
    if match:
        minutes, seconds = map(float, match.groups())
        total_seconds = minutes * 60 + seconds
        return total_seconds
    else:
        print("未找到运行时间信息")
    # print(result.stderr.decode().strip().split('\n'))
    # return float(result.stderr.strip().split()[-4])


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python3 benchmark.py [output_csv_path]")
        sys.exit(1)
    out_file = sys.argv[1]
    polybench = POLYBENCH_CODE_PATH
    c_files = []
    lib = os.path.join(polybench, 'utilities')
    polybench_c = os.path.join(lib, 'polybench.c')
    for root, subdirs, files in os.walk(polybench):
        if 'utilities' in os.path.splitext(root):
            continue
        c_files += [os.path.join(root, f) for f in files if f.endswith('.c')]
    print(f'Collected \033[1m{len(c_files)}\033[0m files to benchmark.')
    print(f'\u001b[35mOutput will be saved to {out_file}\u001b[0m')
    output = open(out_file, 'w')
    output.write('Name, Original, Optimized\n')
    for f in c_files:
        name = os.path.splitext(os.path.basename(f))[-2]
        print('----------------')
        print(f'\033[1mBenchmarking {name}\033[0m')
        result = subprocess.run(emit_llvm_command(f, lib).split())
        assert result.returncode == 0, 'll generation failed'
        result = subprocess.run(opt_command(name).split())
        assert result.returncode == 0, 'running opt failed'
        original = f'{name}.ll'
        optimized = f'{name}_opt.ll'
        for i in range(RUNS_PER_BENCHMARK):
            print(f'\033[4mRun {i + 1} of {RUNS_PER_BENCHMARK}\033[0m')
            orig = benchmark(original, polybench_c)
            print(f'\033[36mOriginal: {orig}\033[0m')
            opt = benchmark(optimized, polybench_c)
            print(f'\033[92mOptimized: {opt}\033[0m')
            output.write(f'{name}, {str(orig)}, {str(opt)}\n')
    output.close()
