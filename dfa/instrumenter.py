import collections
import pprint
import re

file_path = "/Users/siddharthnarasimhan/uci.cs295/dfa/syringePumpSim.c"
output_file_path = "/Users/siddharthnarasimhan/uci.cs295/dfa/source.c"
key_word = "(secret)"

prime_data_variables = set()
variable_type_map = dict()
data_types = set([
    "int",
    "char",
    "float",
    "double",
    "long",
    ]) # add more data types
operators = set([
    "=",
    "+=",
    "-=",
    "*=",
    "/=",
])


# Assumes that secret variable is always declared as (secret) followed by variable declaration. 
# Also assumes that the variable declaration is always on a new line.
def identify_secret_variables():
    with open(file_path) as code_file: # Does not load entire file into memory. Good for large files.
        for line in code_file:
            if(key_word in line):
                tokens = line.split(' ')
                prime_data_variables.add(tokens[2])
    
    display_current_results()
    # identify_aliases() # To Do
    identify_dependencies()

# def identify aliases():



def identify_dependencies():
    with open(file_path) as code_file:
        # File can be scanned twice to include potential dependencies missed out in the first iteration.
        # may be once top-down, and once bottom-up
        # To Do
        for line in code_file:
            tokens = []
            tokens.extend(line.strip().strip(';').split(' '))
            for op in operators:
                if(op == "="):
                    if(op in tokens):
                        idx = tokens.index(op)
                        if(len(set(tokens[idx+1:]).intersection(prime_data_variables)) > 0):
                            prime_data_variables.add(tokens[idx-1])
                else:
                    if(op in tokens):
                        idx = tokens.index(op)
                        if(tokens[idx+1] in prime_data_variables):
                            prime_data_variables.add(tokens[idx-1])
               
    generate_variable_type_map()
    display_current_results()

def display_current_results():
    print "\nPrime Data Variables: ",prime_data_variables
    pprint.pprint(variable_type_map, width=1)

def generate_variable_type_map():
    for var in prime_data_variables:
        with open(file_path) as code_file:
            for line in code_file:
                tokens = []
                tokens.extend(line.strip().strip(';').split(' '))
                if(var in tokens and tokens.index(var) > 0):
                    if(tokens[tokens.index(var)-1] in data_types):
                        variable_type_map[var] = tokens[tokens.index(var)-1]

# To Do: add neccessary include
def insert_stub_function_calls():
    with open(file_path) as code_file:
        with open(output_file_path, "w") as f2:
            prev_line = ""
            open_paranthesis_cnt = 0
            in_scope = False
            for line_number,line in enumerate(code_file):
                line = re.sub('//.*?(\r\n?|\n)|/\*.*?\*/', '\n', line, flags=re.S)
                line = line.replace("(secret) ","")
                tokens = []
                tokens.extend(line.strip().strip(';').split(' '))
                if("{" in line):
                    open_paranthesis_cnt += 1
                    in_scope = True
                if("}" in line):
                    if(open_paranthesis_cnt > 1):
                        open_paranthesis_cnt -= 1
                    else:
                        open_paranthesis_cnt = 0
                        in_scope = False
                if(len(line) > 1):
                    f2.write(line)
                if("enum{PUSH,PULL};" in prev_line):
                    f2.write("enum{DEF,USE};\n")

                for op in operators:
                    if(op in tokens and in_scope):
                        idx = tokens.index(op)
                        for token in reversed(tokens[idx+1:]):
                            if(token in variable_type_map):
                                f2.write("\t\t\t\tprimeVariableChecker({}, USE);\n".format(token))
                        if(op == "="):
                            for token in reversed(tokens[:idx]):
                                if(token in variable_type_map):
                                    f2.write("\t\t\t\tprimeVariableChecker({}, DEF);\n".format(token))
                        else:
                            for token in reversed(tokens[:idx]):
                                if(token in variable_type_map):
                                    f2.write("\t\t\t\tprimeVariableChecker({}, USE);\n".format(token))
                                    f2.write("\t\t\t\tprimeVariableChecker({}, DEF);\n".format(token))
                
                prev_line = line


identify_secret_variables()
insert_stub_function_calls()