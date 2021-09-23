# Array Transformation with OpenMP

Program takes consecutively seeded array of 100 000 elements and creates an array, where elements
are calculated as `b[i] = (a[i - 1] * a[i] * a[i + 1]) / 3`.

Use either CMakeLists.txt or combination of build.sh & run.sh scripts
(run.sh script is required to be used with `-n` option denoting number of threads).
`generate_data.sh` is used to generate `.dat` file with target values, which are used to check calculations
from main program.
