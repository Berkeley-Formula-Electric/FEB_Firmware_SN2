import csv

# constants
CSV_FILE_PATH = "values.csv"
C_HEADER_FILE_PATH = "temp_volt_map.h"
RESOLUTION = 0.001
# LOWER_TEMP = -40
# UPPER_TEMP = 120

LOWER_VOLTAGE = 1.30
UPPER_VOLTAGE = 2.44

# test
CSV_NEW_DATA_FILE_PATH = "result.csv"

def read_csv(file_path):
    assert ".csv" in file_path
    
    with open(file_path) as csv_file:
        reader = csv.reader(csv_file, delimiter=",", quotechar='"')
        return list(reader)

def interpolate(x, x1, y1, x2, y2):
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1)

# def interpolate_data(temp_voltage_data):
#     result = []
#     for i in range(len(temp_voltage_data) - 1):
#         x1 = temp_voltage_data[i][0]
#         y1 = temp_voltage_data[i][1]
#         x2 = temp_voltage_data[i + 1][0]
#         y2 = temp_voltage_data[i + 1][1]

#         x = x1
#         while x < x2:
#             y = interpolate(x, x1, y1, x2, y2)
#             result.append([round(x, 4), round(y, 4)])
#             x += RESOLUTION
    
#     result.append(temp_voltage_data[-1][:])

#     return result

def interpolate_data(temp_voltage_data):
    result = []
    index = len(temp_voltage_data) - 1
    x = LOWER_VOLTAGE

    x1, y1 = temp_voltage_data[index][1], temp_voltage_data[index][0]
    x2, y2 = temp_voltage_data[index - 1][1], temp_voltage_data[index - 1][0]

    while x <= UPPER_VOLTAGE:
        if x >= temp_voltage_data[index - 1][1]:
            index -= 1

            x1, y1 = temp_voltage_data[index][1], temp_voltage_data[index][0]
            x2, y2 = temp_voltage_data[index - 1][1], temp_voltage_data[index - 1][0]

        y = interpolate(x, x1, y1, x2, y2)
        result.append([round(x, 4), round(y, 4)])
        
        x += RESOLUTION
    
    return result

def test_interpolate_func(volt_temp_map, voltage):
    index = round((voltage - LOWER_VOLTAGE) / RESOLUTION)
    return volt_temp_map[index][1]

def write_csv(file_path, data_arr):
    with open(file_path, "w") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerows(data_arr)

def write_c_header(file_path, str):
    with open(file_path, "w") as c_header_file:
        c_header_file.write(str)

def arr_to_c_arr_string(temp_voltage_map):
    def map_arr_cArr(arr):
        if not isinstance(arr, list):
            return str(arr)
        
        return "{" + ", ".join(map(map_arr_cArr, arr)) + "}"
    
    result_arr_str = []
    result_arr_str.append(f"#define MAP_COUNT {len(temp_voltage_map)}")
    result_arr_str.append(f"float TEMP_VOLT_MAP[][2] = {map_arr_cArr(temp_voltage_map)};")
    result_arr_str.append(f"float RESOLUTION = {RESOLUTION};")
    result_arr_str.append(f"float MIN_MAP_TEMP = {temp_voltage_map[-1][1]};")
    result_arr_str.append(f"float MAX_MAP_TEMP = {temp_voltage_map[0][1]};")
    result_arr_str.append(f"float MIN_MAP_VOLT = {temp_voltage_map[0][0]};")
    result_arr_str.append(f"float MAX_MAP_VOLT = {temp_voltage_map[-1][0]};")

    return "\n".join(result_arr_str)

def main():
    csv_data = read_csv(CSV_FILE_PATH)[1:]
    temp_voltage_data = sorted(list(map(lambda x : [float(x[0]), float(x[1])], csv_data)))    
    temp_voltage_LUT = interpolate_data(temp_voltage_data)

    # test
    write_csv(CSV_NEW_DATA_FILE_PATH, temp_voltage_LUT)

    c_arr_string = arr_to_c_arr_string(temp_voltage_LUT)

    write_c_header(C_HEADER_FILE_PATH, c_arr_string)

if __name__ == "__main__":
    main()