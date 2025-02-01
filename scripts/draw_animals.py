import os.path
import shutil
from abc import ABC, abstractmethod
from lattice_animal import TwodAnimal


class LatexElement(ABC):
    @abstractmethod
    def get_code(self):
        pass

    @abstractmethod
    def gen_files(self, path):
        pass


class TextFileElement(LatexElement):
    def __init__(self, file_path):
        self.file_path = file_path

    def get_code(self):
        with open(self.file_path, "r") as f:
            return "".join(f.readlines())

    def gen_files(self, path):
        pass


class StringElement(LatexElement):
    def __init__(self, latex_str):
        self.latex_str = latex_str

    def get_code(self):
        return self.latex_str

    def gen_files(self, path):
        pass

class FileElement(LatexElement):
    def __init__(self, file_path):
        self.file_path = file_path

    def get_code(self):
        return ""

    def gen_files(self, path):
        f_name = os.path.basename(self.file_path)
        target_path = os.path.join(path,f_name)
        if not os.path.exists(target_path):
            shutil.copyfile(self.file_path, target_path)


class AnimalsTransformations:
    def __init__(self):
        self.scale = 1
        self.rotate = 0
        self.draw_perimeter = True
        self.draw_border = False


class AnimalsElement(LatexElement):
    animals_list: list
    names: list

    def __init__(self, transformations=None):
        self.animals = []
        self.names = []
        self.nameless_counter = 0
        if transformations is None:
            transformations = AnimalsTransformations()
        self.transformations = transformations

    def add_animal(self, animal: TwodAnimal, name = None):
        if name is None:
            name = str(self.nameless_counter)
            self.nameless_counter += 1
        self.animals.append(animal)
        self.names.append(name+".txt")

    def tikz_command(self, animal, name):
        s =  animal.get_tikz_command()
        if self.transformations.rotate != 0 or self.transformations.scale != 1:
            s += "["
            if self.transformations.rotate != 0:
                s += f"rotate = {self.transformations.rotate}, "
            if self.transformations.scale != 1:
                s += f"scale = {self.transformations.scale}, "
            s = s[:-2]
            s += "]"
        s += "{"
        s += name
        s += "}"
        return s

    def set_transformations(self, transformations):
        self.transformations = transformations

    def get_code(self):
        return "\n".join([self.tikz_command(animal, name) for animal, name in zip(self.animals, self.names)]) + "\n"

    def gen_files(self, path):
        graphics_path = os.path.join(path, "graphics")
        if not os.path.exists(graphics_path):
            os.mkdir(graphics_path)
        animal: TwodAnimal
        for animal, name in zip(self.animals, self.names):
            with open(os.path.join(graphics_path, name), "w+") as f:
                f.write(animal.tikz_string(draw_perimeter=self.transformations.draw_perimeter,
                                           draw_border=self.transformations.draw_border))


class LatexDrawer:
    header: list
    body: list
    footer: list
    path: str
    target_type: str
    verbose: bool

    def __init__(self, path, name = "main"):
        self.header = []
        self.body = []
        self.footer = []
        self.path = path
        self.name = name
        self.set_target_pdf()
        self.verbose = False

    def add_element(self, element, section = "body"):
        match section:
            case "header":
                self.header.append(element)
            case "body":
                self.body.append(element)
            case "footer":
                self.footer.append(element)

    def set_target_pdf(self):
        self.target_type = "pdf"

    def set_target_png(self):
        self.target_type = "png"

    def create_file(self):
        main_file =os.path.join(self.path, self.name+".tex")
        with open(main_file, "w+") as f:
            element: LatexElement
            for element in self.header + self.body + self.footer:
                f.write(element.get_code())
                element.gen_files(self.path)
        self.__compile()

    def __output_file(self):
        if self.verbose:
            return ""
        else:
            return " >/dev/null 2> /dev/null"

    def __compile(self):
        if self.target_type == "pdf":
            self.__compile_pdf()
        elif self.target_type == "png":
            self.__compile_png()

    def __compile_png(self):
        old_cwd = os.getcwd()
        os.chdir(self.path)
        os.system(f"pdflatex {self.name}.tex" + self.__output_file())
        os.system(f"convert -density 300 {self.name}.pdf -quality 90 {self.name}.png" + self.__output_file())
        os.chdir(old_cwd)

    def __compile_pdf(self):
        old_cwd = os.getcwd()
        os.chdir(self.path)
        os.system(f"pdflatex {self.name}.tex" + self.__output_file())
        os.chdir(old_cwd)