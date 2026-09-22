#include <stdio.h>
#include <math.h>

#include "cal.h"

#define MAX_DATA 128

static void solve(double a[3][4]) {
        for (int i = 0; i < 3; ++i) {
                int pivot = i;

                for (int r = i + 1; r < 3; ++r) {
                        if (fabs(a[r][i]) > fabs(a[pivot][i]))
                        pivot = r;
                }

                if (pivot != i) {
                        for (int c = 0; c < 4; ++c) {
                                double tmp = a[i][c];
                                a[i][c] = a[pivot][c];
                                a[pivot][c] = tmp;
                        }
                }

                double div = a[i][i];
                
                for (int c = 0; c < 4; ++c) {
                        a[i][c] /= div;
                }

                for (int r = 0; r < 3; ++r) {
                        if (r == i) {
                                continue;
                        }

                        double factor = a[r][i];
                        for (int c = 0; c < 4; ++c) {
                                a[r][c] -= factor * a[i][c];
                        }
                }
        }
}

int calculate_polynomial(const char *filename, double *a0, double *a1, double *a2) {
        /* read dataset and keep in x and y */
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
                return (0);
        }

        double x[MAX_DATA];
        double y[MAX_DATA];

        int n = 0;
        char line[256];

        fgets(line, sizeof(line), file);        // clear columns name
        while (n < MAX_DATA && fgets(line, sizeof(line), file)) {
                if (sscanf(line, "%lf,%lf", &x[n], &y[n]) == 2) {
                        ++n;
                }
        }

        fclose(file);
        /* end read dataset */

        if (n < 3) {
                return (0);
        }

        double sx = 0.0;
        double sx2 = 0.0;
        double sx3 = 0.0;
        double sx4 = 0.0;

        double sy = 0.0;
        double sxy = 0.0;
        double sx2y = 0.0;

        for (int i = 0; i < n; ++i) {
                double x2 = x[i] * x[i];

                sx += x[i];
                sx2 += x2;
                sx3 += x2 * x[i];
                sx4 += x2 * x2;

                sy += y[i];
                sxy += x[i] * y[i];
                sx2y += x2 * y[i];
        }

        double matrix[3][4] = {
                {n, sx, sx2, sy},
                {sx, sx2, sx3, sxy},
                {sx2, sx3, sx4, sx2y}
        };

        solve(matrix);

        *a0 = matrix[0][3];
        *a1 = matrix[1][3];
        *a2 = matrix[2][3];

        return (1);
}

double calculate_distance(double speed, double a0, double a1, double a2) {
        return a0 + a1 * speed + a2 * speed * speed;
}