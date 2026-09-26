import os
import tkinter as tk
from tkinter import messagebox

import matplotlib

matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

from cal import calculate_distance, calculate_polynomial

DATASET_FILE = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "dataset.csv"
)

GRAPH_MIN_X = 0.0
GRAPH_MAX_X = 1000.0
GRAPH_STEP = 50.0

PLACEHOLDER_TEXT = "Enter Speed(km/h)"

class App:
    def __init__(self, root):
        self.root = root
        root.title("Finding Break Distance")
        root.geometry("1024x768")

        self.coeffs = calculate_polynomial(DATASET_FILE)

        frame = tk.Frame(root)
        frame.place(relx=0.5, rely=0.5, anchor="center")

        self.entry = tk.Entry(frame, width=30)
        self.entry.grid(row=0, column=0, pady=10)
        self._show_placeholder()

        calc_button = tk.Button(frame, text="calculate", command=self.on_submit)
        calc_button.grid(row=1, column=0, pady=5)

        graph_button = tk.Button(frame, text="Graph", command=self.on_graph)
        graph_button.grid(row=2, column=0, pady=5)

    def _show_placeholder(self):
        self.entry.insert(0, PLACEHOLDER_TEXT)
        self.entry.config(fg="grey")
        self.entry.bind("<FocusIn>", self._clear_placeholder)

    def _clear_placeholder(self, _event):
        if self.entry.get() == PLACEHOLDER_TEXT:
            self.entry.delete(0, tk.END)
            self.entry.config(fg="black")

    def _entered_speed(self):
        text = self.entry.get()
        if text == PLACEHOLDER_TEXT:
            text = ""
        try:
            return float(text)
        except ValueError:
            return None

    def on_submit(self):
        if self.coeffs is None:
            messagebox.showinfo("Finding Break Distance", "can't load dataset.")
            return

        speed = self._entered_speed()
        if speed is None or speed < 0:
            messagebox.showinfo(
                "Finding Break Distance", "please enter a valid speed."
            )
            return

        a0, a1, a2 = self.coeffs
        distance = calculate_distance(speed, a0, a1, a2)

        messagebox.showinfo(
            "Finding Break Distance",
            f"speed: {speed:.2f} km/h\nbreaking distance: {distance:.2f} m",
        )

    def on_graph(self):
        if self.coeffs is None:
            messagebox.showinfo("Finding Break Distance", "can't load dataset.")
            return

        a0, a1, a2 = self.coeffs

        graph_window = tk.Toplevel(self.root)
        graph_window.title("Equation Graph")
        graph_window.geometry("1024x768")

        xs = []
        ys = []
        x = GRAPH_MIN_X
        while x <= GRAPH_MAX_X:
            xs.append(x)
            ys.append(calculate_distance(x, a0, a1, a2))
            x += GRAPH_STEP

        fig = Figure(figsize=(9, 6.5), dpi=100)
        ax = fig.add_subplot(111)

        ax.plot(xs, ys, color="#1a59cc", linewidth=2.5, zorder=2)
        ax.scatter(xs, ys, color="#cc2626", s=25, zorder=3)
        ax.axhline(0, color="#262626", linewidth=1.5, zorder=1)
        ax.axvline(0, color="#262626", linewidth=1.5, zorder=1)
        ax.grid(True, color="#d9d9d9", linewidth=0.5)
        ax.set_xlabel("Speed")
        ax.set_ylabel("Distance")
        ax.set_title(f"y = {a0:.4f} + {a1:.4f}x + {a2:.4f}x^2")

        canvas = FigureCanvasTkAgg(fig, master=graph_window)
        canvas.draw()
        canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

def main():
    root = tk.Tk()
    App(root)
    root.mainloop()

if __name__ == "__main__":
    main()
