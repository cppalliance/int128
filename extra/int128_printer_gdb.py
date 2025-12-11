# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Struct definitions:
#   struct uint128_t { std::uint64_t low; std::uint64_t high; };
#   struct int128_t  { std::uint64_t low; std::int64_t high;  };
#
# On big endian machines the word order is reversed
#
# Usage: source int128_printer.py

import gdb
import gdb.printing
import re

class Uint128Printer:
    """Pretty printer for uint128_t type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            high = int(self.val["high"]) & 0xFFFFFFFFFFFFFFFF  # Treat as unsigned
            low = int(self.val["low"]) & 0xFFFFFFFFFFFFFFFF

            value = (high << 64) | low
            return f"{value:,}"
        except Exception as e:
            return f"<invalid uint128_t: {e}>"

    def children(self):
        yield "low", self.val["low"]
        yield "high", self.val["high"]

    def display_hint(self):
        return None


class Int128Printer:
    """Pretty printer for int128_t type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            # high is std::int64_t (signed)
            high = int(self.val["high"])
            # Ensure high is treated as signed 64-bit
            if high >= 0x8000000000000000:
                high -= 0x10000000000000000

            # low is std::uint64_t (unsigned)
            low = int(self.val["low"]) & 0xFFFFFFFFFFFFFFFF

            value = (high << 64) + low
            return f"{value:,}"
        except Exception as e:
            return f"<invalid int128_t: {e}>"

    def children(self):
        yield "low", self.val["low"]
        yield "high", self.val["high"]

    def display_hint(self):
        return None


def lookup_int128_type(val):
    """
    Lookup function to detect if a type should use our pretty printers.
    Returns the appropriate printer or None.
    """
    # Get the basic type name, stripping references and const qualifiers
    type_obj = val.type

    # Handle references and pointers
    if type_obj.code == gdb.TYPE_CODE_REF:
        type_obj = type_obj.target()
    if type_obj.code == gdb.TYPE_CODE_PTR:
        return None  # Don't handle pointers directly

    # Strip const/volatile qualifiers
    type_obj = type_obj.unqualified()

    type_name = str(type_obj)

    # Patterns to match uint128_t and int128_t types
    uint128_pattern = re.compile(
        r"^(boost::int128::uint128_t|(\w+::)*uint128_t|uint128_t)$"
    )
    int128_pattern = re.compile(
        r"^(boost::int128::int128_t|(\w+::)*int128_t|int128_t)$"
    )

    if uint128_pattern.match(type_name):
        return Uint128Printer(val)
    if int128_pattern.match(type_name):
        return Int128Printer(val)

    return None


def register_int128_printers(objfile=None):
    """Register the int128 pretty printers."""
    if objfile is None:
        objfile = gdb

    objfile.pretty_printers.append(lookup_int128_type)


# Auto-register when the script is sourced
register_int128_printers()
print("int128_t and uint128_t pretty printers loaded successfully")
