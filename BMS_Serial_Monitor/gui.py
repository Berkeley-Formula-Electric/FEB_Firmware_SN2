import tkinter as tk

class GUI:
    WIDTH = 1350
    HEIGHT = 600

    def __init__(self):
        self.root = tk.Tk()
        self.__setup_root()

    def __setup_root(self):
        self.root.geometry(f"{GUI.WIDTH}x{GUI.HEIGHT}")
        self.root.title("BMS Serial Monitor")

