#!/usr/bin/python3

import matplotlib.pyplot as plt
import argparse
import numpy as np
import csv
import os


def get_grid(N, step):
    return [x * step for x in range(0, N)]


parser = argparse.ArgumentParser()
parser.add_argument("--dir", type=str,
                    help="Plot graph for data file in directory")
cmd_args = parser.parse_args()

path_to_csv = os.path.join(os.path.dirname(os.path.realpath(__file__)), cmd_args.dir, 'data.csv')
if not os.path.isfile(path_to_csv):
    raise ValueError(f"unknown path: {path_to_csv}")

values = []
with open(path_to_csv) as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in spamreader:
        values += [float(x) for x in row]

h = values[-1]
M = int(values[-2])
tau = values[-3]
K = int(values[-4])
del values[-4:]

x = get_grid(M, h)
t = get_grid(K, tau)
X, T = np.meshgrid(x, t)
U = np.array(values).reshape(X.shape)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(X, T, U)

ax.set_xlabel('coordinate x')
ax.set_ylabel('time t')
ax.set_zlabel('u(t, x)')

fig.set_figheight(20)
fig.set_figwidth(25)

fig.savefig(os.path.join(os.path.dirname(os.path.realpath(__file__)), cmd_args.dir, "graph.png"))
