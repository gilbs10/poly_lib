min_n = 57
max_n = 57
ns = range(min_n, max_n+1)
cpus = 3
# exclude_widths = [31,32] + list(range(52,max_n+1))
exclude_widths = list(range(52))
c = 0
for n in ns:
    for w in range(2,max_n+1):
        if w in exclude_widths:
            continue
        for wm in ['y','n']:
            c += 1
print(c)
for n in ns:
    for w in range(2,max_n+1):
        if w in exclude_widths:
            continue
        for wm in ['y','n']:
            print('r')
            print(n)
            print(w)
            print(wm)
            print(cpus)
