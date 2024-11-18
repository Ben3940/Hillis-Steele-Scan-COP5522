#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

void hillis_steele_scan(int argc, char **argv, int *input, int *output, int size, MPI_Comm comm)
{
    int rank, num_procs;
    int nth_neighbor_to_left = 1;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Status status;
    int index = rank;
    int send_count = size / num_procs;
    int *temp = malloc((sizeof(int) * size) / num_procs);

    int log_n = ceil(log(size));

    // MPI_Allgather(temp, send_count, MPI_INT, output, send_count, MPI_INT, comm);
    while (nth_neighbor_to_left <= log_n + 1)
    {
        int local_index = 0;
        index = rank;
        while (index < size)
        {
            int current_value = output[index];
            if (index - nth_neighbor_to_left >= 0)
            {
                int neighbor_value = output[index - nth_neighbor_to_left];
                // output[index] = current_value + neighbor_value;
                temp[local_index] = current_value + neighbor_value;
            }
            else
            {
                // output[index] = current_value;
                temp[local_index] = current_value;
            }
            // offset to impose "sliding" of threads across array elements
            index += num_procs;
            local_index++;
        }

        nth_neighbor_to_left++;

        if (rank == 0)
        {
            for (int i = 0; i < send_count; i++)
            {
                output[i * num_procs] = temp[i];
            }

            for (int sender = 1; sender < num_procs; sender++)
            {
                int val = 0;
                for (int i = 0; i < send_count; i++)
                {
                    MPI_Recv(&val, send_count, MPI_INT, sender, 0, comm, &status);
                    output[sender + (i * (num_procs))] = val;
                }
            }
            MPI_Bcast(output, size, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else
        {
            for (int i = 0; i < send_count; i++)
            {
                MPI_Send(&temp[i], send_count, MPI_INT, 0, 0, comm);
            }
        }
    }
    //     // synchronize processes
    //     // MPI_Barrier(comm);
    // }
    if (rank == 0)
    {
        print_array_elements("Input Array", input, size);
        print_array_elements("Output Array", output, size);
    }
    MPI_Finalize();
    free(temp);
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
    copy_array_values(input, output, size);

    hillis_steele_scan(argc, argv, input, output, size, MPI_COMM_WORLD);

    free(input);
    free(output);
    return 0;
}