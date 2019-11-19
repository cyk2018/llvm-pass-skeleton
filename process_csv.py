import os
import statistics
import sys

def get_mean_std(out_csv):
    with open(out_csv) as f:
        lines = f.readlines()
    tests = dict()
    for t in lines[1:]:
        t = t.split(",")
        test_name = t[0].strip()
        opt = float(t[2].strip())
        tests[test_name] = tests.get(test_name, list()) + [opt]
    means = {t: sum(v)/len(v) for t, v in tests.items()}
    std = {t: statistics.stdev(v) for t, v in tests.items()}
    return means

def get_tests(out_csv):
    tests = set()
    orig = dict()
    with open(out_csv) as f:
        lines = f.readlines()
    for l in lines[1:]:
        l = l.split(",")
        name = l[0].strip()
        original_runtime = float(l[1].strip())
        tests.add(name)
        orig[name] = orig.get(name, list()) + [original_runtime]
    tests = sorted(list(tests))
    means = {t: sum(v)/len(v) for t, v in orig.items()}
    std = {t: statistics.stdev(v) for t, v in orig.items()}
    return [[t, str(means[t])] for t in tests]

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python3 process_csv.py [output_csv_path]")
        sys.exit(1)
    num_runs = (1, 2, 3)
    thresholds = (10, 100, 1000)
    filename = lambda runs, threshold: f'out-{runs}-{threshold}.csv'
    output_file = sys.argv[1]
    header = ['test', 'original']
    tests = get_tests('out-1-10.csv')
    for threshold in thresholds:
        for run in num_runs:
            out_csv = filename(run, threshold)
            stat = get_mean_std(out_csv)
            header += [f'runs:{run}+thresh:{threshold}']
            tests = [row + [str(stat[row[0]])] for row in tests]
    with open(output_file, 'w') as f:
        f.write(', '.join(header) + '\n')
        for row in tests:
            f.write(', '.join(row) + '\n')
