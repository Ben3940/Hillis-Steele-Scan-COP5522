# Repo Description

This repo contains code for a sequential prefix sum algorithm and parallel Hillis-Steele scan. The Hillis-Steele scan implements _Message Passing Interface_ (MPI), both programs are written in C. The code associated with each algorithm can be found in the respective folder. A makefile is provided to simplify the process of compiling the main.c file locally.

# Prerequisites

1. You need to have MPI installed on your machine to run the Hillis-Steele Scan algorithm

# How to compile prefix-scan algorithm

1. Open a terminal and navigate to Sequential-Inclusive-Scan directory
2. Inside the folder run (this will compile main.c file)
   ```shell
       $ make
   ```
3. A _main_ executable will be created inside the folder. Run
   ```shell
      $ ./main 10
   ```
   NOTE: replace 10 with your preferred array size for input array

# How to compile Hillis-Steele algorithm

1. Open a terminal and navigate to Hillis-Steele-Scan directory
2. Inside the folder run (this will compile main.c file)
   ```shell
       $ make
   ```
3. A _main_ executable will be created inside the folder. Run:

   ```shell
       $ mpiexec -n 2 ./main 10
   ```

   NOTE: replace 2 with your preferred thread counts and 10 for input array size
