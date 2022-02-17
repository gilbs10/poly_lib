from redelmeier import RedelmeierHandler
from lattice_animal import LatticeAnimal

class CountAllHandler(RedelmeierHandler):
    counts: dict

    def __init__(self):
        self.counts = {}

    def handle_animal(self, animal: LatticeAnimal):
        if animal.size() not in self.counts:
            self.counts[animal.size()] = 0
        self.counts[animal.size()] += 1

    def finalize(self):
        i = 1
        while i in self.counts:
            print(f"{i}: {self.counts[i]}")
            i += 1


class PrintProgressHandler(RedelmeierHandler):
    def __init__(self, n, seed: LatticeAnimal, res = 100000.0):
        counts = seed.get_counts()
        self.target = sum(counts[0:n])
        self.res = res
        self.count = 0
        self.perc = 0

    def handle_animal(self, _):
        self.count += 1
        if self.res * self.count / self.target > self.perc:
            print('\r' + str(100 * self.perc / self.res) + "      ", end="", flush=True)
            self.perc = self.res * self.count / self.target

    def finalize(self):
        print("")

