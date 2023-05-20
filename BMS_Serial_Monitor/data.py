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
        self.std_deviation = statistics.stdev(self.nums)
        self.mean = sum(self.nums) / len(self.nums)
        self.min = min(self.nums)
        self.max = max(self.nums)
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
