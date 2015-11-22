#!/usr/bin/env python

import os
import subprocess
from time import sleep


def test_apply_protection(test):
    application_cmd = "./protector " + test + ".c"
    p = subprocess.Popen(application_cmd, shell=True, stdin=None,
                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=True)

    compile_output = p.stdout.read()
    if compile_output != "":
        print ("you failed to protect " + test + " (error during protection of the test)")
        print compile_output
        return "FAIL"

    # print ("PASS APPLY " + test)
    return "PASS"


def test_comile(test):
    protected_test_file = "protected_" + test + ".c"
    compilation_cmd = "gcc -fno-stack-protector -O0 -z execstack -o protected_prog " + protected_test_file + " -g3"
    p = subprocess.Popen(compilation_cmd, shell=True, stdin=None,
                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=True)
    compile_output = p.stdout.read()
    if compile_output != "":
        print (test + " failed to compile after protection.")
        print compile_output
        return "FAIL"
    return "PASS"


def test_behaviour(test):
    input_file = open(test + ".in", 'r+')
    output_file = open(test + ".out", 'w+')
    err_file = open(test + ".errors", 'w+')
    subprocess.Popen("./protected_prog", shell=True, stdin=input_file,
                     stdout=output_file, stderr=err_file, close_fds=True)
    # subprocess.Popen("./protected_prog < "+test+".in > "+test+".out 2>"+test+".errors", shell=True)


    output_file.flush()
    err_file.flush()
    os.fsync(output_file.fileno())
    os.fsync(err_file.fileno())
    sleep(1)
    diff_fails = ""
    if os.system("diff " + test + ".out.expected " + test + ".out") != 0:
        print "diff " + test + ".out.expected " + test + ".out"
        print os.system("diff " + test + ".out.expected " + test + ".out")
        diff_fails += "output, "
    if os.system("diff " + test + ".errors.expected " + test + ".errors") != 0:
        diff_fails += "errors"
    input_file.close()
    output_file.close()
    err_file.close()
    if diff_fails != "":
        print (test + " behaviour changed! diff on " + diff_fails)
        return "FAIL"
    return "PASS"


def test_overflow(test):
    input_file = open(test + ".in.overflow", 'r')
    output_file = open(test + ".out.overflow", 'w')

    subprocess.Popen("./protected_prog", shell=True, stdin=input_file,
                     stdout=output_file, stderr=None, close_fds=True)

    output_file.flush()
    os.fsync(output_file.fileno())
    input_file.close()
    output_file.close()
    sleep(1)
    p = subprocess.Popen('grep "Alert! Buffer Overflow detected." ' + test + '.out.overflow', shell=True, stdout=subprocess.PIPE)
    warning = p.stdout.read()
    if warning == "":
        print ("failed to detect buffer overflow on " + test)
        return "FAIL"
    return "PASS"

# def test_leaks(test):


os.system("rm tests/protected_*")
p = subprocess.Popen("gcc -o protector *.c -g3", shell=True, stdin=subprocess.PIPE,
                     stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=True)
compile_output = p.stdout.read()
if compile_output != "":
    print "your code failed to compile!"
    print compile_output

os.system("cp ./protector ./tests/")
os.chdir("./tests")
subprocess.Popen("dos2unix *", shell=True, stdin=None,
                 stdout=None, stderr=None)

test_list = []
for file in os.listdir("."):
    if file.endswith(".c"):
        print(file)
        test_list.append(file.split('.')[0])

for test in test_list:
    ret_value = test_apply_protection(test)
    if ret_value == "PASS":
        ret_value = test_comile(test)
    if ret_value == "PASS":
        ret_value = test_behaviour(test)
    if ret_value == "PASS":
        ret_value = test_overflow(test)
    if ret_value == "PASS":
        print (test + " - PASS!")
