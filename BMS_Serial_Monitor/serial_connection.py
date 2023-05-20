import serial.tools.list_ports, serial, constants

class Serial_Connection:
    def __init__(self, port, baudrate, bytesize, parity, stopbits):
        self.__display_active_ports()
        self.connection = self.__connect(port, baudrate, bytesize, parity, stopbits)

    def __display_active_ports(self):
        print(f"Active ports: {[tuple(x)[0] for x in list(serial.tools.list_ports.comports())]}")
            
    def __connect(self, port, baudrate, bytesize, parity, stopbits):
        while True:
            try:
                serial_connection = serial.Serial(port, baudrate, bytesize, parity, stopbits)
                return serial_connection
            except serial.SerialException as error:
                pass
        
    def __process_message(message, message_id, bank, table):
        match message_id:
            case constants.VOLTAGE_ID:
                for i in range(constants.NUM_CELLS_PER_BANK):
                    table.update_value(float(message[i + 1]), bank, i, "V")
            case constants.TEMPERATURE_ID:
                for i in range(constants.NUM_CELLS_PER_BANK):
                    table.update_value(float(message[i + 1]), bank, i, "T")
            case constants.DISCHARGE_ID:
                for i in range(constants.NUM_CELLS_PER_BANK):
                    table.update_discharge(int(message[i + 1]), bank, i)
    
    def monitor_data(self, table):
        self.connection.readline()
        while True:
            message = ""
            try:
                message = self.connection.readline().decode().replace("\x00", "").replace("\n", "").split(" ")
                message_id = int(message[0]) & constants.MESSAGE_ID_MASK
                bank = (int(message[0]) & constants.BANK_ID_MASK) >> constants.BITS_PER_MESSAGE_ID
                self.__process_message(message, message_id, bank, table)
            except:
                if message:
                    print("Error:", message)

    