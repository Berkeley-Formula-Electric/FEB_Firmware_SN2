import gui, table, constants, random, threading, time, serial_connection

def main():
    sc = serial_connection.Serial_Connection(constants.PORT, constants.BAUDRATE, constants.BYTESIZE, constants.PARITY, constants.STOPBITS)
    win = gui.GUI()
    tb = table.Table(win)
    
    t1 = threading.Thread(target=update_stats, args=[tb])
    t2 = threading.Thread(target=sc.monitor_data, args=[tb])
    t1.start()
    t2.start()

    win.root.mainloop()

def update_stats(tb):
    while True:
        tb.update_statistics()
        time.sleep(1)

if __name__ == "__main__":
    main()