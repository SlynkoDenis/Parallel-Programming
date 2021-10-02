import matplotlib.pyplot as plt


x_axis = list(range(2, 13))
seq_time = 0.009093
y_axis = list(seq_time / x for x in [0.015795, 0.007666, 0.0067159, 0.00585, 0.005705, 0.0053699, 0.0053159, 0.0064359, 0.00657399, 0.00617399, 0.007208])

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
