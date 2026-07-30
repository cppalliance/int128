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

import lldb

def uint128_summary(valobj, internal_dict):
    """
    Custom summary for uint128 type (unsigned).
    Displays as decimal (base 10).
    """
    try:
        val = valobj.GetNonSyntheticValue()
        high = val.GetChildMemberWithName("high").GetValueAsUnsigned()
        low  = val.GetChildMemberWithName("low").GetValueAsUnsigned()

        value = (high << 64) | low
        return f"{value:,}"
    except Exception as e:
        return f"<invalid uint128: {e}>"

def int128_summary(valobj, internal_dict):
    """
    Custom summary for int128 type (signed).
    Displays as decimal (base 10).
    """
    try:
        val = valobj.GetNonSyntheticValue()
        # Both words are std::uint64_t, so read them unsigned and fold the
        # two's complement sign in by hand.
        high = val.GetChildMemberWithName("high").GetValueAsUnsigned()
        low  = val.GetChildMemberWithName("low").GetValueAsUnsigned()

        value = (high << 64) | low
        if value >= (1 << 127):
            value -= (1 << 128)

        return f"{value:,}"
    except Exception as e:
        return f"<invalid int128: {e}>"

def __lldb_init_module(debugger, internal_dict):
    uint128_pattern = r"^(const )?(boost::int128::uint128|(\w+::)*uint128)( &| \*)?$"
    int128_pattern = r"^(const )?(boost::int128::int128|(\w+::)*int128)( &| \*)?$"

    debugger.HandleCommand(
        f'type summary add -x "{uint128_pattern}" -e -F int128_printer_lldb.uint128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{uint128_pattern}" -l int128_printer_lldb.Uint128SyntheticProvider'
    )

    debugger.HandleCommand(
        f'type summary add -x "{int128_pattern}" -e -F int128_printer_lldb.int128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{int128_pattern}" -l int128_printer_lldb.Int128SyntheticProvider'
    )

    print("int128 and uint128 pretty printers loaded successfully")

class Uint128SyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 2

    def get_child_index(self, name):
        if name == "low":
            return 0
        elif name == "high":
            return 1
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("low")
        elif index == 1:
            return self.valobj.GetChildMemberWithName("high")
        return None

    def update(self):
        pass

    def has_children(self):
        return True

class Int128SyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 2

    def get_child_index(self, name):
        if name == "low":
            return 0
        elif name == "high":
            return 1
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("low")
        elif index == 1:
            return self.valobj.GetChildMemberWithName("high")
        return None

    def update(self):
        pass

    def has_children(self):
        return True
