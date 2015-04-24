#!/usr/bin/python

from setuptools import setup, find_packages, Extension

setup(
    name = "liblognorm",
    version = "0.0.0",
    description = "fast log normalization library",
    ext_modules = [
        Extension('liblognorm', sources = ['c_lib/lognorm.c']),
    ],
)
