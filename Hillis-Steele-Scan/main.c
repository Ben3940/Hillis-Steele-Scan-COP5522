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

void copy_array_values(int *input, int *output, int size)
{
    for (int i = 0; i < size; i++)
        output[i] = input[i];
};

void print_array_elements(char *label, int *arr, int size)
{
    printf("%s \t [", label);
    for (int i = 0; i < size; i++)
        printf("%3i", arr[i]);
    printf("  ]\n");
}

void hillis_steele_scan(int argc, char **argv, int *input, int *output, int *temp, int size, MPI_Comm comm)
{
    int rank, num_procs;
    int nth_neighbor_to_left = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    int index = rank;
    while (nth_neighbor_to_left < size)
    {
        while (index < size)
        {
            int current_value = output[index];
            if (index - nth_neighbor_to_left >= 0)
            {
                int neighbor_value = output[index - nth_neighbor_to_left];
                output[index] = current_value + neighbor_value;
            }
            else
            {
                output[index] = current_value;
            }
            // offset to impose "sliding" of threads across array elements
            index += num_procs;
        }

        nth_neighbor_to_left++;

        // synchronize processes
        MPI_Barrier(comm);
    }
    print_array_elements("Input Array", input, size);
    print_array_elements("Output Array", output, size);

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
    int *temp = malloc(sizeof(int) * size);

    init_input_array(input, size);
    copy_array_values(input, output, size);

    hillis_steele_scan(argc, argv, input, output, temp, size, MPI_COMM_WORLD);

    free(input);
    free(output);
    free(temp);
    return 0;
}