# Lab Work 1

### Task:
* Implement one-thread solver of convection–diffusion equation for a given velocity and source of sinks function.

![](images/equation.png?raw=true)
* With the use of MPI develop a multi-thread program, which solves the equation given in the first sub-task.
### Implementation details
* The leap-frog scheme was chosen for the both solutions:
  
![](images/scheme.png?raw=true)
![](images/formula.png?raw=true)

* For instance with 100000 time points and 2000 coordinate points the elapsed time for one-thread and multi-thread programs equals 2.45s and 0.97s respectively.

* The solution graph for instance of the task with `a = 1`, `f = x * t`, `phi(x) = x` and `psi(x) = -t` is shown below

![](images/graph.png?raw=true)

* For instances with the large amount of points (for example, 10000 x 2000) matplotlib.pyplot works very slow, thus plotting consumes most of the time.
