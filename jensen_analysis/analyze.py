import re
from dataclasses import dataclass
from datetime import datetime
import matplotlib.pyplot as plt
import pandas
import pandas as pd

mem_logger_file = "memlogger.9.log"
jensenlog_file = 'res_detailed_56.txt'

def load_memlogger_file():
    logs = {}
    with open(mem_logger_file, "r") as f:
        title = f.readline().split(", ")
        title = [x.strip() for x in title]
        for t in title:
            logs[t] = []
        for line in f:
            line = line.split(", ")
            logs[title[0]].append(datetime.strptime(line[0], "%Y-%m-%d %H:%M:%S.%f"))
            for i in range(1, len(line)):
                logs[title[i]].append(int(float(line[i].strip())))
    return pandas.DataFrame(logs)

def parse_value(x):
    try:
        x = int(float(x.strip()))
    except ValueError: # x is a tuple of hi,low 64 bit numbers
        x = eval(x)
        x = x[0]*2**64+x[1]
    return x

def load_jensenlog_file():
    logs = {}
    with open(jensenlog_file, "r") as f:
        for line in f:
            row_title = re.findall("<.*?>", line)
            if row_title:
                row_title = row_title[0][1:-1]
                if row_title not in logs:
                    logs[row_title] = {}
                attrs = line.split('\t')[1:-1]
                for attr in attrs:
                    attr = attr.split(":")
                    attr_type = attr[0].strip()
                    attr_val = parse_value(attr[1])
                    if attr_type not in logs[row_title]:
                        logs[row_title][attr_type] = []
                    logs[row_title][attr_type].append(attr_val)
    return {k: pd.DataFrame(v) for k,v in logs.items()}



def main():
    logs = load_jensenlog_file()
    for log in logs:
        print(log)
        print(logs[log])
    # mem_logs = load_memlogger_file()
    # fig, ax1 = plt.subplots()
    # ax1.plot(mem_logs['Time'], mem_logs['RES'])
    # ax2 = ax1.twinx()
    # freq = 20
    # ax2.plot(mem_logs['Time'].iloc[::freq], mem_logs['CPU'].iloc[::freq])
    # plt.show()



if __name__ == '__main__':
    main()