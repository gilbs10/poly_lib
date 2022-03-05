target_w = 29
min_n = target_w
max_n = 57
ns = range(min_n, max_n+1)
cpus = 31
# exclude_widths = [31,32] + list(range(52,max_n+1))
exclude_widths = list(range(target_w)) + list(range(target_w+1,max_n+1))
# exclude_widths = list(range(30,max_n+1))
# exclude_widths = list(range(52))
c = 0
for n in ns:
    for w in range(2,max_n+1):
        if w in exclude_widths:
            continue
        if w % 2 == 1:
            c += 2
        else:
            c += 1
print(c)
for n in ns:
    for w in range(2,max_n+1):
        if w in exclude_widths:
            continue
        if w%2 == 0:
            wms = ['n']
        else:
            wms = ['y','n']
        for wm in wms:
            print('r')
            print(n)
            print(w)
            print(wm)
            print(cpus)
