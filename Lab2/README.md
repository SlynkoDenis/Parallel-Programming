# Lab Work 2

### Task:
* Implement multi-thread numerical integration scheme for the given integral.

![](images/int.png?raw=true)
### Implementation details
* The Simpson's rule (also known as 1/3 rule) was chosen for the solution:

![](images/simpsons_rule.png?raw=true)

with the cumulative error bounded by 

![](images/error.png?raw=true)

* One-thread and multi-thread implementations are provided for the convenient comparison. For both approaches it is necessary to provide epsilon, which will constrain the computations error.

* For multi-thread implementation the variable-length integration step was used for the efficient load of all threads and the better performance. On the contrary, the fixed-length integration grid was used for one-thread implementation for showing disadvantages of naive solution.

* Grid points are distributed within threads in a way to balance computations' load equally.

* Example of time elapsed by both implementations<sup>(1)</sup> with 8 threads for multi-thread one:

| epsilon | one-thread (s) | multi-thread (s) |
| :---: | :---: | :---: |
| 1.0e-6 | 2.6408 | 0.0297 |
| 1.0e-7 | 26.2282 | 0.2582 |

<sub>Result were obtained on device with: Intel(R) Core(TM) i5-8265U CPU @ 1.60GHz</sub>

