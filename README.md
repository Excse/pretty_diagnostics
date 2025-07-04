# Pretty Diagnostics

Pretty Diagnostics is a library designed to produce elegant and informative error messages, with labeled references to code segments across multiple files, similar to the error output in Rust. It is particularly useful for compilers, interpreters, and any program that needs to provide meaningful error messages based on text files. The goal is to make error diagnostics as user-friendly and descriptive as possible, helping users quickly identify and resolve issues in their code.

## Features

- Generate structured, labeled error messages.
- Highlight code segments across multiple files.
- Enhance the debugging experience for developers.
- Focused on compilers, interpreters, and other text file-based error reporting systems.

## Installation

Download the project or use git to clone it:
```sh
$ git clone https://github.com/Excse/pretty_diagnostics.git
```

Navigate to the directory where the project is located and execute these commands:
```sh
$ cmake -S . -B build -DBUILD_TESTING=OFF
$ cmake --build build
$ cmake --install build
$ sudo make install
```

When you are finished, you are ready to use the library. If you want to use it in a
cmake project, use the following code to implement it.

and in CMakeLists.txt:
```cmake
find_package(pretty_diagnostics CONFIG REQUIRED)

target_link_libraries(!!PROJECT_NAME!! PRIVATE pretty_diagnostics::pretty_diagnostics)
```

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
