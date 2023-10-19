from statistics import mean, stdev

class Bank_Statistic(dict):
    RESOLUTION = 3

    def get_stats(self, bank=None):
        if bank != None:
            values = [v for k,v in self.items() if k[0] == bank]
        else:
            values = self.values()
        stats = {
            "mean": round(mean(values), Bank_Statistic.RESOLUTION) if values else 0,
            "min": round(min(values), Bank_Statistic.RESOLUTION) if values else 0,
            "max": round(max(values), Bank_Statistic.RESOLUTION) if values else 0,
            "stdev": round(stdev(values), Bank_Statistic.RESOLUTION) if len(values) > 1 else 0,
            "range": round(max(values) - min(values), Bank_Statistic.RESOLUTION) if values else 0,
            "sum": round(sum(values), Bank_Statistic.RESOLUTION)
        }
        return stats
    
    """
    Args:
        key [tuple(int, int)]: (bank, cell)
        value [int or float]
    """
    def __setitem__(self, key, value):
        if not (isinstance(value, int) or isinstance(value, float)):
            raise ValueError("Value is not type integer or float.")
        elif not isinstance(key, tuple) or len(key) != 2 or not (isinstance(key[0], int) and isinstance(key[1], int)):
            raise ValueError("Key is not format (bank, cell).")
        super().__setitem__(key, value)