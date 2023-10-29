import os
import sys


def get_opts(env: "Environment"):
    return []


def configure(env: "Environment"):
    env.Append(CPPPATH=["#/modules/ftxui/upstream/include"])