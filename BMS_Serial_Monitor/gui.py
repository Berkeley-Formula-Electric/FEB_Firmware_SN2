import tkinter as tk

class GUI:
    WIDTH = 1350
    HEIGHT = 475

    def __init__(self):
        self.root = tk.Tk()
        self.__setup()

    def __setup(self):
        self.root.geometry(f"{GUI.WIDTH}x{GUI.HEIGHT}")
        self.root.title("BMS Serial Monitor")
