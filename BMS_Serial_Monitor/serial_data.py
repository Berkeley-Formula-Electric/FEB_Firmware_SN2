from bank_statistic import Bank_Statistic

class Serial_Data:
    def __init__(self):
        self.bms_state = dict()
        self.voltage = Bank_Statistic()
        self.temperature = Bank_Statistic()
        self.relay_state = dict()
        self.shutdown_state = dict()
        self.ivt_data = dict()
        self.charger_data = dict()
        self.cell_balance_data = dict()