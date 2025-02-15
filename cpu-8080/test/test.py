#! python
import sys
import os
import traceback
from ctypes import CDLL, byref
import yaml
from asm import asm
from emu8080 import Emu8080Context, flag_map, reg_map


class TestException(Exception):
    """Text specific exception"""


def expected_memory_location(location: int, data: list, context: Emu8080Context):
    for i, expected in enumerate(data):
        actual = ord(context.memory.contents[location + i])
        if expected != actual:
            raise TestException(
                f"memory location {location + i} expected: {expected} found: {actual}")


def expected_memory(expected: list, context: Emu8080Context):
    for e in expected:
        expected_memory_location(e["location"], e["data"], context)


def expected_flags(flags: dict, context: Emu8080Context):
    for flag in flags:
        index = flag_map[flag.upper()]
        expected = flags[flag] != 0
        actual = context.flags[index] != 0
        if expected != actual:
            raise TestException(
                f"flag '{flag}' expected: {expected}, found: {actual}")


def expected_registers(registers: dict, context: Emu8080Context):
    for reg in registers:
        if reg == 'sp':
            actual = context.SP
        elif reg == 'pc':
            actual = context.PC
        else:
            actual = context.registers[reg_map[reg]]
        expected = registers[reg]
        if expected != actual:
            raise TestException(
                f"register '{reg}' expected: {expected} found: {actual}")


def check_results(expected: dict, context: Emu8080Context):
    if 'registers' in expected:
        expected_registers(expected['registers'], context)
    if 'memory' in expected:
        expected_memory(expected['memory'], context)
    if 'flags' in expected:
        expected_flags(expected['flags'], context)


def test_init_memory(locations: list, context: Emu8080Context):
    for location in locations:
        offset = location['location']
        for i, value in enumerate(location['data']):
            context.memory.contents[offset + i] = value


def test_init_flags(flags: dict, context: Emu8080Context):
    for flag in flags:
        context.flags[flag_map[flag.upper()]] = flags[flag]


def test_init_registers(registers: dict, context: Emu8080Context):
    for reg in registers:
        if reg == "pc":
            context.PC = registers[reg]
        elif reg == "sp":
            context.SP = registers[reg]
        elif reg == "halt":
            context.halt = registers[reg]
        else:
            context.registers[reg_map[reg]] = registers[reg]


def test_init_state(state: dict, context: Emu8080Context):
    if 'registers' in state:
        test_init_registers(state['registers'], context)
    if 'memory' in state:
        test_init_memory(state['memory'], context)
    if 'flags' in state:
        test_init_flags(state['flags'], context)


def test_reset_state(context: Emu8080Context):
    for i in range(0, 32768):
        context.memory.contents[i] = 0
    for i in range(0, 8):
        context.registers[i] = 0
    context.PC = 0
    context.SP = 0


def asm_code(instructions: list) -> list:
    buffer = []
    for line in instructions:
        code = asm(line)
        if len(code) == 0:
            raise TestException("invalid instruction: {0} {1}".format(
                line, sys._getframe().f_code.co_name))
        buffer += code
    buffer += [0b01110110]  # HLT
    return buffer


def run_test_code(emu8080: CDLL, instructions: list, context: Emu8080Context):
    code = asm_code(instructions)
    print(code, len(instructions))
    for i, b in enumerate(code):
        context.memory.contents[i] = b
    print(instructions)
    max_loops = 1000
    while max_loops > 0 and context.halt == 0:
        context.print()
        max_loops -= 1
        emu8080.emu_8080_execute(byref(context))


def run_test(emu8080: CDLL, test: dict, context: Emu8080Context):
    context.halt = 0
    if (not 'reset' in test) or test['reset']:
        test_reset_state(context)
    if 'state' in test:
        test_init_state(test['state'], context)
    run_test_code(emu8080, test['code'], context)
    check_results(test['expected'], context)


def run_tests(emu8080, tests: dict, context: Emu8080Context):
    fail_count = 0
    for test in tests:
        try:
            print(f"executing : '{test['name']} '")
            run_test(emu8080, test, context)
            print(f"success : '{test['name']}'")

        except TestException as ex:
            fail_count += 1
            print(f"failed : '{test['name']}'")
            context.print()
            for msg in ex.args:
                tb = sys.exc_info()[-1]
                print(tb)
                stk = traceback.extract_tb(tb, 1)
                fname = stk[0][2]
                print(f"\t- {msg} in {fname}")
        finally:
            print("--------------------------")
    print(
        f"total: {len(tests)} success: {len(tests)-fail_count} failed: {fail_count}")


def execute(emu8080: CDLL):

    tests = None
    with open(f'{os.path.dirname(__file__)}/test.yml', 'r', encoding='utf-8') as stream:
        try:
            tests = yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            print(exc)
            exit(1)

    context = Emu8080Context()

    run_tests(emu8080, tests, context)


if __name__ == '__main__':
    from ctypes import cdll
    execute(emu8080=cdll.LoadLibrary(
        f"{os.path.dirname(__file__)}/{sys.argv[1]}"))

# print("size of python context ",sizeof(Emu8080Context))
# context.reg[7] = 9
# emu_8080_context_init = lib.emu_8080_context_init
# emu_8080_context_init.argtypes = [POINTER(Emu8080Context), c_int]
# emu_8080_context_init(byref(context), 100)
# print(context.memory.contents)
# print(memory.contents[1])
