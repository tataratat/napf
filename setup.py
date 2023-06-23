import platform

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

with open("README.md") as f:
    readme = f.read()

with open("napf/_version.py") as f:
    version = eval(f.read().strip().split("=")[-1])

flags = {}
if platform.system().startswith("Windows"):
    flags["extra_compile_args"] = ["/O2"]
else:
    flags["extra_compile_args"] = ["-O3"]
    flags["libraries"] = ["stdc++"]

ext_modules = [
    Pybind11Extension(
        "napf._napf",
        [
            "src/python/classes/radius_search_result_vectors.cpp",
            "src/python/classes/int_trees.cpp",
            "src/python/classes/long_trees.cpp",
            "src/python/classes/float_trees.cpp",
            "src/python/classes/double_trees.cpp",
            "src/python/napf.cpp",
        ],
        include_dirs=["third_party"],
        cxx_std=11,
        **flags
    )
]

setup(
    name="napf",
    version=version,
    description="nanoflann python bindings for kdtree.",
    long_description=readme,
    long_description_content_type="text/markdown",
    author="Jaewook Lee",
    author_email="jaewooklee042@gmail.com",
    url="https://github.com/tataratat/napf",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    packages=["napf"],
    package_data={  # didn't do much. so added MANIFEST.in
        "src": ["*.hpp", "python/*.hpp"],
        "third_party": ["*.hpp"],
    },
    classifiers=[
        "Development Status :: 2 - Pre-Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Natural Language :: English",
        "Topic :: Scientific/Engineering",
    ],
    install_requires=[
        "numpy",
    ],
    zip_safe=False,
    license="MIT",
)
