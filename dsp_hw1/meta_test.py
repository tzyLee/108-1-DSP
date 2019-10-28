import subprocess
import matplotlib.pyplot as plt

DELTA = 50
MAX = 500

MODEL_COUNT = 5

train_seq_files = [
    'data/train_seq_0{}.txt'.format(m) for m in range(1, MODEL_COUNT+1)]
last_out_files = ['model_init.txt'] * 5
result = []
x = []

it = DELTA
while it <= MAX:
    x.append(it)
    out_files = ['model_0{}_{}.txt'.format(m, it)
                 for m in range(1, MODEL_COUNT+1)]
    processes = []
    for m in range(1, MODEL_COUNT+1):
        p = subprocess.Popen(['./train', str(it), last_out_files[m-1],
                              train_seq_files[m-1], out_files[m-1]])
        processes.append(p)

    for p in processes:
        p.wait()
    last_out_files = out_files

    with open('model_names.txt', 'w+', newline='\n') as f:
        for path in last_out_files:
            f.write(path)
            f.write('\n')

    subprocess.call(['./test', 'model_names.txt',
                     './data/test_seq.txt', 'test_result.txt'])
    same = 0
    with open('test_result.txt') as f1, open('./data/test_lbl.txt') as f2:
        for i, j in zip(f1, f2):
            name = i.split()[0]
            if name[:8] == j.strip()[:8]:
                same += 1

    result.append(same)
    print(same)
    it += DELTA

print(result)

y = [i/2500 for i in result]
plt.plot(x, y)
plt.show()
