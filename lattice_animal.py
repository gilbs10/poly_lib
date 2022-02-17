from abc import ABC, abstractmethod
from copy import copy, deepcopy
from random import sample
from utils import minmax


class LatticeAnimal(ABC):
    def __init__(self, cells=None):
        self.poly_cells = set()
        self.perimeter = set()
        self.specials = set()
        if cells:
            for c in cells:
                self.add_cell(c)

    def add_cell(self, c):
        if c not in self.poly_cells:
            self.poly_cells.add(c)
            self.update_perimeter_add(c)

    def remove_cell(self, c):
        if c in self.poly_cells:
            self.poly_cells.remove(c)
            self.update_perimeter_remove(c)

    def is_empty(self):
        return bool(self.poly_cells)

    def update_perimeter_add(self, c):
        if c in self.perimeter:
            self.perimeter.remove(c)
        for neighbor in self.cell_neighbors(c):
            if neighbor not in self.poly_cells:
                self.perimeter.add(neighbor)

    def update_perimeter_remove(self, c):
        # Assuming that c is already removed from poly_cells
        disconnected = True
        for neighbor in self.cell_neighbors(c):
            if neighbor not in self.perimeter:
                disconnected = False
                continue
            to_remove = True
            for neighbor2 in self.cell_neighbors(neighbor):
                if neighbor2 in self.poly_cells:
                    to_remove = False
                    break
            if to_remove:
                self.perimeter.remove(neighbor)
        if not disconnected:
            self.perimeter.add(c)

    # Mark as special cell for display uses
    def mark_special(self, c):
        if c not in self.specials:
            self.specials.add(c)

    def unmark_special(self, c):
        if c in self.specials:
            self.specials.remove(c)

    @staticmethod
    @abstractmethod
    def cell_neighbors(c):
        pass

    @staticmethod
    @abstractmethod
    def dimensions():
        pass

    @classmethod
    def get_singleton(cls):
        return tuple([0]*cls.dimensions())

    def __eq__(self, p):
        return self.poly_cells == p.poly_cells

    def __hash__(self):
        self.canonize()
        return hash(tuple(sorted(self.poly_cells)))

    def __len__(self):
        return len(self.poly_cells)

    def size(self):
        return len(self)

    def perimeter_size(self):
        return len(self.perimeter)

    """ Defect values as described in "Enumerating Polyominoes with 
        Fixed Perimeter Defect. Asinowski, Barequet and Zheng".
    """
    def defect(self):
        return 2 * self.size() + 2 - self.perimeter_size()

    # e part of the defect (see above paper).
    def defect_e(self):
        e = 0
        for c in self.perimeter:
            e -= 1
            for neighbor in self.cell_neighbors(c):
                if neighbor in self.poly_cells:
                    e += 1
        return e

    # f part of the defect (see above paper).
    def defect_f(self):
        return int((self.defect() - self.defect_e()) / 2)

    """ Inflation and deflation of polyominoes as described in 
        "Properties of Minimal Perimeter Polyominoes. Barequet and Ben-Shachar".
    """
    def inflate(self):
        for c in copy(self.perimeter):
            self.add_cell(c)

    def deflate(self):
        for c in self.border():
            self.remove_cell(c)

    def border(self):
        b = []
        for c in self.poly_cells:
            for neighbor in self.cell_neighbors(c):
                if neighbor in self.perimeter:
                    b.append(c)
                    break
        return b

    def border_size(self):
        return len(self.border())

    def add_random(self):
        if self.poly_cells:
            self.add_cell(sample(self.perimeter, 1)[0])
        else:
            self.add_cell(self.get_singleton())

    def remove_random(self):
        self.remove_cell(sample(self.poly_cells, 1)[0])

    def width(self):
        if self.poly_cells:
            return max([p[0] for p in self.poly_cells]) - min([p[0] for p in self.poly_cells]) + 1
        return 0

    def height(self):
        if self.poly_cells:
            return max([p[1] for p in self.poly_cells]) - min([p[1] for p in self.poly_cells]) + 1
        return 0

    def diag_width_1(self):
        if self.poly_cells:
            return max([p[0] + p[1] for p in self.poly_cells]) - min([p[0] + p[1] for p in self.poly_cells]) + 1
        return 0

    def diag_width_2(self):
        if self.poly_cells:
            return max([p[0] - p[1] for p in self.poly_cells]) - min([p[0] - p[1] for p in self.poly_cells]) + 1
        return 0

    def width3(self):
        if self.poly_cells:
            return max([p[2] for p in self.poly_cells]) - min([p[2] for p in self.poly_cells]) + 1
        return 0

    def jensen_pos(self):  # The index in which Jensen alg will recognize the poly
        if self.poly_cells:
            max_x = max([p[0] for p in self.poly_cells])
            return min([p[1] for p in self.poly_cells if p[0] == max_x]) - min([p[1] for p in self.poly_cells]) + 1
        return 0

    def sum_of_degrees(self):
        d = 4 * (self.size() - self.border_size())
        for c in self.border():
            for neighbor in self.cell_neighbors(c):
                if neighbor in self.poly_cells:
                    d += 1
        return d

    def rotate(self):
        x_shift = max([p[0] for p in self.poly_cells])
        y_shift = min(p[1] for p in self.poly_cells if p[0] == x_shift)
        return self.__class__([(p[1] - y_shift, x_shift - p[0]) for p in self.poly_cells])

    def mirror(self):
        x_shift = max(p[0] for p in self.poly_cells if p[1] == 0)
        return self.__class__([(x_shift - p[0], p[1]) for p in self.poly_cells])

    def mirror_self(self):
        x_shift = max(p[0] for p in self.poly_cells if p[1] == 0)
        self.rebuild_poly([(x_shift - p[0], p[1]) for p in self.poly_cells])

    def rotate_self(self):
        x_shift = max([p[0] for p in self.poly_cells])
        y_shift = min(p[1] for p in self.poly_cells if p[0] == x_shift)
        self.rebuild_poly([(p[1] - y_shift, x_shift - p[0]) for p in self.poly_cells])

    def rebuild_poly(self, new_cells):
        cur_cells = copy(self.poly_cells)
        for c in cur_cells:
            self.remove_cell(c)
        for c in new_cells:
            self.add_cell(c)

    def canonize(self):
        if self.poly_cells:
            min_y = min(c[1] for c in self.poly_cells)
            min_x = min(c[0] for c in self.poly_cells if c[1] == min_y)
            self.rebuild_poly(sorted([(c[0] - min_x, c[1] - min_y) for c in self.poly_cells]))

    def tikz_string(self, draw_border=False, draw_perimeter=True, cell_text=None):
        if cell_text is None:
            cell_text = {}

        def tikz_cell_string(x, y, c_type, text):
            return f"({x},{y},{c_type},{text}),"
        self.canonize()
        s = ""
        border_cells = self.border()
        for c in self.poly_cells - self.specials:
            if draw_border and c in border_cells:
                cell_type = "b"
            else:
                cell_type = "*"
            s += tikz_cell_string(c[0], c[1], cell_type, cell_text.get(c, ""))
        if draw_perimeter:
            for c in self.perimeter:
                s += tikz_cell_string(c[0], c[1], "p", cell_text.get(c, ""))
        for c in self.specials:
            s += tikz_cell_string(c[0], c[1], "s", cell_text.get(c, ""))
        return s[:-1]  # Remove last ","

    # BFS over the lattice animal
    def split_to_connected(self):
        visited = set()
        to_visit = set()
        res = []
        for c in self.poly_cells:
            if c not in visited:
                p = self.__class__()
                to_visit.add(c)
                while to_visit:
                    cur_c = to_visit.pop()
                    p.add_cell(cur_c)
                    visited.add(cur_c)
                    for nei_c in self.cell_neighbors(cur_c):
                        if nei_c in self.poly_cells and nei_c not in visited:
                            to_visit.add(nei_c)
                res.append(deepcopy(p))
        return res

    def is_connected(self):
        return len(self.split_to_connected()) == 1

    def __str__(self):
        if len(self.poly_cells) == 0:
            return "Empty\n"
        min_x, max_x = minmax([c[0] for c in self.poly_cells])
        min_y, max_y = minmax([c[1] for c in self.poly_cells])
        max_c = max(self.poly_cells)
        s = ""
        for y in range(max_y, min_y - 1, -1):
            for x in range(min_x, max_x + 1):
                if (x, y) == max_c:
                    s += "#"
                else:
                    s += "*" if (x, y) in self.poly_cells else " "
            s += "\n"
        return s

    @staticmethod
    @abstractmethod
    def get_counts():
        pass


class TwodAnimal(LatticeAnimal):

    @staticmethod
    def dimensions():
        return 2

    @staticmethod
    @abstractmethod
    def get_tikz_command():  # Tikz polydraw module supports all 2d animals
        pass


class Polyomino(TwodAnimal):

    @staticmethod
    def cell_neighbors(c):
        x = c[0]
        y = c[1]
        return [(x - 1, y), (x, y - 1), (x + 1, y), (x, y + 1)]

    @staticmethod
    def get_counts():
        return [1, 2, 6, 19, 63, 216, 760, 2725, 9910, 36446, 135268,
                505861, 1903890, 7204874, 27394666, 104592937,
                400795844, 1540820542, 5940738676, 22964779660,
                88983512783, 345532572678, 1344372335524, 5239988770268,
                20457802016011, 79992676367108, 313224032098244,
                1228088671826973, 4820975409710116, 18946775782611174,
                74541651404935148, 293560133910477776, 1157186142148293638,
                4565553929115769162, 18027932215016128134, 71242712815411950635,
                281746550485032531911, 1115021869572604692100,
                4415695134978868448596, 17498111172838312982542,
                69381900728932743048483, 275265412856343074274146,
                1092687308874612006972082, 4339784013643393384603906,
                17244800728846724289191074, 68557762666345165410168738,
                272680844424943840614538634, 1085035285182087705685323738,
                4319331509344565487555270660, 17201460881287871798942420736,
                68530413174845561618160604928, 273126660016519143293320026256,
                1088933685559350300820095990030, 4342997469623933155942753899000,
                17326987021737904384935434351490, 69150714562532896936574425480218]

    @staticmethod
    def get_tikz_command():
        return "\drawpoly"


class Polyhex(TwodAnimal):

    @staticmethod
    def cell_neighbors(c):
        x = c[0]
        y = c[1]
        return [(x, y + 1), (x + 1, y + 1), (x + 1, y), (x, y - 1), (x - 1, y - 1), (x - 1, y)]

    @staticmethod
    def get_counts():
        return [1, 3, 11, 44, 186, 814, 3652, 16689, 77359, 362671,
                1716033, 8182213, 39267086, 189492795, 918837374,
                4474080844, 21866153748, 107217298977, 527266673134,
                2599804551168, 12849503756579, 63646233127758,
                315876691291677, 1570540515980274, 7821755377244303,
                39014584984477092, 194880246951838595,
                974725768600891269, 4881251640514912341,
                24472502362094874818, 122826412768568196148,
                617080993446201431307, 3103152024451536273288,
                15618892303340118758816, 78679501136505611375745]

    @staticmethod
    def get_tikz_command():
        return "\drawpolyhex"


class Polycube(LatticeAnimal):

    @staticmethod
    def cell_neighbors(c):
        x = c[0]
        y = c[1]
        z = c[2]
        return [(x-1,y,z),(x+1,y,z),(x,y-1,z),(x,y+1,z),(x,y,z-1),(x,y,z+1)]

    @staticmethod
    def dimensions():
        return 3

    @staticmethod
    def get_counts():
        return [1, 3, 15, 86, 534, 3481, 23502, 162913, 1152870,
                8294738, 60494549, 446205905, 3322769321,
                24946773111, 188625900446, 1435074454755,
                10977812452428, 84384157287999, 651459315795897]
