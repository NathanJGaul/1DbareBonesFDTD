/* 1D FDTD simulation of a lossless dielectric region
 * followed by a lossy layer which matches the impedance
 * of the dielectric. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 200
#define LOSS 0.02
#define LOSS_LAYER 180

int main()
{
    FILE *snapshot;

    double ez[SIZE], hy[SIZE - 1], ceze[SIZE], cezh[SIZE], chyh[SIZE - 1], chye[SIZE - 1],  imp0 = 377.0;
    int qTime, maxTime = 450, frame = 0, mm;
    char basename[80] = "sim", filename[100], command[100];

    /* initialize electric field */
    for (mm = 0; mm < SIZE; mm++)
        ez[mm] = 0.0;

    /* initialize magnetic field */
    for (mm = 0; mm < SIZE - 1; mm++)
        hy[mm] = 0.0;

    /* set electric-field update coefficients */
    for (mm = 0; mm < SIZE; mm++) {
        if (mm < 100) { /* free space */
            ceze[mm] = 1.0;
            cezh[mm] = imp0;
        } else if (mm < LOSS_LAYER) {
            ceze[mm] = 1.0;
            cezh[mm] = imp0 / 9.0;
        } else {        /* lossy dialectric */
            ceze[mm] = (1.0 - LOSS) / (1.0 + LOSS);
            cezh[mm] = imp0 / 9.0 / (1.0 + LOSS);
        }
    }

    /* set magnetic-field update coefficients */
    for (mm = 0; mm < SIZE - 1; mm++)
        if (mm < LOSS_LAYER) {
            chyh[mm] = 1.0;
            chye[mm] = 1.0 / imp0;
        } else {
            chyh[mm] = (1.0 - LOSS) / (1.0 + LOSS);
            chye[mm] = 1.0 / imp0 / (1.0 + LOSS);
        }

    /* do time stepping */
    for (qTime = 0; qTime < maxTime; qTime++) {

        /* update magnetic field */
        for (mm = 0; mm < SIZE - 1; mm++)
            hy[mm] = chyh[mm] * hy[mm] +
                        chye[mm] * (ez[mm + 1] - ez[mm]);

        /* correction for Hy adjacent  to TFSF boudary */
        hy[49] -= exp(-(qTime - 30.) * (qTime - 30.) / 100.) / imp0;

        /* use simple ABC */
        ez[0] = ez[1];

        /* update electric field */
        for (mm = 1; mm < SIZE; mm++)
            ez[mm] = ceze[mm] * ez[mm] +
                        cezh[mm] * (hy[mm] - hy[mm - 1]);

        /* correction for Ez adjacent to TFSF boundary */
        ez[50] += exp(-(qTime + 0.5 - (-0.5) - 30.) * 
                       (qTime + 0.5 - (-0.5) - 30.) / 100.);
        
        /* write snapshot if time a multiple of 10 */
        if (qTime % 10 == 0) {
            sprintf(filename, "%s.%03d", basename, frame++);

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
        sprintf(filename, "%s.%03d", basename, mm);
        strcpy(command, "eplot -r [0:200][-1:1] -d -M ");
        strcat(command, filename);
        strcat(command, " 2>/dev/null");
        system(command);
    }

    return 0;
}