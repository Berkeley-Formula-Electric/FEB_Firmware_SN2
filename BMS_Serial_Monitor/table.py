import tkinter as tk, constants, data

class Table:
    CELL_WIDTH = 8
    CELL_HEIGHT = 1
    STATISTICS = ["std_deviation", "range", "mean", "min", "max"]
    DEFAULT_COLOR = "systemWindowBackgroundColor"
    INVALID_RANGE_COLOR = "#fc5c47"
    DISCHARGE_COLOR = "#326ba8"

    def __init__(self, win):
        self.win = win
        self.cells = dict()             # (bank: int, cell: int, type: str) -> tk.Label
        self.information = dict()       # (str, str) -> tk.Label
        self.volt_data = data.Data()
        self.temp_data = data.Data()

        # modes
        self.discharge = False
        self.range = False

        self.frame = self.__setup()
        self.__create_table()
        self.__create_information_table()
        self.__bind_key_press()

    def __setup(self):
        frame = tk.Frame(self.win.root)
        frame.pack()
        return frame
    
    def __create_table(self):
        self.__set_row_header()
        self.__set_column_header()
        self.__set_body()
    
    def __set_row_header(self):
        tk.Label(self.frame, text="Cell", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT*2, bd=1, relief="solid").grid(row=0, column=0, rowspan=2, sticky="nesw")
        for r in range(2, constants.NUM_CELLS_PER_BANK + 2):
            tk.Label(self.frame, text=str(r-1), borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=r, column=0, sticky="nesw")

    def __set_column_header(self):
        for c in range(1, constants.NUM_BANKS + 1):
            tk.Label(self.frame, text=f"Bank {c}", borderwidth=1, width=Table.CELL_WIDTH*2, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=0, column=c*2-1, columnspan=2, sticky="nesw")
            tk.Label(self.frame, text="V", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=1, column=c*2-1, sticky="nesw")
            tk.Label(self.frame, text="T", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=1, column=c*2, sticky="nesw")

    def __set_body(self):
        for b in range(constants.NUM_BANKS):
            for c in range(constants.NUM_CELLS_PER_BANK):
                volt_label = tk.Label(self.frame, text="V", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid")
                temp_label = tk.Label(self.frame, text="T", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid")

                volt_row, volt_column = self.__get_voltage_position(b, c)
                temp_row, temp_column = self.__get_temperature_position(b, c)
                volt_label.grid(row=volt_row, column=volt_column, sticky="nesw")
                temp_label.grid(row=temp_row, column=temp_column, sticky="nesw")
				
                self.cells[(b, c, "V")] = volt_label
                self.cells[(b, c, "T")] = temp_label
    
    def __get_voltage_position(self, bank, cell):
        return (cell + 2, bank * 2 + 1)
    
    def __get_temperature_position(self, bank, cell):
        return (cell + 2, bank * 2 + 2)
    
    def __create_information_table(self):
        self.__set_empty_row()
        self.__set_information_row_header()
        self.__set_information_column_header()
        self.__set_information_body()
    
    def __set_empty_row(self):
        row = constants.NUM_CELLS_PER_BANK + 2
        for c in range(0, constants.NUM_BANKS * 2 + 1):
            tk.Label(self.frame, text="", width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT).grid(row=row, column=c)
        
    def __set_information_row_header(self):
        row_start = constants.NUM_CELLS_PER_BANK + 3
        tk.Label(self.frame, text="Type", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row_start, column=0, sticky="nesw")
        tk.Label(self.frame, text="V", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row_start+1, column=0, sticky="nesw")
        tk.Label(self.frame, text="T", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row_start+2, column=0, sticky="nesw")

    def __set_information_column_header(self):
        row = constants.NUM_CELLS_PER_BANK + 3
        column_start = 1
        
        tk.Label(self.frame, text="Std. Deviation", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start, columnspan=2, sticky="nesw")
        tk.Label(self.frame, text="Range", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start+2*1, columnspan=2, sticky="nesw")
        tk.Label(self.frame, text="Mean", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start+2*2, columnspan=2, sticky="nesw")
        tk.Label(self.frame, text="Min", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start+2*3, columnspan=2, sticky="nesw")
        tk.Label(self.frame, text="Max", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start+2*4, columnspan=2, sticky="nesw")
        
        tk.Label(self.frame, text="Monitor Mode", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start+2*6, columnspan=2, sticky="nesw")
        tk.Label(self.frame, text="BMS State", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid").grid(row=row, column=column_start+2*7, columnspan=2, sticky="nesw")

    def __set_information_body(self):
        column_start = 1
        row_start = constants.NUM_CELLS_PER_BANK + 4
        
        for i in range(len(Table.STATISTICS)):
            volt_label = tk.Label(self.frame, text="V", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid")
            temp_label = tk.Label(self.frame, text="T", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid")

            volt_label.grid(row=row_start, column=column_start+2*i, columnspan=2, sticky="nesw")
            temp_label.grid(row=row_start+1, column=column_start+2*i, columnspan=2, sticky="nesw")

            self.information[Table.STATISTICS[i] + "_volt"] = volt_label
            self.information[Table.STATISTICS[i] + "_temp"] = temp_label

        monitor_mode_label = tk.Label(self.frame, text="standard", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid")
        bms_state_label = tk.Label(self.frame, text="standard", borderwidth=1, width=Table.CELL_WIDTH, height=Table.CELL_HEIGHT, bd=1, relief="solid")

        monitor_mode_label.grid(row=row_start, column=column_start+2*6, rowspan=2, columnspan=2, sticky="nesw")
        bms_state_label.grid(row=row_start, column=column_start+2*7, rowspan=2, columnspan=2, sticky="nesw")

        self.information["monitor_mode"] = monitor_mode_label
        self.information["bms_state"] = bms_state_label

    def __bind_key_press(self):
        self.win.root.bind("<KeyPress>", self.__key_press)

    def __key_press(self, e):
        match e.char:
            case 's':
                if self.discharge or self.range:
                    self.discharge = False
                    self.range = False
                    self.__reset_cells()
                    self.information["monitor_mode"].configure(text="standard")
            case 'd':
                if not self.discharge:
                    self.discharge = True
                    self.range = False
                    self.__reset_cells()
                    self.__setup_discharge()
                    self.information["monitor_mode"].configure(text="discharge")
            case 'r':
                if not self.range:
                    self.discharge = False
                    self.range = True
                    self.__reset_cells()
                    self.__setup_range()
                    self.information["monitor_mode"].configure(text="range")
                
    def __reset_cells(self):
        for b in range(constants.NUM_ACTIVE_BANKS):
            for c in range(constants.NUM_CELLS_PER_BANK):
                self.cells[(b, c, "V")].configure(bg=Table.DEFAULT_COLOR)
                self.cells[(b, c, "T")].configure(bg=Table.DEFAULT_COLOR)
    
    def __setup_discharge(self):
        pass

    def __setup_range(self):
        for b in range(constants.NUM_ACTIVE_BANKS):
            for c in range(constants.NUM_CELLS_PER_BANK):
                cell_volt = self.cells[(b, c, "V")]
                cell_temp = self.cells[(b, c, "T")]
                voltage = float(cell_volt.cget("text"))
                temperature = float(cell_temp.cget("text"))

                if not self.__valid_voltage(voltage):
                    cell_volt.configure(bg=Table.INVALID_RANGE_COLOR)
                if not self.__valid_temperature(temperature):
                    cell_temp.configure(bg=Table.INVALID_RANGE_COLOR)
    
    def __valid_voltage(self, value):
        return constants.MIN_VOLTAGE <= value <= constants.MAX_VOLTAGE
    
    def __valid_temperature(self, value):
        return constants.MIN_TEMPERATURE <= value <= constants.MAX_TEMPERATURE
    
    def update_value(self, value, bank, cell, type):
        cell_label = self.cells[(bank, cell, type)]
        cell_label.configure(text=str(round(value, 3)))
        
        if type == "V":
            self.volt_data.update_value(value, bank, cell)
            if self.range:
                cell_color = Table.DEFAULT_COLOR
                if not self.__valid_voltage(value):
                    cell_color = Table.INVALID_RANGE_COLOR
                cell_label.configure(bg=cell_color)
        elif type == "T":
            self.temp_data.update_value(value, bank, cell)
            if self.range:
                cell_color = Table.DEFAULT_COLOR
                if not self.__valid_temperature(value):
                    cell_color = Table.INVALID_RANGE_COLOR
                cell_label.configure(bg=cell_color)

    def update_statistics(self):
        self.volt_data.calculate()
        self.temp_data.calculate()
        for s in Table.STATISTICS:
            self.information[s + "_volt"].configure(text=str(round(self.volt_data.get_value(s), 3)))
            self.information[s + "_temp"].configure(text=str(round(self.temp_data.get_value(s), 3)))

    def update_discharge(self, state, bank, cell):
        if not self.discharge:
            return
        cell_color = Table.DISCHARGE_COLOR if state else Table.DEFAULT_COLOR
        self.cells[(bank, cell, "V")].configure(bg=cell_color)
