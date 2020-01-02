# Generating Documentation
To generate documentation for the C/C++ code, we use Doxygen.

## Installing and Running Doxygen and Related Tools

### Required software
- Doxygen
- GraphViz
- GhostSctipt version 9.27. A bug in Doxygen prevents it from working with GhostScript 9.50.

#### Ubuntu Linux
1. Download doxygen and graphviz applications using a the software manager:
    ```
    sudo apt-get install doxygen graphviz dot ghostscript
    ```

2. Compile documentation
    ```
    cd /path/to/biocro/documentation
    doxygen Doxyfile
    ```

####  Windows: 
1. Download and install the Doxygen binary distribution from <http://www.stack.nl/~dimitri/doxygen/download.html#srcbin>. After installation.
2. You may need to update the path Windows to include `C:\Program Files (x86)\Graphiviz.2.38\bin` and `C:\Program Files (x86)\gs\gs9.27\bin`.
3. You can use the doxywizard program and select the Biocro directory as the working directory from which doxygen will run. Press ctrl+O, find the Biocro directory and select the "Doxyfile" file. Then select run and run doxygen.

Alternatively, one can install a terminal such as MobaXterm, and complete the instructions for Linux using a local termina.

### Viewing the Documentation
After Doxygen has been run, a new directory named "html" will be created under documentation directory.  Point your browser to /path/to/biocro/documentation/html/index.html

#### Options for ctags.
ctags is a program that generates tags for C and C++ files so that identifiers can be located easily.
This is useful for jumping between locations in the code with editors such as Vim.
If you use ctags, the flag "--c++-kinds=+p" will cause ctags to create tages for symbols in header files, which is often useful.
    ```
    ctags --c++-kinds=+p
    ```
