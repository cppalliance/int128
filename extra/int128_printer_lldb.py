# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Struct definitions:
#   struct uint128_t { std::uint64_t low; std::uint64_t high; };
#   struct int128_t  { std::uint64_t low; std::int64_t high;  };
#
# On big endian machines the word order is reversed

import lldb

def uint128_summary(valobj, internal_dict):
    """
    Custom summary for uint128_t type (unsigned).
    Displays as decimal (base 10).
    """
    try:
        val = valobj.GetNonSyntheticValue()
        high = val.GetChildMemberWithName("high").GetValueAsUnsigned()
        low  = val.GetChildMemberWithName("low").GetValueAsUnsigned()

        value = (high << 64) | low
        return f"{value:,}"
    except Exception as e:
        return f"<invalid uint128_t: {e}>"

def int128_summary(valobj, internal_dict):
    """
    Custom summary for int128_t type (signed).
    Displays as decimal (base 10).
    """
    try:
        val = valobj.GetNonSyntheticValue()
        # high is std::int64_t, so use GetValueAsSigned()
        high = val.GetChildMemberWithName("high").GetValueAsSigned()
        # low is std::uint64_t, so use GetValueAsUnsigned()
        low  = val.GetChildMemberWithName("low").GetValueAsUnsigned()

        value = (high << 64) + low
        return f"{value:,}"
    except Exception as e:
        return f"<invalid int128_t: {e}>"

def __lldb_init_module(debugger, internal_dict):
    uint128_pattern = r"^(const )?(boost::int128::uint128_t|(\w+::)*uint128_t)( &| \*)?$"
    int128_pattern = r"^(const )?(boost::int128::int128_t|(\w+::)*int128_t)( &| \*)?$"

    debugger.HandleCommand(
        f'type summary add -x "{uint128_pattern}" -e -F int128_printer.uint128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{uint128_pattern}" -l int128_printer.Uint128SyntheticProvider'
    )

    debugger.HandleCommand(
        f'type summary add -x "{int128_pattern}" -e -F int128_printer.int128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{int128_pattern}" -l int128_printer.Int128SyntheticProvider'
    )

    print("int128_t and uint128_t pretty printers loaded successfully")

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
