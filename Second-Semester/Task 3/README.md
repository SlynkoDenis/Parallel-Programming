# Array Transformation with OpenMP

Program takes consecutively seeded array of 100 000 elements and modifies it in a way that elements
are calculated as `new a[i] = (a[i - 1] * a[i] * a[i + 1]) / 3`.
Only small buffer array is created, modifications are performed on the existing array -
**\* task condition is satisfied**.

Use either CMakeLists.txt or combination of build.sh & run.sh scripts
(run.sh script is required to be used with `-not` option denoting number of threads).
`generate_data.sh` is used to generate `.dat` file with target values, which are used to check calculations
from main program.
