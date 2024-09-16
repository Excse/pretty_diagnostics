# Pretty Diagnostics

Pretty Diagnostics is a library designed to produce elegant and informative error messages, with labeled references to code segments across multiple files, similar to the error output in Rust. It is particularly useful for compilers, interpreters, and any program that needs to provide meaningful error messages based on text files. The goal is to make error diagnostics as user-friendly and descriptive as possible, helping users quickly identify and resolve issues in their code.

## Features

- Generate structured, labeled error messages.
- Highlight code segments across multiple files.
- Enhance the debugging experience for developers.
- Focused on compilers, interpreters, and other text file-based error reporting systems.

## Installation

### Using CMake

You can install **Pretty Diagnostics** either locally for development or globally for system-wide use.

#### Local Installation

To install locally:

```bash
git clone https://github.com/Excse/pretty_diagnostics.git
cd pretty-diagnostics
mkdir build
cd build
cmake ..
make
```

This will build the project and allow you to use it locally within your development environment.

#### Global Installation

To install globally:

```bash
git clone https://github.com/Excse/pretty_diagnostics.git
cd pretty-diagnostics
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

This will install the library system-wide, allowing it to be accessible from anywhere on your machine.

## Example Usage

```cpp
auto details = pretty_diagnostics::File{"This is a test text\n"
                                            "Maybe you could have guessed that.",
                                            "some/weird/path"};

auto report = pretty_diagnostics::ReportBuilder ()
    .type (pretty_diagnostics::Type::ERROR)
    .message ("Displaying a brief summary of what happend.")
    .code (1337)
    .label (pretty_diagnostics::LabelBuilder ()
                    .message ("Giving some {RED}tips{/} or {ORANGE}extra details{/} about "
                                    "what is wrong here.")
                    .span ({&details, 10, 13})
                    .color (pretty_diagnostics::ColorType::RED)
                    .build ())
    .note ("Can be used to give a hint about what you could do better.")
    .build ();

report.print (std::cout);
```

The output looks like:</br>
![Image of this example](resources/example.png)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
