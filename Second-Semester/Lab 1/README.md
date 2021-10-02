# Lab Work 1

## Task

Implement parallel version of the `sequential_main.c` program using MPI.
Algorithm working on `P` nodes divides matrix's rows into `P` and computes the elements
independently. After that the root reduces data from side nodes and validates result
with the one computed in sequential program.

## Speedup Factor

Assume that we have matrix of size `N x M`. Then the sequential runtime will equal

![T_seq](https://latex.codecogs.com/svg.latex?T_{seq}=N*M*t,)

where t denotes time necessary for one element computation.
The parallel program runtime will be

![T_par](https://latex.codecogs.com/svg.latex?T_{par}=\frac{N*M}{P}+P*t_{rec},)

where t<sub>rec</sub> denotes time to receive one process segment.
Then the speedup factor can be derived as

![spf](https://latex.codecogs.com/svg.latex?SPF=\frac{N*M}{\frac{N*M}{P}+P*\frac{t_{rec}}{t}}.)

As ![relation](https://latex.codecogs.com/svg.latex?t_{rec}\gg{t}) is true, the algorithm
will work faster then one-thread version only for small number of used nodes.

![plot](./speedup.png)
