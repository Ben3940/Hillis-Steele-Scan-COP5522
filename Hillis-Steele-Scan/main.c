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
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    // iterate over stages
    for (int step = 1; step < size; step *= 2)
    {
        int temp[size];

        // each process receives values from its preceding process
        for (int i = 0; i < size; i++)
        {
            if (i >= step)
            {
                temp[i] = output[i] + output[i - step];
            }
            else
            {
                temp[i] = output[i];
            }
        }

        // update output with new values from temp
        for (int i = 0; i < size; i++)
        {
            output[i] = temp[i];
        }

        if (rank =)

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