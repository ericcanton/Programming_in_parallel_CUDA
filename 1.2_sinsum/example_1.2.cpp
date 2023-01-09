/*
    Example 1.2, compute integral of sin(x) over 0 <= x <= pi, via trapezoid rule and Taylor approximation.
    Version 2: uses multiple CPU threads via OpenMP.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "cxtimers.h"

inline float sinsum(float x, int terms) {
    float term = x;
    float sum = term;
    float x2 = x * x;
    for (int n=1; n < terms; n++) {
        term *= -x2 / (float) (2 * n * (2*n + 1));
        sum += term;
    }

    return sum;
}

int main(int argc, char *argv[]) {
    int steps = (argc > 1) ? atoi(argv[1]) : 1000000;
    int terms = (argc > 2) ? atoi(argv[2]) : 1000;
    int threads = (argc > 3) ? atoi(argv[3]) : 4;

    omp_set_num_threads(threads);

    double pi = 3.14159265358979323;
    double step_size = pi/(steps - 1);

    cx::timer timr;
    double omp_sum = 0.0;

    #pragma omp parallel for reduction (+:omp_sum)
    for (int step = 0; step < steps; step++) {
        float x = step_size * step;
        omp_sum += sinsum(x, terms); // Sum of Taylor series
    }
    double omp_time = timr.lap_ms();

    // Trapezoid rule correction
    omp_sum -= 0.5 * (sinsum(0.0, terms) + sinsum(pi, terms));
    omp_sum *= step_size;
    printf("omp sum = %.10f\n threads: %d\n steps: %d\n terms %d\n time: %.3f ms\n",
            omp_sum, threads, steps, terms, omp_time);
    return 0;
}