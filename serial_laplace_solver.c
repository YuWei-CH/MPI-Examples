#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

// size of plate
#define COLUMNS 1000
#define ROWS 1000

// Largest difference in temperature that is allowed to continue the iteration
#define MAX_TEMP_ERROR 0.001

double Temperature[ROWS + 2][COLUMNS + 2];      // Temperature grid
double Temperature_last[ROWS + 2][COLUMNS + 2]; // Temperature grid from last iteration

// helper function
void initialize();
void track_progress(int iter);

int main(int argc, char *argv[])
{
    int i, j;                                           // grid indexes
    int max_iterations;                                 // number of iterations
    int iteration = 1;                                  // current iteration
    double dt = 100;                                    // largest change in t
    struct timeval start_time, stop_time, elapsed_time; // timers

    printf("Maximum iterations [100-40000]?\n");
    scanf("%d", &max_iterations);

    gettimeofday(&start_time, NULL); // unix timer
    initialize();                    // initialize Temp_last including boundary conditions

    // do until error is minimal or until max steps
    while (dt > MAX_TEMP_ERROR && iteration <= max_iterations)
    {
        // main calculation: average my four neighbors
        for (i = 1; i <= ROWS; i++)
        {
            for (j = 1; j <= COLUMNS; j++)
            {
                Temperature[i][j] = 0.25 * (Temperature_last[i + 1][j] + Temperature_last[i - 1][j] + Temperature_last[i][j + 1] + Temperature_last[i][j - 1]);
            }
        }

        dt = 0.0; // reset largest temperature change
        // copy grid to old grid for next iteration and find latest dt
        for (i = 1; i <= ROWS; i++)
        {
            for (j = 1; j <= COLUMNS; j++)
            {
                dt = fmax(fabs(Temperature[i][j] - Temperature_last[i][j]), dt); // find the max difference of the temperature
                Temperature_last[i][j] = Temperature[i][j];
            }
        }

        // periodically print test values
        if ((iteration % 100) == 0)
        {
            track_progress(iteration);
        }

        iteration++;
    }

    gettimeofday(&stop_time, NULL);
    timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
    printf("\nMax error at iteration %d was %f\n", iteration - 1, dt);
    printf("Total time was %f seconds.\n", elapsed_time.tv_sec + elapsed_time.tv_usec / 1000000.0);
}

// Initialize plate and boundary conditions
// Temp_last is used to to start the iteration
void initialize()
{
    int i, j;
    for (i = 0; i <= ROWS + 1; i++)
    {
        for (j = 0; j <= COLUMNS + 1; j++)
        {
            Temperature_last[i][j] = 0.0;
        }
    }

    // these boundary conditions never change throughout run
    // set left side to 0 and right side to a linear increase
    for (i = 0; i < ROWS + 1; i++)
    {
        Temperature_last[i][0] = 0.0;
        Temperature_last[i][COLUMNS + 1] = (100.0 / ROWS) * i;
    }

    // set top to 0 and bottom to linear increase
    for (j = 0; i < COLUMNS + 1; i++)
    {
        Temperature_last[0][j] = 0.0;
        Temperature_last[ROWS + 1][j] = (100.0 / COLUMNS) * j;
    }
}

// Print diagonal in bottom right corner where most actions is. For debugging.
void track_progress(int iteration)
{
    int i;
    printf("---------- Iteration number: %d ----------\n", iteration);
    for (i = ROWS - 5; i <= ROWS; i++)
    {
        printf("[%d,%d]: %5.2f  ", i, i, Temperature[i][i]);
    }
    printf("\n");
}
