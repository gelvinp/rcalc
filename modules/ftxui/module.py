import os
import sys


def configure(env: "Environment"):
    env.Append(CPPPATH=["#/modules/ftxui/upstream/include"])