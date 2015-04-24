#!/usr/bin/python

from setuptools import setup, find_packages, Extension
import commands

cflags = commands.getoutput('pkg-config --cflags lognorm').strip().split()
ldflags = commands.getoutput('pkg-config --libs lognorm').strip().split()

setup(
    name = "liblognorm",
    version = "0.1.0",
    description = "fast log normalization library",
    ext_modules = [
        Extension(
            'liblognorm',
            sources = ['c_lib/lognorm.c'],
            extra_compile_args = cflags,
            extra_link_args = ldflags,
        ),
    ],
)
