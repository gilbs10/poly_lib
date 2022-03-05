import re
from dataclasses import dataclass
from datetime import datetime
import matplotlib.pyplot as plt
import pandas
import pandas as pd

def load_memlogger_file(path):
    logs = {}
    with open(path, "r") as f:
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

def load_jensenlog_file(path):
    logs = {}
    with open(path, "r") as f:
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
    mem_logger_file = "memlogger.9.log"
    # jensenlog_files = ['out57_31n.txt', 'out57_31y.txt', 'out57_32n.txt', 'out57_32y.txt', 'res_rect_57_all_32G.txt', 'res_52p.txt']
    jensenlog_files = [f'out60_{n}.txt' for n in range(30,37,2)]
    jensenlog_files += [f'out60_{n}n.txt' for n in range(31,36,2)]
    jensenlog_files += [f'out60_{n}y.txt' for n in range(31,36,2)]
    jensenlog_files += ['res60_29m.txt', 'res60_37p.txt', 'res60_52p.txt', 'res60_51n.txt']
    counts = {}
    for jlf in jensenlog_files:
        print(jlf)
        logs = load_jensenlog_file(jlf)
        res_log = logs['RESULT WIDTH COL']
        for i, row in res_log.iterrows():
            k = (row['Res_Size'], row['Width'], row['Column'], row['White mode'])
            if(row['Count']):
                # print(k, row['Count'])
                if k in counts:
                    if counts[k] != row['Count']:
                        print("Ahhhh", counts[k], row['Count'])
                        exit(1)
                counts[k] = row['Count']
    max_n = 61
    for n in range(3, max_n+1):
        c = 0
        for w in range(2, n+1):
            for col in range(w, n+2):
                for wm in [0, 1]:
                    k = (n, w, col, wm)
                    if k not in counts:
                        pass
                        # print("Missing: ", k)
                    else:
                        # print(k, c)
                        c += counts[k]
                        if(col > w):
                            c += counts[k]
                    if w%2 == 0:
                        k = (n, w, col, 0 if wm == 1 else 1)
                        if k not in counts:
                            pass
                            # print("Missing: ", k)
                        else:
                            # print(k, c)
                            c += counts[k]
                            if (col > w):
                                c += counts[k]
        print(n,c)

    # mem_logs = load_memlogger_file()
    # fig, ax1 = plt.subplots()
    # ax1.plot(mem_logs['Time'], mem_logs['RES'])
    # ax2 = ax1.twinx()
    # freq = 20
    # ax2.plot(mem_logs['Time'].iloc[::freq], mem_logs['CPU'].iloc[::freq])
    # plt.show()



if __name__ == '__main__':
    main()