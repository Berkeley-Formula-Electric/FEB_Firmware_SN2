import matplotlib, matplotlib.animation as animation
import matplotlib.pyplot as plt
from serial_data import Serial_Data

# TEST
import time

matplotlib.use('TkAgg')
plt.rcParams['figure.dpi'] = 110

class Graph:
    def __init__(self, sd: Serial_Data, ui, ti):
        self.__fig, self.__axis = plt.subplots(2,1,figsize=(8, 5))
        self.__serial_data = sd
        self.__update_interval = ui
        self.__time_interval = ti

        # Plot 0
        self.__counter_0 = 0
        self.__x_coords_0 = []
        self.__y_coords_0 = []

        # Plot 1
        self.__counter_1 = 0
        self.__x_coords_1 = []
        self.__y_coords_1 = []

        self.anim = animation.FuncAnimation(self.__fig, self.__animate, interval=self.__update_interval, cache_frame_data=False)
        plt.show()
        plt.close()

    @staticmethod
    def visualize_graph():
        vg = ""
        while vg not in ["y", "n"]:
            vg = input("Visaulize Graph (y/n): ").lower()
        return vg == "y"
    
    def __label_plots(self):
        # Plot 0
        self.__axis[0].set_title("IVT Current")
        self.__axis[0].set_xlabel("Time / s")
        self.__axis[0].set_ylabel("Current / A")

        # Plot 1
        self.__axis[1].set_title("Battery Voltage")
        self.__axis[1].set_xlabel("Time / s")
        self.__axis[1].set_ylabel("Total Voltage / V")

    def __update(self):
        # Clear
        self.__axis[0].clear()
        self.__axis[1].clear()

        # Update
        self.__label_plots()
        self.__axis[0].plot(self.__x_coords_0, self.__y_coords_0)
        self.__axis[1].plot(self.__x_coords_1, self.__y_coords_1)
        plt.tight_layout(h_pad=2)

    def __update_ivt(self):
        if len(self.__x_coords_0) > 1 and self.__x_coords_0[-1] - self.__x_coords_0[0] > self.__time_interval:
            self.__x_coords_0.pop(0)
            self.__y_coords_0.pop(0)
        current = self.__serial_data.ivt_data.get("i1", 0)
        self.__x_coords_0.append(self.__counter_0 * self.__update_interval / 1000)
        self.__y_coords_0.append(current)
        self.__counter_0 += 1

    def __update_voltage(self):
        if len(self.__x_coords_1) > 1 and self.__x_coords_1[-1] - self.__x_coords_1[0] > self.__time_interval:
            self.__x_coords_1.pop(0)
            self.__y_coords_1.pop(0)
        total_voltage = sum(self.__serial_data.voltage.values())
        self.__x_coords_1.append(self.__counter_1 * self.__update_interval / 1000)
        self.__y_coords_1.append(total_voltage)
        self.__counter_1 += 1

    def __animate(self, i):
        self.__update_voltage()
        self.__update_ivt()
        self.__update()