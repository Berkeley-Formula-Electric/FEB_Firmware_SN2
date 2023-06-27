import constants, statistics

class Data:
    def __init__(self):
        self.nums = [0] * constants.NUM_ACTIVE_BANKS * constants.NUM_CELLS_PER_BANK
        self.std_deviation = 0
        self.range = 0
        self.mean = 0
        self.min = 0
        self.max = 0

    def __index(self, bank, cell):
        return bank * constants.NUM_CELLS_PER_BANK + cell
    
    def update_value(self, value, bank, cell):
        self.nums[self.__index(bank, cell)] = value

    def calculate(self):
        error_codes = [-41, -42]
        filtered_nums = list(filter(lambda x : int(x) not in error_codes, self.nums))
        self.std_deviation = statistics.stdev(filtered_nums)
        self.mean = sum(filtered_nums) / len(filtered_nums)
        self.min = min(filtered_nums)
        self.max = max(filtered_nums)
        self.range = self.max - self.min

    def get_value(self, type):
        match type:
            case "std_deviation":
                return self.std_deviation
            case "range":
                return self.range
            case "mean":
                return self.mean
            case "min":
                return self.min
            case "max":
                return self.max
