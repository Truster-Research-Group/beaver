Beaver
=====

### Description
MOOSE App for Variational Multiscale Nitsche (VMN) and Discontinuous Galerkin (VMDG) Interface Framework of linear and nonlinear solid mechanics.

### Installation and Testing
Follow the typical steps for installing the MOOSE Framework, including the module 'Tensor Mechanics'.

[Install MOOSE](https://mooseframework.inl.gov/getting_started/installation/index.html)

Then, in a terminal with environmental variable "MOOSE_DIR" defined, 
change directory to the base of this repo and run the 'make' command:

```bash
cd ~/projects/beaver
make METHOD=opt -j4
```

Verify the installation using the provided tests:

```bash
./run_tests -j4
```

### Documentation (Theory and Input)
Build the documentation for this framework using the MOOSE Documentation system. Details are found at:

[MooseDocs Setup](https://mooseframework.inl.gov/python/MooseDocs/setup.html)

Short steps are:

1. Install these Python packages with pip: pybtex livereload==2.5.1 pylatexenc anytree pandas.
2. Build and launch a live website for your application:

```bash
cd ~/projects/beaver/doc
./moosedocs.py build --serve
```

Then open a web browser and visit the URL: [http://127.0.0.1:8000](http://127.0.0.1:8000).
Alternatively, the '--serve' option can be omitted, and the website can be opened from the file:
~/.local/share/moose/site/index.html

### Documentation (Source)

Obtain hyperlinked source by building the Doxygen HTML files for this framework.
In Ubuntu-Linux, if the packages 'doxygen' and 'graphviz' are installed, 
then build the pages with this command:

```bash
cd ~/projects/beaver
doxygen doc/doxygen/Doxyfile
```

Then open the website from the file:
~/projects/beaver/doc/doxygen/html/index.html

### License
Copyright 2022 Timothy Truster and CLMI ant the University of Tennessee.
License type to be designated.

### Author Contact Information
Timothy Truster
ttruster@utk.edu

