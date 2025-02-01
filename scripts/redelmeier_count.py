from redelmeier import Redelmeier
from redelmeier_proners import ProneBySize
from redelmeier_handlers import CountAllHandler, PrintProgressHandler
from lattice_animal import *

def main():
    n = 8
    seed = Polycube()
    redl = Redelmeier(seed)
    redl.register_handler(PrintProgressHandler(n, seed))
    redl.register_handler(CountAllHandler())
    redl.register_proner(ProneBySize(n))
    redl.run()
    redl.finalize()

if __name__ == '__main__':
    main()