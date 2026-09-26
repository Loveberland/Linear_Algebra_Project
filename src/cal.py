"""Least-squares quadratic fit and breaking-distance calculation.

Port of the original cal.c/cal.h. Given a CSV dataset of (speed, distance)
points, fits y = a0 + a1*x + a2*x^2 using the normal equations solved by
Gauss-Jordan elimination, matching the original algorithm exactly.
"""

MAX_DATA = 128


def _solve(a):
    """Solve a 3x4 augmented matrix in place via Gauss-Jordan elimination."""
    for i in range(3):
        pivot = i
        for r in range(i + 1, 3):
            if abs(a[r][i]) > abs(a[pivot][i]):
                pivot = r

        if pivot != i:
            a[i], a[pivot] = a[pivot], a[i]

        div = a[i][i]
        a[i] = [c / div for c in a[i]]

        for r in range(3):
            if r == i:
                continue
            factor = a[r][i]
            a[r] = [a[r][c] - factor * a[i][c] for c in range(4)]


def calculate_polynomial(filename):
    """Read (x, y) pairs from filename and fit y = a0 + a1*x + a2*x^2.

    Returns an (a0, a1, a2) tuple on success, or None if the file is
    missing or does not contain enough valid data points.
    """
    try:
        with open(filename, "r") as f:
            lines = f.readlines()
    except OSError:
        return None

    x = []
    y = []

    for line in lines[1:]:  # first line is the column header, skip it
        if len(x) >= MAX_DATA:
            break

        parts = line.strip().split(",")
        if len(parts) != 2:
            continue

        try:
            xi = float(parts[0])
            yi = float(parts[1])
        except ValueError:
            continue

        x.append(xi)
        y.append(yi)

    n = len(x)
    if n < 3:
        return None

    sx = sx2 = sx3 = sx4 = 0.0
    sy = sxy = sx2y = 0.0

    for xi, yi in zip(x, y):
        x2 = xi * xi

        sx += xi
        sx2 += x2
        sx3 += x2 * xi
        sx4 += x2 * x2

        sy += yi
        sxy += xi * yi
        sx2y += x2 * yi

    matrix = [
        [float(n), sx, sx2, sy],
        [sx, sx2, sx3, sxy],
        [sx2, sx3, sx4, sx2y],
    ]

    _solve(matrix)

    a0 = matrix[0][3]
    a1 = matrix[1][3]
    a2 = matrix[2][3]

    return (a0, a1, a2)


def calculate_distance(speed, a0, a1, a2):
    return a0 + a1 * speed + a2 * speed * speed
