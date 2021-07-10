from pprint import pprint

class Counter():

    """keep track of function calls"""

    def __init__(self, f):

        self._f = f
        self._uses = 0

    def __call__(self, **kwargs):

        self._uses += 1
        print(f"\nnum_uses: {self._uses}")

        self._f(kwargs)

@Counter
def print_data(*args):

    """print variables and wait for input"""
    for key, value in args[0].items():
        print (f"{key}: {value}")

    print("\n")
    input()




