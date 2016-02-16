# run-tests.py
# Very simple test runner that compare testcase output with expected results
# Inspired by Duktape tests
# Copyright (c) 2016 Laurent Zubiaur
# MIT License (http://opensource.org/licenses/MIT)
from __future__ import print_function

import sys, argparse, subprocess
from hashlib import md5
from os import path
from difflib import Differ, unified_diff, SequenceMatcher
from os import path, environ

debug = False

def trace(*arg):
    if debug:
        print(*arg)

def parse_test_case_coffee(filename):
    data, record = [], False
    m = md5()
    with open(filename, 'r') as source:
        for line in source:
            if line.startswith('### @test'):
                record = True
            elif record and line == '###\n':
                record = False
            elif record:
                m.update(line.encode('utf-8'))
                data.append(line)
    trace(*data)
    return {
        'filename': filename,
        'basename': path.basename(filename),
        'md5': m.hexdigest(),
        'data': data
    }

def parse_test_case(filename):
    ext = path.splitext(filename)[1]
    trace(filename)
    if ext == '.coffee':
        return parse_test_case_coffee(filename)
    # elif ext == 'c':
    else:
        raise NameError('File type {0} not supported'.format(ext))

def open_process(cmd):
    env = environ.copy()
    proc = subprocess.Popen(cmd,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        env=env)
    # proc.stdin.close()

    m = md5()
    output = []
    for line in proc.stdout:
        m.update(line)
        output.append(line)
    # while proc.poll() is None:
    #     output.append(proc.stdout.readline())
    # proc.stdout.close()
    proc.wait() # Must wait for the process to finish in order to get its return code

    return {
        'data': output,
        'md5': m.hexdigest(),
        'code': proc.returncode
    }

def run_test(source, cmd):
    testcase = parse_test_case(source)
    output = open_process(cmd)

    trace(testcase)
    trace(output)

    if testcase['md5'] == output['md5']:
        print('*** PASS : {0[basename]}'.format(testcase))
    else:
        print('*** FAIL : {0[basename]}'.format(testcase))
        for line in unified_diff(testcase['data'], output['data']):
            sys.stdout.write(line)
    # s = SequenceMatcher(None, testcase['data'], output['data'])
    # if s.ratio() < 1.0:
    #     print('Match failed. Ratio:', s.quick_ratio())

parser = argparse.ArgumentParser()
parser.add_argument('-s', '--source', help='Source file to parse to get the expected test output')
parser.add_argument('-d', '--debug', action='store_true', help='Enable this script debug logging')
# Rest of the command line are passed to the test process
parser.add_argument('cmd', nargs=argparse.REMAINDER)

args = parser.parse_args()
debug = args.debug
run_test(args.source, args.cmd)
