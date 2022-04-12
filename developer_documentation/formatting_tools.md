## Formatting Tools

### Clang

Many of these BioCro formatting preferences can be applied automatically using the program [clang-format](https://clang.llvm.org/docs/ClangFormat.html) with the [.clang-format](../.clang-format) file provided in the base directory of BioCro. **Do not apply clang-format to all files indiscriminately**, as that will ruin manually-aligned tables.

#### Installation

One can install clang-format on Ubunutu using `sudo apt install clang-format` and on MacOS through Homebrew.

#### Using Clang

Files can be formatted using

```
clang-format file_name > new_file
```
or edited in place using
```
clang-format -i file_name
```

If your editor has the ability to display differences between the original and revised versions of the file, it is a good idea to step through and inspect the proposed changes to ensure they are desirable.

#### Clang with the CodeLite IDE

On Windows, MacOS, or Linux, the CodeLite IDE includes clang-format
and provides an easy way to use it. First go to Plugins -> Source Code
Formatter -> Options. In the C++ tab, select `use .clang-format
file`. Now press `Ctrl-I` or click Plugins -> Source Code Formatter ->
Format Current Source to format a file.

### EditorConfig

Another tool to help with formatting is EditorConfig.  EditorConfig,
when used in conjunction with the [.editorconfig](../.editorconfig)
file provided in the base directory of BioCro, provides a method for
standardizing settings across different text editors. While some
editors have native support, others require a plugin. See the
[EditorConfig website](https://editorconfig.org/) for more details.
