from abc import ABC, abstractmethod
from copy import copy

from lattice_animal import LatticeAnimal


class RedelmeierHandler(ABC):
    @abstractmethod
    def handle_animal(self, animal: LatticeAnimal):
        pass

    @abstractmethod
    def finalize(self):
        pass


class RedelmeierProner(ABC):
    @abstractmethod
    def check_animal(self, animal: LatticeAnimal):
        pass


class Redelmeier:
    handlers: list
    tried: set
    animal: LatticeAnimal

    def __init__(self, seed_animal: LatticeAnimal):
        self.handlers = []
        self.proners = []
        self.animal = seed_animal.__class__()

    def run(self):
        self.animal = self.animal.__class__()
        self.tried = set()
        untried = set()
        untried.add(self.animal.get_singleton())
        self.run_aux(untried)

    def register_handler(self, handler: RedelmeierHandler):
        self.handlers.append(handler)

    def register_proner(self, proner: RedelmeierProner):
        self.proners.append(proner)

    def run_handlers(self):
        handler: RedelmeierHandler
        for handler in self.handlers:
            handler.handle_animal(self.animal)

    def finalize(self):
        handler: RedelmeierHandler
        for handler in self.handlers:
            handler.finalize()

    def check_proners(self):
        return all([proner.check_animal(self.animal) for proner in self.proners])

    def run_aux(self, untried):
        while untried:
            c = untried.pop()
            self.tried.add(c)
            self.animal.add_cell(c)
            self.run_handlers()
            if self.check_proners():
                neighbors = self.get_legal_neighbors(c, untried)
                for neighbor in neighbors:
                    untried.add(neighbor)
                self.run_aux(copy(untried))
                for neighbor in neighbors:
                    if neighbor in self.tried:
                        self.tried.remove(neighbor)
                    if neighbor in untried:
                        untried.remove(neighbor)
            self.animal.remove_cell(c)

    def is_legal_and_untried(self, c, untried):
        if c in self.tried or c in untried:
            return False
        else:
            for i in range(len(c)-1, -1, -1):
                if c[i] > 0:
                    return True
                if c[i] < 0:
                    return False
        return c[0] >= 0

    def get_legal_neighbors(self, c, untried):
        neighbors = self.animal.cell_neighbors(c)
        neighbors = [c for c in neighbors if self.is_legal_and_untried(c, untried)]
        return neighbors
