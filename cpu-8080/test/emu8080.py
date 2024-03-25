""" Binding to cpu-8080 dynamic library"""
from ctypes import c_int, c_char, c_ubyte, c_void_p, \
    CFUNCTYPE, POINTER, pointer, create_string_buffer, \
    Structure

REG_B = 0
REG_C = 1
REG_D = 2
REG_E = 3
REG_H = 4
REG_L = 5
REG_M = 6
REG_A = 7

Z_FLAG = 0
C_FLAG = 1
P_FLAG = 2
S_FLAG = 3
A_FLAG = 4

reg_map = {
    'a': REG_A,
    'b': REG_B,
    'c': REG_C,
    'd': REG_D,
    'e': REG_E,
    'h': REG_H,
    'l': REG_L,
    'm': REG_M,
}

flag_map = {
    'Z': Z_FLAG,
    'C': C_FLAG,
    'P': P_FLAG,
    'S': S_FLAG,
    'A': A_FLAG,
}

reg8_t = c_ubyte
reg16_t = c_int


class Emu8080Context(Structure):
    """interface to the C structure"""
    _fields_ = [
        ("registers", reg8_t * 8),
        ("SP", reg16_t),
        ("PC", reg16_t),
        ("flags", c_int * 6),
        ("halt", c_int),
        ("interrupt", c_int),
        ("memory", POINTER(c_char * 32768)),
        ("port_read", CFUNCTYPE(c_void_p, c_int)),
        ("port_write", CFUNCTYPE(c_void_p, c_int, c_int)),
        ('address_mask', c_int),
        ('rom_size', c_int),
        ('M', reg8_t),
        ('gData', c_void_p),
    ]

    def __init__(self):
        super().__init__()
        self.memory = pointer(create_string_buffer(32768))
        self.address_mask = 0x3FFF
        self.rom_size = 0

    def print(self):
        m_addr = self.registers[REG_L] + (self.registers[REG_H] * 256)
        m_val = ord(self.memory.contents[m_addr & 0x7fff])
        print("REG: a  b  c  d  e  h  l  M  pc   sp     Z C P S A")
        print(
            "    " +
            f" {self.registers[REG_A]:02x}" +
            f" {self.registers[REG_B]:02x}" +
            f" {self.registers[REG_C]:02x}" +
            f" {self.registers[REG_D]:02x}" +
            f" {self.registers[REG_E]:02x}" +
            f" {self.registers[REG_H]:02x}" +
            f" {self.registers[REG_L]:02x}" +
            f" {m_val:02x}" +
            f" {self.PC:04x}" +
            f" {self.SP:04x}" +
            "  " +
            f" {self.flags[Z_FLAG] != 0:1}" +
            f" {self.flags[C_FLAG] != 0:1}" +
            f" {self.flags[P_FLAG] != 0:1}" +
            f" {self.flags[S_FLAG] != 0:1}" +
            f" {self.flags[A_FLAG] != 0:1}"
        )
