import constants
from graph import Graph
from serial_data import Serial_Data
import tkinter as tk
from VerticalScrolledFrame import VerticalScrolledFrame

class Table:
    CELL_WIDTH = 8
    CELL_HEIGHT = 1

    # Colors
    COLOR_DEFAULT = ""
    COLOR_OUT_OF_RANGE = "#FF0000"

    """
    Atributes:
        __state_cells [dict]: name [str] -> cell [tk.Label]

        __volt_cells [dict]: (bank [int], cell [int]) -> cell [tk.Label]
        __temp_cells [dict]: (bank [int], cell [int]) -> cell [tk.Label]
        __volt_bank_stats_cells [dict]: (bank[int], type [str]) -> cell [tk.Label], processed voltage data for each bank
        __temp_bank_stats_cells [dict]: (bank[int], type [str]) -> cell [tk.Label], processed temperature data for each bank
        __volt_stats_cells [dict]: type [str] -> data [float]
        __temp_stats_cells [dict]: type [str] -> data [float]
        __relay_state_cells [dict]: type [str] -> cell [tk.Label]
        __shutdown_state_cells [dict]: type [str] -> cell [tk.Label]
        __ivt_data_cells [dict]: type [str] -> cell [tk.Label]
        __charger_data_cells [dict]: type [str] -> cell [tk.Label]
        __discharge_data_cells [dict]: type [str] -> cell [tk.Label]
    """
    def __init__(self, root, sd: Serial_Data, ui):
        self.__master = root
        self.__serial_data = sd
        self.__frame = self.__create_table_frame()
        self.__update_interval = ui

        # BMS States
        self.__range_state = False

        # Cells
        self.__state_cells = dict()
        self.__volt_cells = dict()
        self.__temp_cells = dict()
        self.__volt_bank_stats_cells = dict()
        self.__temp_bank_stats_cells = dict()
        self.__volt_stats_cells = dict()
        self.__temp_stats_cells = dict()
        self.__relay_state_cells = dict()
        self.__shutdown_state_cells = dict()
        self.__ivt_data_cells = dict()
        self.__charger_data_cells = dict()
        self.__cell_balance_data_cells = dict()

        self.__setup()
        self.__bind_key_press()

    ###############
    # Key Presses #
    ###############

    def __bind_key_press(self):
        self.__master.bind("<KeyPress>", self.__key_press)

    def __key_press(self, e):
        match e.char:
            case 'r' | 'R':
                self.__range_state = True
                self.__state_cells["monitor"].config(text="Range")
            case 's' | 'S':
                self.__range_state = False
                self.__state_cells["monitor"].config(text="Standard")

    ################
    # Update Table #
    ################

    def update(self):
        self.__update_states_table()
        self.__update_volt_temp_table()
        self.__update_volt_temp_stats_table()
        # self.__update_shutdown_state_table()
        # self.__update_relay_state_table()
        self.__update_IVT_table()
        # self.__update_charger_data_table()
        # self.__update_cell_balance_data_table()

        self.__master.after(self.__update_interval, self.update)
    
    ################
    # Create Table #
    ################

    def __create_table_frame(self):
        frame = VerticalScrolledFrame(self.__master)
        frame.pack(expand = True, fill = tk.BOTH)
        return frame.interior

    def __setup(self):
        row = 0
        self.__create_cell(0, -1, "", border=False, wsf=0.3)
        row = self.__create_blank_row(row)
        row = self.__create_states_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_volt_temp_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_volt_temp_stats_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_shutdown_state_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_relay_state_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_IVT_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_charger_data_table(row)
        row = self.__create_blank_row(row)
        row = self.__create_cell_balance_data_table(row)
        row = self.__create_blank_row(row)

    #####################
    # Controller States #
    #####################

    def __update_states_table(self):
        state_split = []
        for k,v in self.__serial_data.bms_state.items():
            if v == True:
                state_split.append(k)
        state = " ".join(state_split).title() if state_split else "-"
        self.__state_cells["bms"].config(text=state)

    def __create_states_table(self, row):
        # Header
        self.__create_cell(row, 0, "State", rs = 2)
        self.__create_cell(row, 1, "Monitor State", cs = 2)
        self.__create_cell(row, 3, "BMS State", cs = 2)
        row += 1

        # Body
        self.__state_cells["monitor"] = self.__create_cell(row, 1, "Standard", cs=2)
        self.__state_cells["bms"] = self.__create_cell(row, 3, "", cs=2)
        row += 1

        return row
    
    #######################
    # Volt-Temp Cell Data #
    #######################

    def __update_volt_temp_table(self):
        # Voltage
        for bank in range(constants.NUM_BANKS):
            for cell in range(constants.NUM_CELLS_PER_BANK):
                value = self.__serial_data.voltage.get((bank, cell), '-')
                bg = Table.COLOR_DEFAULT
                if value != '-':
                    value = round(value, 3)
                    if self.__range_state and not constants.CELL_VOLTAGE_MIN <= value <= constants.CELL_VOLTAGE_MAX:
                        bg = Table.COLOR_OUT_OF_RANGE
                    elif self.__range_state:
                        green = int((value - constants.CELL_VOLTAGE_MIN) / (constants.CELL_VOLTAGE_MAX - constants.CELL_VOLTAGE_MIN) * 135 + 120) % 256
                        rgb = (0, green, 0)
                        bg = "#%02x%02x%02x" % rgb 
                self.__volt_cells[(bank, cell)].config(text=value, background=bg)

        # Temperature
        for bank in range(constants.NUM_BANKS):
            for cell in range(constants.NUM_CELLS_PER_BANK):
                value = self.__serial_data.temperature.get((bank, cell), '-')
                bg = Table.COLOR_DEFAULT
                if value != '-':
                    value = round(value, 3)
                    if self.__range_state and not constants.CELL_TEMPERATURE_MIN <= value <= constants.CELL_TEMPERATURE_MAX:
                        bg = Table.COLOR_OUT_OF_RANGE
                self.__temp_cells[(bank, cell)].config(text=value, background=bg)

    def __update_volt_temp_stats_table(self):
        # Voltage
        volt_stats = self.__serial_data.voltage.get_stats()
        self.__volt_stats_cells["mean"].config(text=volt_stats["mean"])
        self.__volt_stats_cells["min"].config(text=volt_stats["min"])
        self.__volt_stats_cells["max"].config(text=volt_stats["max"])
        self.__volt_stats_cells["stdev"].config(text=volt_stats["stdev"])
        self.__volt_stats_cells["range"].config(text=volt_stats["range"])
        self.__volt_stats_cells["sum"].config(text=volt_stats["sum"])

        # Bank voltage
        for bank in range(constants.NUM_BANKS):
            bank_volt_stats = self.__serial_data.voltage.get_stats(bank=bank)
            self.__volt_bank_stats_cells[(bank, "mean")].config(text=bank_volt_stats["mean"])
            self.__volt_bank_stats_cells[(bank, "sum")].config(text=bank_volt_stats["sum"])

        # Temperature
        temp_stats = self.__serial_data.temperature.get_stats()
        self.__temp_stats_cells["mean"].config(text=temp_stats["mean"])
        self.__temp_stats_cells["min"].config(text=temp_stats["min"])
        self.__temp_stats_cells["max"].config(text=temp_stats["max"])
        self.__temp_stats_cells["stdev"].config(text=temp_stats["stdev"])
        self.__temp_stats_cells["range"].config(text=temp_stats["range"])
    
        # Bank temperature
        for bank in range(constants.NUM_BANKS):
            bank_temp_stats = self.__serial_data.temperature.get_stats(bank=bank)
            self.__temp_bank_stats_cells[(bank, "mean")].config(text=bank_temp_stats["mean"])

    def __create_volt_temp_table(self, row):
        # Create header
        self.__create_cell(row, 0, "Cell", rs = 2, wsf=2)
        for i in range(constants.NUM_BANKS):
            col = i * 2 + 1
            bank_num = i + 1
            self.__create_cell(row, col, f"Bank {bank_num}", cs=2)
        for i in range(constants.NUM_BANKS * 2):
            col = i + 1
            txt = "V" if i % 2 == 0 else "T"
            self.__create_cell(row + 1, col, txt)
        row += 2

        # Create body
        for i in range(constants.NUM_CELLS_PER_BANK):
            self.__create_cell(row, 0, str(i + 1))
            for j in range(constants.NUM_BANKS):
                self.__volt_cells[(j, i)] = self.__create_cell(row, j * 2 + 1, "")
                self.__temp_cells[(j, i)] = self.__create_cell(row, j * 2 + 2, "")
            row += 1

        # Mean
        self.__create_cell(row, 0, "Mean")
        for i in range(constants.NUM_BANKS):
            self.__volt_bank_stats_cells[(i, "mean")] = self.__create_cell(row, i * 2 + 1, "")
            self.__temp_bank_stats_cells[(i, "mean")] = self.__create_cell(row, i * 2 + 2, "")
        row += 1

        # Total
        self.__create_cell(row, 0, "Total")
        for i in range(constants.NUM_BANKS):
            self.__volt_bank_stats_cells[(i, "sum")] = self.__create_cell(row, i * 2 + 1, "")
            self.__create_cell(row, i * 2 + 2, "-")
        row += 1

        return row
    
    def __create_volt_temp_stats_table(self, row):
        # Create header
        self.__create_cell(row, 0, "All Banks")
        self.__create_cell(row, 1, "Mean", cs=2)
        self.__create_cell(row, 3, "Min", cs=2)
        self.__create_cell(row, 5, "Max", cs=2)
        self.__create_cell(row, 7, "Std. Deviation", cs=2)
        self.__create_cell(row, 9, "Range", cs=2)
        self.__create_cell(row, 11, "Total", cs=2)
        row += 1

        # Voltage stats
        self.__create_cell(row, 0, "V")
        self.__volt_stats_cells["mean"] = self.__create_cell(row, 1, "", cs=2)
        self.__volt_stats_cells["min"] = self.__create_cell(row, 3, "", cs=2)
        self.__volt_stats_cells["max"] = self.__create_cell(row, 5, "", cs=2)
        self.__volt_stats_cells["stdev"] = self.__create_cell(row, 7, "", cs=2)
        self.__volt_stats_cells["range"] = self.__create_cell(row, 9, "", cs=2)
        self.__volt_stats_cells["sum"] = self.__create_cell(row, 11, "", cs=2)
        row += 1

        # Temperature stats
        self.__create_cell(row, 0, "T")
        self.__temp_stats_cells["mean"] = self.__create_cell(row, 1, "", cs=2)
        self.__temp_stats_cells["min"] = self.__create_cell(row, 3, "", cs=2)
        self.__temp_stats_cells["max"] = self.__create_cell(row, 5, "", cs=2)
        self.__temp_stats_cells["stdev"] = self.__create_cell(row, 7, "", cs=2)
        self.__temp_stats_cells["range"] = self.__create_cell(row, 9, "", cs=2)
        self.__temp_stats_cells["sum"] = self.__create_cell(row, 11, "-", cs=2)
        row += 1

        return row

    ###################
    # Shutdown States #
    ###################

    def __update_shutdown_state_table(self):
        states = ["bms", "imd", "tsms"]
        for s in states:
            value = self.__serial_data.shutdown_state.get(s, "-")
            bg = Table.COLOR_DEFAULT if value == 0 else Table.COLOR_OUT_OF_RANGE
            self.__shutdown_state_cells[s].config(text=value, background=bg)

    def __create_shutdown_state_table(self, row):
        # Headers
        self.__create_cell(row, 0, "Shutdown", rs=2)
        self.__create_cell(row, 1, "BMS", cs=2)
        self.__create_cell(row, 3, "IMD", cs=2)
        self.__create_cell(row, 5, "TSMS", cs=2)
        row += 1

        # Body
        self.__shutdown_state_cells["bms"] = self.__create_cell(row, 1, "", cs=2)
        self.__shutdown_state_cells["imd"] = self.__create_cell(row, 3, "", cs=2)
        self.__shutdown_state_cells["tsms"] = self.__create_cell(row, 5, "", cs=2)
        row += 1

        return row
    
    ################
    # Relay States #
    ################

    def __update_relay_state_table(self):
        states = ["pre_charge", "air_plus"]
        for s in states:
            value = self.__serial_data.relay_state.get(s, "-")
            bg = Table.COLOR_DEFAULT if value == 0 else Table.COLOR_OUT_OF_RANGE
            self.__relay_state_cells[s].config(text=value, background=bg)

    def __create_relay_state_table(self, row):
        # Headers
        self.__create_cell(row, 0, "Relay", rs=2)
        self.__create_cell(row, 1, "Pre-charge Relay", cs=2)
        self.__create_cell(row, 3, "Air+ Relay", cs=2)
        row += 1

        # Body
        self.__relay_state_cells["pre_charge"] = self.__create_cell(row, 1, "", cs=2)
        self.__relay_state_cells["air_plus"] = self.__create_cell(row, 3, "", cs=2)
        row += 1

        return row
    
    ############
    # IVT Data #
    ############

    def __update_IVT_table(self):
        states = ["u1", "u2", "u3", "i1"]
        for s in states:
            value = self.__serial_data.ivt_data.get(s, 0)
            self.__ivt_data_cells[s].config(text=value)

    def __create_IVT_table(self, row):
        # Headers
        self.__create_cell(row, 0, "IVT Data", rs=2)
        self.__create_cell(row, 1, "U1 [Name]", cs=2)
        self.__create_cell(row, 3, "U2 [Name]", cs=2)
        self.__create_cell(row, 5, "U3 [Name]", cs=2)
        self.__create_cell(row, 7, "I1 [Name]", cs=2)
        row += 1

        # Body
        self.__ivt_data_cells["u1"] = self.__create_cell(row, 1, "", cs=2)
        self.__ivt_data_cells["u2"] = self.__create_cell(row, 3, "", cs=2)
        self.__ivt_data_cells["u3"] = self.__create_cell(row, 5, "", cs=2)
        self.__ivt_data_cells["i1"] = self.__create_cell(row, 7, "", cs=2)

        row += 1

        return row

    ################
    # Charger Data #
    ################

    def __update_charger_data_table(self):
        states = ["state", "max_current", "max_voltage", "output_current", "output_voltage",
                  "hardware_failure", "temperature_protection", "input_voltage", "starting_state",
                  "communication_state"]
        for s in states:
            value = self.__serial_data.charger_data.get(s, "-")
            self.__charger_data_cells[s].config(text=value)

    def __create_charger_data_table(self, row):
        # Header 1
        self.__create_cell(row, 0, "Charger Data", rs=2)
        self.__create_cell(row, 1, "State (On / Off)", cs=2)
        self.__create_cell(row, 3, "Max Current", cs=2)
        self.__create_cell(row, 5, "Max Voltage", cs=2)
        self.__create_cell(row, 7, "Output Current", cs=2)
        self.__create_cell(row, 9, "Output Voltage", cs=2)
        row += 1

        # Body 1
        self.__charger_data_cells["state"] = self.__create_cell(row, 1, "", cs=2)
        self.__charger_data_cells["max_current"] = self.__create_cell(row, 3, "", cs=2)
        self.__charger_data_cells["max_voltage"] = self.__create_cell(row, 5, "", cs=2)
        self.__charger_data_cells["output_current"] = self.__create_cell(row, 7, "", cs=2)
        self.__charger_data_cells["output_voltage"] = self.__create_cell(row, 9, "", cs=2)
        row += 1

        # Header 2
        self.__create_cell(row, 0, "Charger Status Flags", rs=2)
        self.__create_cell(row, 1, "Hardware Failure", cs=2)
        self.__create_cell(row, 3, "Temperature Protection", cs=2)
        self.__create_cell(row, 5, "Input Voltage", cs=2)
        self.__create_cell(row, 7, "Starting State", cs=2)
        self.__create_cell(row, 9, "Communication State", cs=2)
        row += 1

        # Body 2
        self.__charger_data_cells["hardware_failure"] = self.__create_cell(row, 1, "", cs=2)
        self.__charger_data_cells["temperature_protection"] = self.__create_cell(row, 3, "", cs=2)
        self.__charger_data_cells["input_voltage"] = self.__create_cell(row, 5, "", cs=2)
        self.__charger_data_cells["starting_state"] = self.__create_cell(row, 7, "", cs=2)
        self.__charger_data_cells["communication_state"] = self.__create_cell(row, 9, "", cs=2)
        row += 1

        return row
    
    #####################
    # Cell Balance Data #
    #####################

    def __update_cell_balance_data_table(self):
        states = ["target_voltage", "num_cells_discharging"]
        for s in states:
            value = self.__serial_data.cell_balance_data.get(s, "-")
            self.__cell_balance_data_cells[s].config(text=value)

    def __create_cell_balance_data_table(self, row):
        # Headers
        self.__create_cell(row, 0, "Discharge Data", rs=2)
        self.__create_cell(row, 1, "Target Voltage", cs=2)
        self.__create_cell(row, 3, "No. Cells Discharging", cs=2)
        row += 1

        # Body
        self.__cell_balance_data_cells["target_voltage"] = self.__create_cell(row, 1, "", cs=2)
        self.__cell_balance_data_cells["num_cells_discharging"] = self.__create_cell(row, 3, "", cs=2)
        row += 1

        return row

    ##################
    # Helper Methods #
    ##################

    """
    Create cell at (row, col). Col == -1 reserved as margin.

    Args:
        row [int]: Cell grid row.
        col [int]: Cell grid column.
        txt [str]: Cell text.
        border [bool]: Cell border.
        rs [int]: Cell row span.
        cs [int]: Cell column span.
        wsf [float]: Cell width scale factor.
    Return:
        cell: Tkinter label object.
    """
    def __create_cell(self, row, col, txt, border=True, rs=1, cs=1, wsf=1):
        bd_width = 1 if border else 0
        bd = 1 if border else 0
        cell = tk.Label(self.__frame, text=txt, borderwidth=bd_width, width=int(Table.CELL_WIDTH * wsf * cs), 
                        height=Table.CELL_HEIGHT * rs, bd=bd, relief="solid")
        cell.grid(row=row, column=col + 1, rowspan=rs, columnspan=cs, sticky="nesw")
        if not Table.COLOR_DEFAULT:
            Table.COLOR_DEFAULT = cell.cget("background")
        return cell
    
    def __create_blank_row(self, row):
        self.__create_cell(row, 0, "", border=False)
        return row + 1
