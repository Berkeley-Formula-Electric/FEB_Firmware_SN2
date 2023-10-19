import constants
import serial, serial.tools.list_ports
from serial_data import Serial_Data

# TODO: TEST
import random, time

class Serial_Connection:
    BAUDRATE = 115200
    BYTESIZE = serial.SEVENBITS
    PARITY = serial.PARITY_EVEN
    STOPBITS = serial.STOPBITS_ONE

    def __init__(self, sd: Serial_Data):
        ports = self.__get_active_ports()
        self.__display_active_ports(ports)
        port_str = self.__select_active_port(ports)
        self.connection = self.__connect(port_str)
        self.__serial_data = sd

    
    def read_data(self):
        # TEST CODE
        # while True:
        #     bank = random.randint(0, 6)
        #     cell = random.randint(0, 19)
        #     volt = 2.8 + 1.6 * random.random()
        #     temp = 12 + 8 * random.random()
        #     self.__serial_data.voltage[(bank, cell)] = volt
        #     self.__serial_data.temperature[(bank, cell)] = temp

        #     self.__process_message(constants.BMS_STATE_MSG_ID, list("11101"))
        #     self.__process_message(constants.RELAY_STATE_MSG_ID, list("01"))
        #     self.__process_message(constants.SHUTDOWN_STATE_MSG_ID, list("101"))
        #     self.__process_message(constants.IVT_DATA_MSG_ID, f"3 2 1 {int(sum(random.random() for _ in range(100)) * 30 - 1500)}".split(" "))

        #     time.sleep(0.02)

        # Actual Code:
        
        self.connection.readline()
        message = ""
        while True:
            try:
                message = self.connection.readline()
                message_split = message.decode().replace("\x00", "").replace("\n", "").split(" ")
                message_id = int(message_split[0])
                message_data = message_split[1:]
                self.__process_message(message_id, message_data)
            except:
                if message:
                    print("Error:", message)

    def __process_message(self, message_id, message_data):
        match message_id:
            case constants.CELL_VOLT_MSG_ID:
                bank = int(message_data[0])
                for i in range(1, constants.NUM_CELLS_PER_BANK + 1):
                    cell = i - 1
                    # value = int(message_data[i]) * 10 ** -4
                    value = float(message_data[i])
                    self.__serial_data.voltage[(bank, cell)] = value
            case constants.CELL_TEMP_MSG_ID:
                bank = int(message_data[0])
                for i in range(1, constants.NUM_CELLS_PER_BANK + 1):
                    cell = i - 1
                    # value = int(message_data[i]) * 10 ** -4
                    value = float(message_data[i])
                    self.__serial_data.temperature[(bank, cell)] = value
            case constants.BMS_STATE_MSG_ID:
                states = ["balance", "charge", "drive", "precharge", "shutdown"]
                for i in range(len(states)):
                    if message_data[i] not in ['0', '1']:
                        raise ValueError("Invalid BMS state.")
                    value = message_data[i] == '0'
                    self.__serial_data.bms_state[states[i]] = value
            case constants.RELAY_STATE_MSG_ID:
                states = ["pre_charge", "air_plus"]
                for i in range(len(states)):
                    value = int(message_data[i])
                    self.__serial_data.relay_state[states[i]] = value
            case constants.SHUTDOWN_STATE_MSG_ID:
                states = ["bms", "imd", "tsms"]
                for i in range(len(states)):
                    value = int(message_data[i])
                    self.__serial_data.shutdown_state[states[i]] = value
            case constants.CHARGE_DATA_MSG_ID:
                state = message_data[0]
                max_current = message_data[1] * 10 ** -1
                max_voltage = message_data[2] * 10 ** -1
                output_current = message_data[3] * 10 ** -1
                output_voltage = message_data[4] * 10 ** -1
                status_flag = int(message_data[5])

                # General data
                self.__charger_data_cells["state"] = state
                self.__charger_data_cells["max_current"] = max_current
                self.__charger_data_cells["max_voltage"] = max_voltage
                self.__charger_data_cells["output_current"] = output_current
                self.__charger_data_cells["output_voltage"] = output_voltage

                # Errors
                self.__charger_data_cells["hardware_failure"]       = status_flag >> 7 & 1
                self.__charger_data_cells["temperature_protection"] = status_flag >> 6 & 1
                self.__charger_data_cells["input_voltage"]          = status_flag >> 5 & 1
                self.__charger_data_cells["starting_state"]         = status_flag >> 4 & 1
                self.__charger_data_cells["communication_state"]    = status_flag >> 3 & 1
            case constants.CELL_BALANCE_MSG_ID:
                pass
            case constants.IVT_DATA_MSG_ID:
                states = ["u1", "u2", "u3", "i1"]
                print("IVT VALUE!")
                for i in range(len(states)):
                    # value = int(message_data[i]) * 10 ** -3
                    value = float(message_data[i])
                    self.__serial_data.ivt_data[states[i]] = value
            case _:
                raise ValueError("Invalid message identifier.")

    """
    Args:
        port_str: String name of serial port to connect to.
    Returns:
        Serial object.
    Raises:
        Exception: Unable to connect to serial port.
    """
    def __connect(self, port_str):
        try:
            serial_connection = serial.Serial(port_str, Serial_Connection.BAUDRATE, Serial_Connection.BYTESIZE, 
                                              Serial_Connection.PARITY, Serial_Connection.STOPBITS)
            return serial_connection
        except serial.SerialException as err:
            raise Exception("Cannot connect to serial port.")
    
    def __display_active_ports(self, ports):
        ports_str = [p[0] for p in ports]
        print(f"Active ports: {ports_str}")

    """
    Returns:
        List of serial ports.
    Raises:
        Exception: No active serial ports.
    """
    def __get_active_ports(self):
        ports = serial.tools.list_ports.comports()
        if len(ports) == 0:
            raise Exception("No active ports.")
        return ports

    """
    Args:
        ports: List of serial ports, non-empty.
    Returns:
        port_str: String name of port selected by user.
    """
    def __select_active_port(self, ports):
        port_index = -1
        while not 1 <= port_index <= len(ports):
            try:
                port_index = int(input(f"Choose port [1, {len(ports)}]: "))
            except:
                pass
        return ports[port_index - 1][0]