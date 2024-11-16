#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

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

void print_array_elements(char *label, int *arr, int size)
{
    printf("%s \t [", label);
    for (int i = 0; i < size; i++)
        printf("%3i", arr[i]);
    printf("  ]\n");
}

void hillis_steele_scan(int *input, int *output, int size, MPI_Comm comm)
{
    int offset = 0;
    int nth_neighbor_to_left = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    int index = 0;
    // If we can look over n neighbors to left and be under array size
    //   then we can add that value to current index value
    while (nth_neighbor_to_left < size)
    {

        /**
         * Need way to denote when we have scanned over array with a given nieghbor size.
         * Signifying when to increment nth_neighbor_to_left and scan over array again
         * until all elements are computed in final array
         */

        // offset to impose "sliding" of threads across array elements
        index = rank + offset;

        // If thread's current index is less than array size, it should compute
        if (index < size)
        {
            int current_value = input[index];
            if (index - nth_neighbor_to_left > 0)
            {
                int neighbor_value = input[index - nth_neighbor_to_left];
                output[index] = current_value + neighbor_value;
            }
            else
            {
                output[index] = current_value;
            }
        }

        /**
         * Rank 0 should increase offset by number of threads after a full scan over array
         * for a given nth_neighbor_to_left
         */
        if (rank == 0)
        {
        }

        // synchronize processes
        MPI_Barrier(comm);
    }

    MPI_Finalize();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("ERROR\n");
        printf("USAGE: ./main <number_input_elements>\n");
        return 1;
    }

    int rank, num_procs;

    int size = atoi(argv[1]);
    int *input = malloc(sizeof(int) * size);
    int *output = malloc(sizeof(int) * size);

    init_input_array(input, size);
    init_zero_array(output, size);

    hillis_steele_scan(input, output, size, MPI_COMM_WORLD);

    print_array_elements("Input Array", input, size);
    print_array_elements("Output Array", output, size);

    free(input);
    free(output);
    return 0;
}