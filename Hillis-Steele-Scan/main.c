#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "microtime.h"

void init_zero_array(int *arr, int size)
{
    for (int i = 0; i < size; i++)
        arr[i] = 0;
}

void init_input_array(int *arr, int size)
{
    for (int i = 0; i < size; i++)
        arr[i] = i + 1;
}

void copy_array_values(int *input, int *output, int size)
{
    for (int i = 0; i < size; i++)
        output[i] = input[i];
}

void print_array_elements(char *label, int *arr, int size)
{
    printf("%s \t [", label);
    for (int i = 0; i < size; i++)
        printf("%3i", arr[i]);
    printf("  ]\n");
}

void hillis_steele_scan(int argc, char **argv, int *input, int *output, int size, MPI_Comm comm)
{
    int rank, num_procs;
    int step = 0;
    int nth_neighbor_to_left = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Status status;
    int index = rank;

    // Calculate how many elements each process is responsible for
    int send_count = size / num_procs;
    int *temp = malloc(sizeof(int) * send_count); // Correct allocation for each process

    // Calculate number of steps for the scan operation (log2(size))
    int log_n = ceil(log(size) / log(2));

    while (step <= log_n + 1)
    {
        MPI_Barrier(comm);
        int local_index = 0;
        index = rank;

        // Each process handles its segment of the array
        while (index < size)
        {
            int current_value = output[index];
            if (index - nth_neighbor_to_left >= 0)
            {
                int neighbor_value = output[index - nth_neighbor_to_left];
                temp[local_index] = current_value + neighbor_value;
            }
            else
            {
                temp[local_index] = current_value;
            }
            // Move to the next index in the current process' segment
            index += num_procs;
            local_index++;
        }
        step++;
        nth_neighbor_to_left *= 2;

        // Now gather the results
        if (rank == 0)
        {
            // Process 0 copies its local results into the final output array
            for (int i = 0; i < send_count; i++)
            {
                output[i * num_procs] = temp[i];
            }

            // Process 0 receives results from other processes
            for (int sender = 1; sender < num_procs; sender++)
            {
                for (int i = 0; i < send_count; i++)
                {
                    int val = 0;
                    MPI_Recv(&val, 1, MPI_INT, sender, 0, comm, &status);
                    output[sender + (i * (num_procs))] = val;
                }
            }
        }
        else
        {
            // Other processes send their results to process 0
            for (int i = 0; i < send_count; i++)
            {
                MPI_Send(&temp[i], 1, MPI_INT, 0, 0, comm);
            }
        }

        // Broadcast the updated output array to all processes
        MPI_Bcast(output, size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Only process 0 prints the result arrays
    if (rank == 0)
    {
        //print_array_elements("Input Array", input, size);
        //print_array_elements("Output Array", output, size);
    }

    MPI_Finalize();
    free(temp); // Free dynamically allocated memory
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("ERROR\n");
        printf("USAGE: ./main <number_input_elements>\n");
        return 1;
    }

    double time1, time2, t;
    int rank, num_procs;

    int size = atoi(argv[1]);
    int *input = malloc(sizeof(int) * size);
    int *output = malloc(sizeof(int) * size);

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Memory allocation failed for input or output array\n");
        return 1;
    }

    // Initialize the input array with values 1 to n
    init_input_array(input, size);
    // Copy the input array to the output array
    copy_array_values(input, output, size);

    // Measure the execution time of the Hillis-Steele scan
    time1 = microtime();
    hillis_steele_scan(argc, argv, input, output, size, MPI_COMM_WORLD);
    time2 = microtime();
    t = time2 - time1;

    printf("\nTime = %g us\n", t);
    printf("Timer Resolution = %g us\n", getMicrotimeResolution());

    // Free dynamically allocated memory
    free(input);
    free(output);
    return 0;
}
