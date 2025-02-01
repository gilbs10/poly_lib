# Polyomino Counting Library

## Overview

This library provides an efficient algorithm for counting polyominoes of various sizes.

## Building

The project uses CMake for easy compilation. The source code is located in the `src` directory. To build the project:

```sh
mkdir build
cd build
cmake ../src
make
```

This will generate the executable in the `build` directory.

## Usage

After building, run the program from the `build` directory:

```sh
./jensen_diag
```

The program will then prompt for user input to configure the run:

1. **Number of running cases**

   ```
   Enter number of running cases:
   ```

   Specifies how many input configurations to run consecutively. Typically, users enter `1`.

2. **Execution Mode**

   ```
   Run rectangle (r), full size (n) or continue (c)?
   ```

   - `r`: Runs a specific rectangle (details to be provided later).
   - `n`: Runs all rectangles of a given size range, will generate the result for each size.
   - `c`: Continues from a dump file (experimental and potentially unstable).

   The recommended choice for most users is `n`.

3. **Size Range (if ********************`n`******************** was chosen)**

   ```
   Enter sizes from and to:
   ```

   Specifies the range of polyomino sizes to count. For example, to compute counts from size 3 to 10, enter:

   ```
   3 10
   ```

4. **Number of Threads**

   ```
   Enter num of threads to run on:
   ```

   Determines the number of CPU threads to use for computation. Entering a higher value can improve performance on multi-core systems.
