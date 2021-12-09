#!/bin/python3
import re
import subprocess
import matplotlib.pyplot as plt


def get_elapsed_time(cmd):
    process = subprocess.run(cmd, shell=True, capture_output=True)
    output = process.stdout.decode('ascii')
    time_elapsed = re.findall(r'Time elapsed ?([0-9]+.[0-9]+)s\n', output)
    return float(time_elapsed[0])


def average_time(cmd):
    average = 0.0
    n = 5
    for _ in range(n):
        average += get_elapsed_time(cmd)
    return average / n


x_axis = list(range(2, 13))
seq_time = 0.0106409
runtimes = list(average_time(f'./run.sh -not {x}') for x in x_axis)
y_axis = list(seq_time / runtimes[i] for i in range(len(runtimes)))

print('Runtimes taken for number of processes in range', x_axis)
print(runtimes)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(x_axis, y_axis, marker='o')

ax.hlines(y=1.0, xmin=2, xmax=12, linewidth=2, color='r')

ax.grid()

ax.set_xlabel('number of processes')
ax.set_ylabel('speedup factor')

ax.set_title('Speedup Factor vs Number of Processes')

fig.set_figheight(20)
fig.set_figwidth(25)

fig.savefig('speedup.png')
