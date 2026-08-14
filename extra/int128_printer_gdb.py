# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Struct definitions:
#   struct uint128 { std::uint64_t low; std::uint64_t high; };
#   struct int128  { std::uint64_t low; std::uint64_t high; };
#
# Both words of both types are unsigned; int128 reads the pair as two's
# complement. On big endian machines the word order is reversed.
#
# Usage: source int128_printer.py

import gdb
import gdb.printing
import re

class Uint128Printer:
    """Pretty printer for uint128 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            high = int(self.val["high"]) & 0xFFFFFFFFFFFFFFFF  # Treat as unsigned
            low = int(self.val["low"]) & 0xFFFFFFFFFFFFFFFF

            value = (high << 64) | low
            return f"{value:,}"
        except Exception as e:
            return f"<invalid uint128: {e}>"

    def children(self):
        yield "low", self.val["low"]
        yield "high", self.val["high"]

    def display_hint(self):
        return None


class Int128Printer:
    """Pretty printer for int128 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            # high is std::uint64_t; fold the two's complement sign in by hand
            high = int(self.val["high"]) & 0xFFFFFFFFFFFFFFFF
            if high >= 0x8000000000000000:
                high -= 0x10000000000000000

            # low is std::uint64_t (unsigned)
            low = int(self.val["low"]) & 0xFFFFFFFFFFFFFFFF

            value = (high << 64) + low
            return f"{value:,}"
        except Exception as e:
            return f"<invalid int128: {e}>"

    def children(self):
        yield "low", self.val["low"]
        yield "high", self.val["high"]

    def display_hint(self):
        return None


def lookup_int128ype(val):
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

    # Patterns to match uint128 and int128 types
    uint128_pattern = re.compile(
        r"^(boost::int128::uint128|(\w+::)*uint128|uint128)$"
    )
    int128_pattern = re.compile(
        r"^(boost::int128::int128|(\w+::)*int128|int128)$"
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

    objfile.pretty_printers.append(lookup_int128ype)


# Auto-register when the script is sourced
register_int128_printers()
print("int128 and uint128 pretty printers loaded successfully")
