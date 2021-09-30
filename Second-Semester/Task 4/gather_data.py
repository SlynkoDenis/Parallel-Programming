import subprocess
import matplotlib.pyplot as plt
import numpy as np


class ProcessException(Exception):
    pass


def execute(cmd):
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    out, err = process.communicate()
    if process.returncode:
        print("OUT:", out.decode('utf-8'))
        print("ERR:", err.decode('utf-8'))
        raise ProcessException(err.decode('utf-8'))
    return out, err


def main():
    # cmd = "./run.sh -not {} 1000 1000 1000"
    # print([(i, float(execute(cmd.format(i))[0].decode('utf-8'))) for i in range(1, 13)])
    fig = plt.figure()

    x_axis = np.arange(1, 13)
    y_value = []
    y_value.append(np.array([10.048609, 5.021893, 3.398005, 2.765696, 2.7112, 2.832817, 2.902778, 3.031799, 3.019096, 3.037752, 3.099073, 3.053151]))
    # y_value.append(np.array([9.776898, 5.422174, 3.651225, 3.170129, 2.950706, 3.173022, 3.129892, 3.271507, 3.24294, 3.342911, 3.853322, 3.6988440000000002]))
    y_value.append(np.array([9.609091, 5.288082, 3.565477, 3.088248, 2.907191, 3.05833, 3.076188, 3.228596, 3.199158, 3.217045, 3.328879, 3.695413]))
    y_value.append(np.array([9.708491, 4.967823, 3.409603, 2.6595940000000002, 2.747761, 2.969624, 3.049344, 2.976046, 2.955558, 2.995483, 3.0769, 3.613039]))
    # y_value.append(np.array([10.2968, 5.318334, 3.57271, 2.813593, 3.285893, 2.924973, 3.020633, 3.041489, 3.103762, 3.112321, 3.633049, 3.434981]))
    y_value.append(np.full(shape=12, fill_value=2.74172800000000016496))

    ax = fig.add_subplot(111)
    ax.plot(x_axis, y_value[0], label="dynamic [250]", marker='o', markersize=1)
    ax.plot(x_axis, y_value[1], label="runtime", marker='o', markersize=1)
    ax.plot(x_axis, y_value[2], label="guided", marker='o', markersize=1)
    ax.plot(x_axis, y_value[3], label="static [250] - only for 4 threads", marker='o', markersize=1)

    ax.set_title("Performance of matrices multiplication depending on number of threads and type of scheduling", fontsize=20)

    ax.set_xlabel("Number of threads", fontsize=16)
    ax.set_ylabel("Time elapsed, s", fontsize=16)
    ax.legend(fontsize=20)
    ax.grid()

    fig.set_figheight(20)
    fig.set_figwidth(25)
    fig.savefig("comparison.png")


if __name__ == '__main__':
    main()
