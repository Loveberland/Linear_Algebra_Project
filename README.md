from pathlib import Path

content = r"""# How to Use

## 1. Clone the repository

```bash
git clone https://github.com/Loveberland/Linear_Algebra_Project.git
cd Linear_Algebra_Project
```

## 2. Install requirements

```bash
python -m pip install -r requirements.txt
```

If your system uses `python3`:

```bash
python3 -m pip install -r requirements.txt
```

## 3. Run the program

```bash
python src/main.py
```

or:

```bash
python3 src/main.py
```

## 4. Calculate breaking distance

1. Enter the speed in the input box.

Example:

```text
80
```

2. Click:

```text
calculate
```

3. The program will display the estimated breaking distance.

Example:

```text
speed: 80.00 km/h
breaking distance: XX.XX m
```

## 5. Show the graph

Click:

```text
Graph
```

The program will open a graph showing the equation.

The graph uses:

- X-axis: Speed
- Y-axis: Distance
- Range: 0 to 1000
- Interval: 50
