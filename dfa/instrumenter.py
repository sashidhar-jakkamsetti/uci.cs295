"""
To run this script: (can run on Python 2.7)

1. Change the "file_path" and "output_file_path" according to the local system.
2. Add "(secret) " in the beginning of line 33,34,40,41 of syringePumpSim.c to test the script.
3. $ python instrumenter.py syringePumpSim.c source.c (assuming syringePumpSim.c is in the same directory as instrumenter.py)

Once the script runs, it will create a new file in the mentioned
file path named "source.c", which has the required function calls
to the stub.
"""

import collections
import pprint
import re
import sys


file_path = sys.argv[1]
output_file_path = sys.argv[2]
key_word = "(secret)"
id = 0

prime_data_variables = set()
variable_id_map = dict()
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
    global id
    with open(file_path) as code_file:
        for line in code_file:
            if(key_word in line):
                tokens = line.split(' ')
                prime_data_variables.add(tokens[2])
                id += 1
                variable_id_map[tokens[2]] = id
    
    # display_current_results()
    # identify_aliases() # To Do
    identify_dependencies()

# def identify_aliases():

def identify_dependencies():
    global id
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
                            if(tokens[idx-1] not in prime_data_variables):
                                prime_data_variables.add(tokens[idx-1])
                                id += 1
                                variable_id_map[tokens[idx-1]] = id
                else:
                    if(op in tokens):
                        idx = tokens.index(op)
                        if(tokens[idx+1] in prime_data_variables):
                            if(tokens[idx-1] not in prime_data_variables):
                                prime_data_variables.add(tokens[idx-1])
                                id += 1
                                variable_id_map[tokens[idx-1]] = id
               
    # display_current_results()

def display_current_results():
    print ("\nPrime Data Variables: ", prime_data_variables)
    pprint.pprint(variable_id_map, width=1)

def insert_stub_function_calls():
    with open(file_path) as code_file:
        with open(output_file_path, "w") as f2:
            prev_line = ""
            open_paranthesis_cnt = 0
            in_scope = False
            for line_number,line in enumerate(code_file):
                line = line.replace("(secret) ","")
                line_copy = line
                line = line.replace(";","")
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
                f2.write(line_copy)
                if("enum{" in prev_line): # need to change this.
                    f2.write("enum{DEF,USE};\n")
                    f2.write("char report_snip[100];\n")
                    f2.write("int len_report_snip = 0;\n")

                for op in operators:
                    if(op in tokens and in_scope):
                        idx = tokens.index(op)
                        for token in reversed(tokens[idx+1:]):
                            if(token in prime_data_variables):
                                # declare report_snip and len_report_snip in the top.
                                f2.write('\n\t\t\t\tsnprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: {}");\n'.format(token))
                                f2.write('\t\t\t\tlen_report_snip = (int)strlen(report_snip)+{};\n'.format(len(token)))
                                f2.write('\t\t\t\tdfa_primevariable_checker({}, (void *)&{}, sizeof({}), report_snip, len_report_snip, (int)USE);\n'.format(
                                    variable_id_map[token], token, token)
                                )
                        if(op == "="):
                            for token in reversed(tokens[:idx]):
                                if(token in prime_data_variables):
                                    f2.write('\n\t\t\t\tsnprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: {}");\n'.format(token))
                                    f2.write('\t\t\t\tlen_report_snip = (int)strlen(report_snip)+{};\n'.format(len(token)))
                                    f2.write("\t\t\t\tdfa_primevariable_checker({}, (void *)&{}, sizeof({}), report_snip, len_report_snip, (int)DEF);\n".format(
                                        variable_id_map[token], token, token)
                                    )
                        else:
                            for token in reversed(tokens[:idx]):
                                if(token in prime_data_variables):
                                    f2.write('\n\t\t\t\tsnprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: {}");\n'.format(token))
                                    f2.write('\t\t\t\tlen_report_snip = (int)strlen(report_snip)+{};\n'.format(len(token)))
                                    f2.write('\t\t\t\tdfa_primevariable_checker({}, (void *)&{}, sizeof({}), report_snip, len_report_snip, (int)USE);\n'.format(
                                        variable_id_map[token], token, token)
                                    )
                                    f2.write('\n\t\t\t\tsnprintf(report_snip, sizeof(report_snip), "%s%s%s%s%s", "File: ", __FILE__, "; Func: ", __func__, "; Var: {}");\n'.format(token))
                                    f2.write('\t\t\t\tlen_report_snip = (int)strlen(report_snip)+{};\n'.format(len(token)))
                                    f2.write("\t\t\t\tdfa_primevariable_checker({}, (void *)&{}, sizeof({}), report_snip, len_report_snip, (int)DEF);\n".format(
                                        variable_id_map[token], token, token)
                                    )
                
                prev_line = line


identify_secret_variables()
insert_stub_function_calls()

"line: 32; var_name: abc\n"