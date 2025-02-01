from draw_animals import *
from copy import deepcopy

from lattice_animal import Polyomino, Polyhex
from redelmeier import RedelmeierHandler, Redelmeier
from redelmeier_proners import ProneBySize
from redelmeier_handlers import PrintProgressHandler


def get_basic_drawer_template(path, name = "main"):
    drawer = LatexDrawer(path, name)
    drawer.add_element(TextFileElement(os.path.join("latex_drawer", "header.tex")), section="header")
    drawer.add_element(FileElement(os.path.join("latex_drawer", "polylib.sty")), section="header")
    drawer.add_element(TextFileElement(os.path.join("latex_drawer", "footer.tex")), section="footer")
    return drawer


def get_standalone_template(path, name = "main"):
    drawer = LatexDrawer(path, name)
    drawer.add_element(TextFileElement(os.path.join("latex_drawer", "standalone_header.tex")), section="header")
    drawer.add_element(FileElement(os.path.join("latex_drawer", "polylib.sty")), section="header")
    drawer.add_element(TextFileElement(os.path.join("latex_drawer", "footer.tex")), section="footer")
    return drawer


class DrawByCategory(RedelmeierHandler):
    def __init__(self, categorizer, path, transformations):
        self.categorizer = categorizer
        self.categories = {}
        self.path = path
        self.transformations = transformations

    def handle_animal(self, animal):
        category, subcategory = self.categorizer(animal)
        if category == "":
            return
        if category not in self.categories:
            self.categories[category] = {}
        if subcategory not in self.categories[category]:
            self.categories[category][subcategory] = []
        self.categories[category][subcategory].append(deepcopy(animal))

    def finalize(self):
        for category in self.categories:
            drawer = get_basic_drawer_template(self.path, category)
            for subcategory in self.categories[category]:
                drawer.add_element(StringElement("\\section{Categoty: "+subcategory+" Count: "
                                                 +str(len(self.categories[category][subcategory]))+"}"))
                animals_element = AnimalsElement(transformations=self.transformations)
                for i, animal in enumerate(self.categories[category][subcategory]):
                    animals_element.add_animal(animal, f"{category}_{subcategory}_{i}")
                drawer.add_element(animals_element)
                drawer.add_element(StringElement("\\newpage"))
            drawer.create_file()


class SavePics(RedelmeierHandler):
    def __init__(self, categorizer, path, transformations):
        self.categorizer = categorizer
        self.categories = {}
        self.path = path
        self.transformations = transformations

    def handle_animal(self, animal):
        category, subcategory = self.categorizer(animal)
        if category == "":
            return
        category_path = os.path.join(self.path, category)
        if category not in self.categories:
            self.categories[category] = {}
            if not os.path.exists(category_path):
                os.mkdir(category_path)
        subcategory_path = os.path.join(category_path, subcategory)
        if subcategory not in self.categories[category]:
            self.categories[category][subcategory] = 0
            if not os.path.exists(subcategory_path):
                os.mkdir(subcategory_path)
        drawer = get_standalone_template(subcategory_path, str(self.categories[category][subcategory]))
        self.categories[category][subcategory] += 1
        drawer.set_target_png()
        # drawer.verbose = True
        animal_element = AnimalsElement(self.transformations)
        animal_element.add_animal(animal)
        drawer.add_element(animal_element)
        drawer.create_file()

    def finalize(self):
        for category in self.categories:
            for subcategory in self.categories[category]:
                path = os.path.join(self.path, category, subcategory)
                for ext in ["aux", "log", "pdf", "tex"]:
                    files = f"*.{ext}"
                    os.system(f"rm {os.path.join(path,files)}")
                g_files = "graphics"
                os.system(f"rm -rf {os.path.join(path, g_files)}")
                lib_file = "polylib.sty"
                os.system(f"rm {os.path.join(path, lib_file)}")


def size_categorizer(animal: TwodAnimal):
    return "all", str(animal.size())


def polynomial_categorizer(animal: TwodAnimal):
    w = animal.diag_width_1()
    h = animal.diag_width_2()
    n = animal.size()
    if n-w <= 4 and h >= w:
        return f"{n-w}_{n-h}", str(n)
    else:
        return "", ""


def draw_polynomial_categories():
    n = 11
    seed = Polyomino()
    transformations = AnimalsTransformations()
    transformations.rotate = 45
    transformations.draw_perimeter = False
    drawer = DrawByCategory(polynomial_categorizer, "latex_try", transformations)

    redl = Redelmeier(seed)
    redl.register_proner(ProneBySize(n))
    redl.register_handler(PrintProgressHandler(n, seed))
    redl.register_handler(drawer)
    redl.run()
    redl.finalize()

def save_singles():
    n = 7
    seed = Polyomino()
    transformations = AnimalsTransformations()
    transformations.rotate = 45
    transformations.draw_perimeter = False
    drawer = SavePics(polynomial_categorizer, "latex_try", transformations)

    redl = Redelmeier(seed)
    redl.register_proner(ProneBySize(n))
    redl.register_handler(PrintProgressHandler(n, seed))
    redl.register_handler(drawer)
    redl.run()
    redl.finalize()

if __name__ == '__main__':
    save_singles()
