import serial, serial.tools.list_ports, time, threading

try:
	import tkinter as tk
except:
	import Tkinter as tk

# active
PROGRAM_RUN = True
RESOLUTION = 3

# battery configuration
BANKS = 1
CELLS_PER_BANK = 17

# serial settings
PORT = '/dev/cu.usbmodem2103'
BAUD_RATE = 115200
BYTESIZE = serial.SEVENBITS
PARITY = serial.PARITY_EVEN
STOPBITS = serial.STOPBITS_ONE

# data format
DATA_LOG_FILE_PATH = "data.csv"
BITS_PER_MESSAGE 	= 1
BITS_PER_BANK		= 3

# messages
VOLTAGE_ID 			= 0b0
TEMPERATURE_ID 		= 0b1
MESSAGES = {VOLTAGE_ID: "V", TEMPERATURE_ID: "T"}

# masks
BANK_ID_MASK 		= 0b1110
MESSAGE_MASK		= 0b0001

class Table:
	def __init__(self, root: tk.Tk, width: int, height: int):
		self.root = root
		self.cells = {}

		self.create_table(width, height)

	def create_table(self, width, height) -> None:
		cell_width = 15

		# header
		tk.Label(self.root, text = "Bank Number", borderwidth = 1, width = cell_width * 2, height = 5, bd = 1, relief = "solid").grid(row = 0, column = 0, rowspan = 2, sticky = "nesw")

		for col in range(1, width + 1):
			tk.Label(self.root, text = f"Cell {col}", borderwidth = 1, height = 2, bd = 1, relief = "solid").grid(row = 0, column = col * 2 - 1, columnspan = 2, sticky = "nesw")
			tk.Label(self.root, text = "V", borderwidth = 1, width = cell_width, height = 1, bd = 1, relief = "solid").grid(row = 1, column = col * 2 - 1, sticky = "nesw")
			tk.Label(self.root, text = "T", borderwidth = 1, width = cell_width, height = 1, bd = 1, relief = "solid").grid(row = 1, column = col * 2, sticky = "nesw")
		
		for row in range(2, height + 2):
			tk.Label(self.root, text = f"{row - 1}", borderwidth = 1, width = cell_width * 2, height = 1, bd = 1, relief = "solid").grid(row = row, column = 0)

		# body
		for row in range(2, height + 2):
			for col in range(1, width + 1):
				volt_label = tk.Label(self.root, text = "V", borderwidth = 1, width = cell_width, height = 1, bd = 1, relief = "solid")
				temp_label = tk.Label(self.root, text = "T", borderwidth = 1, width = cell_width, height = 1, bd = 1, relief = "solid")
				
				volt_label.grid(row = row, column = col * 2 - 1)
				temp_label.grid(row = row, column = col * 2)
				
				self.cells[(row - 2, col - 1, "V")] = volt_label
				self.cells[(row - 2, col - 1, "T")] = temp_label
	
	def update(self, bank: int, segment: int, type: str, value: int) -> None:
		self.cells[(bank, segment, type.upper())].configure(text = str(value))

def serial_connection_init(port: str, baudrate: int, bytesize: int, parity: str, stopbits: int) -> serial.Serial:
	print(f"Active ports: {[tuple(x)[0] for x in list(serial.tools.list_ports.comports())]}")
	print("Waiting for connection...")
	while True:
		try:
			serial_connection = serial.Serial(port, baudrate, bytesize, parity, stopbits)		
			break
		except:
			pass
	
	time.sleep(0.500)
	print(f"Connected: {serial_connection.name}\n")

	return serial_connection

def setup_gui() -> tuple[tk.Tk, Table]:
	root = tk.Tk()
	root.title("BMS Information")

	canvas = tk.Canvas(root, borderwidth = 0)
	frame = tk.Frame(canvas)
	xscrollbar = tk.Scrollbar(root, orient = "horizontal", command = canvas.xview)
	yscrollbar = tk.Scrollbar(root, orient = "vertical", command = canvas.yview)
	canvas.configure(xscrollcommand = xscrollbar.set, yscrollcommand = yscrollbar.set)

	xscrollbar.pack(side = "bottom", fill = "x")
	yscrollbar.pack(side = "right", fill = "y")
	canvas.pack(fill = "both", expand = True)
	canvas.create_window((4, 4), window = frame, anchor="nw")

	frame.bind("<Configure>", lambda event, canvas=canvas: on_frame_configuration(canvas))
	tk_table = Table(frame, CELLS_PER_BANK, BANKS)

	return root, tk_table

def on_frame_configuration(canvas: tk.Canvas) -> None:
    '''Reset the scroll region to encompass the inner frame'''
    canvas.configure(scrollregion=canvas.bbox("all"))

def serial_monitor(tk_table: Table, serial_connection: serial.Serial) -> None:
	# read sample lines
	serial_connection.readline()

	while PROGRAM_RUN:
		message = ""
		try:
			message = serial_connection.readline().decode().replace("\x00", "").replace("\n", "").split(" ")

			bank_id = (int(message[0]) & BANK_ID_MASK) >> BITS_PER_MESSAGE
			message_id = MESSAGES[int(message[0]) & MESSAGE_MASK]
			data = []

			for i in range(CELLS_PER_BANK):
				cell_data = round(float(message[i + 1]), RESOLUTION)
				data.append(cell_data)
				tk_table.update(bank_id, i, message_id, cell_data)

			log_data(bank_id + 1, message_id, data)
		except:
			if message:
				print("Error:", message)
	
def log_data(bank_id: int, message_id: str, data: list) -> None:
	data_str_arr = []
	data_str_arr.append(str(time.time()))
	data_str_arr.append(str(bank_id))
	data_str_arr.append(message_id)
	for d in data:
		data_str_arr.append(str(d))

	data_str = ','.join(data_str_arr) + '\n'
	write_data_to_file(DATA_LOG_FILE_PATH, data_str)


def write_data_to_file(file_path: str, data: str) -> None:
	assert isinstance(data, str)

	with open(file_path, 'a') as file:
		file.write(data)

def thread_serial_monitor(tk_table: Table, serial_connection: serial.Serial) -> None:
	t1 = threading.Thread(target = serial_monitor, args = [tk_table, serial_connection])
	t1.start()

def main() -> None:
	serial_connection = serial_connection_init(PORT, BAUD_RATE, BYTESIZE, PARITY, STOPBITS)

	# initialize data logging state
	write_data_to_file(DATA_LOG_FILE_PATH, "\n\n")

	# setup gui
	root, tk_table = setup_gui()
	root.after(100, thread_serial_monitor, tk_table, serial_connection)
	root.mainloop()
	
	# end program
	global PROGRAM_RUN
	PROGRAM_RUN = False

if __name__ == "__main__":
	main()