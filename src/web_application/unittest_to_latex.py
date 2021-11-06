from test_orm import ORMTest

def print_table_header():

    print("""
\\begin{longtable}{ |p{0.2\\textwidth}|p{0.75\\textwidth}| }
\hline
\\rowcolor{Gray}&\\\\*[-0.9em]
\\rowcolor{Gray}\\textbf{Test}&\\textbf{Description}\\\\*
\\rowcolor{Gray}&\\\\*[-0.9em]
\\hline
\\endhead
    """)

def print_row(func_name):
    print("&\\\\*")
    print("\\texttt{"+func_name.replace("_", "{\_}")+"}&\\\\*")
    print("&\\\\*")
    print("\\hline")


def main():
    symbols = dir(ORMTest)
    tests = [func for func in symbols if "test_" in func]
    print(len(tests))
    print_table_header()
    for test in tests:
        print_row(test)

if __name__ == "__main__":
    main()
