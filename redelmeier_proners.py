from redelmeier import RedelmeierProner
from lattice_animal import LatticeAnimal


class ProneBySize(RedelmeierProner):
    def __init__(self, n):
        self.n = n

    def check_animal(self, animal: LatticeAnimal):
        return animal.size() < self.n