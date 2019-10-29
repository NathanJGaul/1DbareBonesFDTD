/* Bare-bones 1D FDTD simulation with a hard source. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 200

int main()
{
    FILE *snapshot;

    double ez[SIZE] = {0.}, hy[SIZE] = {0.}, imp0 = 377.0;
    int qTime, maxTime = 300, frame = 0, mm;
    char basename[80] = "sim", filename[100], command[100];

    /* do time stepping */
    for (qTime = 0; qTime < maxTime; qTime++) {
        
        /* use simple ABC */
        hy[SIZE - 1] = hy[SIZE - 2];

        /* update magnetic field */
        for (mm = 0; mm < SIZE - 1; mm++)
            hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;

        /* use simple ABC */
        ez[0] = ez[1];

        /* update electric field */
        for (mm = 1; mm < SIZE; mm++)
            ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0;

        /* use additive source at node 50 */
        ez[50] += exp(-(qTime - 30.) * (qTime - 30.) / 100.);

        if (qTime % 10 == 0) {
            sprintf(filename, "%s.%d", basename, frame++);

            /* open file */
            snapshot = fopen(filename, "w");

            /* write data to file */
            for (mm = 0; mm < SIZE; mm++)
                fprintf(snapshot, "%g\t%g\n", ez[mm], hy[mm - 1]);

            /* close file */
            fclose(snapshot);           
        }        
    } /* end of time-stepping */

    /* output simulation to command line */
    for (mm = 0; mm < (maxTime / 10); mm++) {
        sprintf(filename, "%s.%d", basename, mm);
        strcpy(command, "eplot -r [0:200][-1:1] -d -M ");
        strcat(command, filename);
        strcat(command, " 2>/dev/null");
        system(command);
    }

    return 0;
}