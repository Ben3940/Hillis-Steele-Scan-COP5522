#include <stdio.h>
#include <stdlib.h>
#include <microtime.h>

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

void compute_inclusive_scan(int *input, int *output, int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += input[i];
        output[i] = sum;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("ERROR\n");
        printf("USAGE: ./main <number_input_elements>\n");
        return 1;
    }

    int size = atoi(argv[1]);
    int *input = malloc(sizeof(int) * size);
    int *output = malloc(sizeof(int) * size);
    double t, time1, time2;

    init_input_array(input, size);
    init_zero_array(output, size);

    time1 = microtime();

    compute_inclusive_scan(input, output, size);

    time2 = microtime();
    t = time2 - time1;

    // Print results
    printf("\nTime = %g us\n", t);
    printf("Timer Resolution = %g us\n", getMicrotimeResolution());

    print_array_elements("Input Array", input, size);
    print_array_elements("Output Array", output, size);

    free(input);
    free(output);

    return 0;
}