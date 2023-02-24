import csv
import pandas as pd
import os

REMOVE_CSV_FILE = False

# set file path and file name
XLSX_FILE_DIRECTORY = ""
XLSX_FILE_NAME = "data"
HEADER_FILE_NAME = "FEB_CAN_NODE"

# extensions
HEADER_EXTENSION = ".h"
CSV_FILE_EXTENSION = ".csv"
XLSX_FILE_EXTENSION = ".xlsx"

# file paths
XLSX_FILE_PATH = XLSX_FILE_DIRECTORY + XLSX_FILE_NAME + XLSX_FILE_EXTENSION
CSV_FILE_PATH = XLSX_FILE_NAME + CSV_FILE_EXTENSION
HEADER_FILE_PATH = HEADER_FILE_NAME + HEADER_EXTENSION

class C:
    """A series of template strings for C"""

    @staticmethod
    def comment(comment):
        return f"/*** {comment} ***/"

    @staticmethod
    def define(name, value):
        return f"#define {name} {value}"

    @staticmethod
    def include(file_name):
        return f"#include \"{file_name}\""

    @staticmethod
    def struct(name, attributes):
        """
        >>> name = 'BMS_MESSAGE_TYPE'
        >>> attributes = [('float', 'temperature'), ('float', 'voltage')]
        >>> print(structs(name, attributes))
        typedef struct BMS_MESSAGE_TYPE {
            float temperature;
            float voltage;
        } BMS_MESSAGE_TYPE;
        """
        
        str_lst = []

        str_lst.append(f"typedef struct {name} {'{'}")
        for attribute in attributes:
            str_lst.append(f"    {attribute[0]} {attribute[1]};")
        str_lst.append(f"{'}'} {name};")

        return "\n".join(str_lst)

    @staticmethod
    def typedef(data_type, name):
        return f"typedef {data_type} {name};"

class HeaderFile:
    def __init__(self, id_length_bits, bits_per_id, board_data):
        assert isinstance(id_length_bits, int) and id_length_bits > 0
        assert isinstance(bits_per_id, int) and 0 < bits_per_id < id_length_bits
        assert isinstance(board_data, dict)

        self.id_length_bits = id_length_bits
        self.bits_per_id = bits_per_id
        self.board_data = board_data
    
    @property
    def bits_per_message_type(self):
        return self.id_length_bits - self.bits_per_id

    def __str__(self):
        file_contents = ""

        # ------includes------
        file_contents += "#ifndef INC_FEB_CAN_NODE_H_\n"
        file_contents += "#define INC_FEB_CAN_NODE_H_\n\n"

        file_contents += C.include("string.h")
        file_contents += "\n" * 2

        # ------settings------
        settings = []

        settings.append(C.comment("SETTINGS"))
        settings.append(C.define("ID_LENGTH_BITS", self.id_length_bits))
        settings.append(C.define("BITS_PER_ID", self.bits_per_id))
        settings.append(C.define("BITS_PER_MESSAGE_TYPE", self.bits_per_message_type))
        
        settings.append("")
        settings.append(C.typedef("uint32_t", "AddressIdType"))
        settings.append(C.typedef("uint8_t", "FilterArrayLength"))

        file_contents += "\n".join(settings)
        file_contents += "\n" * 2

        # ------boards------
        boards = []
        board_id = 0
        for board_name in self.board_data.keys():
            boards.append(Board(board_name, self.board_data[board_name], board_id, self.id_length_bits, self.bits_per_id, self.bits_per_message_type))
            board_id += 1
        
        for board in boards:
            file_contents += str(board)
            file_contents += "\n"
        
        # ------helper functions------

        # ------rx arrays------
        rx_str_lst = []
        rx_str_lst.append(C.comment(f"RX Arrays"))
        
        for board in boards:
            rx_string = board.rx_string()
            if rx_string:
                rx_str_lst.append(rx_string)
                rx_str_lst.append("")

        file_contents += "\n".join(rx_str_lst)
        file_contents += "\n"

        # ------assign filter array------
        filter_str_arr = []
        filter_str_arr.append("const AddressIdType* assign_filter_array(AddressIdType NODE_ID) {")
        filter_str_arr.append("    switch(NODE_ID) {")

        for board in boards:
            if board.data["rx_data"]:
                filter_str_arr.append(f"        case {board.name}_ID:")
                filter_str_arr.append(f"            return {board.name}_RX_ID;")
                filter_str_arr.append("            break;")
        
        filter_str_arr.append("    }")
        filter_str_arr.append("    return 0;")
        filter_str_arr.append("}")

        file_contents += "\n".join(filter_str_arr)
        file_contents += "\n" * 2
        
        # ------assign filter array length------
        filter_length_str_arr = []
        filter_length_str_arr.append("FilterArrayLength assign_filter_array_legnth(AddressIdType NODE_ID) {")
        filter_length_str_arr.append("    switch(NODE_ID) {")
        
        for board in boards:
            if board.data["rx_data"]:
                filter_length_str_arr.append(f"        case {board.name}_ID:")
                filter_length_str_arr.append(f"            return {board.name}_RX_NUM;")
                filter_length_str_arr.append("            break;")

        filter_length_str_arr.append("    }")
        filter_length_str_arr.append("    return 0;")
        filter_length_str_arr.append("}")

        file_contents += "\n".join(filter_length_str_arr)
        file_contents += "\n" * 2

        # ------store messages------
        store_msg_str_arr = []
        store_msg_str_arr.append("void store_msg(CAN_RxHeaderTypeDef *pHeader, uint8_t RxData[]) {")
        store_msg_str_arr.append("    switch(pHeader->StdId >> BITS_PER_MESSAGE_TYPE) {")
        
        for board in boards:
            if self.board_data[board.name]["message_type"]:
                store_msg_str_arr.append(f"        case {board.name}_ID:")
                store_msg_str_arr.append(f"            Store_{board.name}_Msg(pHeader->StdId, RxData, pHeader->DLC);")
                store_msg_str_arr.append("            break;")
        
        store_msg_str_arr.append("    }")
        store_msg_str_arr.append("}")

        file_contents += "\n".join(store_msg_str_arr)

        # ------includes------
        file_contents += "\n\n#endif /* INC_FEB_CAN_H_ */\n"

        return file_contents

class Board:
    board_id_list = []

    def __init__(self, name, data, board_id, id_length_bits, bits_per_id, bits_per_message_type):
        self.name = name
        self.data = data
        self.board_id = board_id
        self.id_length_bits = id_length_bits
        self.bits_per_id = bits_per_id
        self.bits_per_message_type = bits_per_message_type
    
    def rx_string(self):
        if not self.data["rx_data"]:
            return ""

        rx_str_lst = []

        rx_boards = ", ".join([f"{rx_board}_ID" for rx_board in self.data["rx_data"]])
        rx_str_lst.append(f"const AddressIdType {self.name}_RX_ID[] = {'{'}{rx_boards}{'}'};")
        rx_str_lst.append(f"const FilterArrayLength {self.name}_RX_NUM = {len(self.data['rx_data'])};")

        return "\n".join(rx_str_lst)
    

    def __str__(self):
        string = ""

        board_str_lst = []
        board_id_binary_str = f"0b{'{0:0' + str(self.bits_per_id) + 'b}'}".format(self.board_id)
        
        # ---Board IDs---
        board_str_lst.append(C.comment(f"{self.name} IDs"))
        board_str_lst.append(C.define(self.name + "_ID", board_id_binary_str))

        # message type id
        message_id = 0
        for message in self.data["message_type"]:            
            current_message_id = (self.board_id << self.bits_per_message_type) + message_id
            
            message_id_binary_str = f"0b{'{0:0' + str(self.id_length_bits) + 'b}'}".format(current_message_id)
            board_str_lst.append(C.define("_".join([self.name, message["name"]]), message_id_binary_str))
            
            message_id += 1
        board_str_lst.append("")

        # ---Message Buffers---       
        if self.data["message_type"]:
            board_str_lst.append(C.comment(f"{self.name} MESSSAGE BUFFER"))

            # message data types
            for message in self.data["message_type"]:
                board_str_lst.append(C.define(f"{self.name}_{message['name']}_TYPE", message["data_type"]))
            board_str_lst.append("")

            # message struct
            struct_attributes = map(lambda x: (f"{self.name}_{x['name']}_TYPE", x["name"].lower()), self.data["message_type"])
            struct_name = f"{self.name}_MESSAGE_TYPE"
            struct_variable = f"{self.name}_MESSAGE"

            board_str_lst.append(C.struct(struct_name, struct_attributes))
            board_str_lst.append(f"{struct_name} {struct_variable};")
            board_str_lst.append("")

        # ---Store Message Function--- 
        if self.data["message_type"]:
            board_str_lst.append(f"void Store_{self.name}_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {'{'}")
            board_str_lst.append("    switch (RxId){")
            
            for message in self.data["message_type"]:
                board_str_lst.append(f"        case {'_'.join([self.name, message['name']])}:")
                board_str_lst.append(f"            memcpy(&({self.name}_MESSAGE.{message['name'].lower()}), RxData, data_length);")
                board_str_lst.append("            break;")
            
            board_str_lst.append("    }")
            board_str_lst.append("}")

        string += "\n".join(board_str_lst)
        string += "\n"

        return string

def xlsx_to_csv(xlsx_file_path, csv_file_path):
    data_xls = pd.read_excel(xlsx_file_path, 'main', index_col=None)
    data_xls.to_csv(csv_file_path, encoding='utf-8', index=False)
    
def read_csv(filepath):
    assert ".csv" in filepath
    
    with open(filepath) as csv_file:
        reader = csv.reader(csv_file, delimiter=",", quotechar='"')
        return list(reader)

def extract_csv_data(csv_data):
    # column locations
    ID_LENGTH_BITS = 0
    BITS_PER_ID = 1
    BOARD_NAMES = 2
    RX_BOARDS = 3
    MESSAGE_TYPES = 4
    MESSAGE_DATA_TYPES = 5

    data = {}
    
    # ------settings------
    data["id_length_bits"] = int(float(csv_data[ID_LENGTH_BITS][1]))
    data["bits_per_id"] = int(float(csv_data[BITS_PER_ID][1]))

    # ------boards------
    boards = {}
    current_board = ""
    for i in range(1, len(csv_data[BOARD_NAMES])):
        if csv_data[BOARD_NAMES][i]:
            current_board = csv_data[BOARD_NAMES][i].upper()
            
            # setup new board
            boards[current_board] = {}
            boards[current_board]["rx_data"] = []
            boards[current_board]["message_type"] = []

        if csv_data[RX_BOARDS][i]:
            boards[current_board]["rx_data"].extend(csv_data[RX_BOARDS][i].upper().split(" "))
        if csv_data[MESSAGE_TYPES][i]:
            message_name = "_".join(csv_data[MESSAGE_TYPES][i].upper().split())
            message_data_type = csv_data[MESSAGE_DATA_TYPES][i]

            boards[current_board]["message_type"].append({"name": message_name, "data_type": message_data_type})
    data["boards"] = boards
    
    return data

def main():
    # read data
    xlsx_to_csv(XLSX_FILE_PATH, CSV_FILE_PATH)
    csv_data = read_csv(CSV_FILE_PATH)
    csv_data_transposed = [*zip(*csv_data)]
    data = extract_csv_data(csv_data_transposed)

    # remove csv file
    if REMOVE_CSV_FILE:
        os.remove(CSV_FILE_PATH)

    header_file = HeaderFile(data["id_length_bits"], data["bits_per_id"], data["boards"])
    file_contents = str(header_file)

    # write file
    with open(HEADER_FILE_PATH, "w") as header_file:
        header_file.write(file_contents)

if __name__ == "__main__":
    main()