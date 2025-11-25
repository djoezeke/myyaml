<p align="center"><h1 align="center">🚀 MYYAML : Modern C/C++ YAML Library</h1></p>

<p align="center">
    <em><code>Lightweight, modern, and easy-to-use C++ YAML serializer & deserializer.</code></em>
</p>
<p align="center">
    <img src="https://img.shields.io/github/license/djoezeke/myyaml?style=flat-square&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
    <img src="https://img.shields.io/github/last-commit/djoezeke/myyaml?style=flat-square&logo=git&logoColor=white&color=0080ff" alt="last-commit">
    <img src="https://img.shields.io/github/languages/top/djoezeke/myyaml?style=flat-square&color=0080ff" alt="repo-top-language">
    <img src="https://img.shields.io/github/languages/count/djoezeke/myyaml?style=flat-square&color=0080ff" alt="repo-language-count">
</p>

<!-- ## Table of contents -->

<details><summary>Table of Contents</summary>

- [📍 Overview](#-overview)
- [👾 Design Goals](#-design-goals)
- [👾 Features](#-features)
- [📁 Project Structure](#-project-structure)
- [🚀 Getting Started](#-getting-started)
  - [☑️ Prerequisites](#-prerequisites)
  - [🛠️ Integration](#-integration)
    <!-- - [git](#git-submodule) -->
    <!-- - [CMake](#cmake-fetchcontent) -->
  - [🤖 Usage](#-usage)
  <!-- - [🤖 Example](#-example) -->
  - [🧪 Testing](#-testing)
  <!-- - [🧪 Benchmarking](#-benchmarking) -->
- [📌 Project Roadmap](#-project-roadmap)
- [ Support](#-support)
- [🔰 Contributing](#-contributing)
  <!-- - [Contributors](#contributors) -->
- [🙌 Acknowledgments](#-acknowledgments)
  <!-- - [Used third party tools](#used-third-party-tools) -->
- [📄 License](#-license)

</details>

## 📍 Overview

<!-- ## About Myyaml -->

**Myyaml** is a modern, lightweight YAML parsing and serialization library for C++. It provides an easy-to-use interface for working with YAML data, including parsing YAML strings, creating YAML objects, and serializing YAML objects to strings.It provides a simple, type-safe API for working with YAML data in C++ projects.

<!-- ## About yaml -->

<img align="right" src=".github/readme/YAML_logo.svg" width="200" alt="YAML Logo">

### [Yaml](https://yaml.org) [YAML Ain't Markup Language]

#### A Human-friendly data serialization.

YAML is one of the most popular data serialization languages, and it is used mostly for writing configuration files. The YAML recursive acronym stands for YAML Ain’t Markup Language. This language is designed with flexibility and accessibility in mind, so it’s human-readable and simple to understand. YAML works with all modern programming languages and is widely used in data persistence, internet messaging, cross-language data sharing, and many other places. YAML files either have the extension .yaml or .yml.

More information about YAML can be obtained at [here](https://yaml.org)

<!-- #### Yaml Example.

```yaml
---
# A sample yaml file
name: "Myyaml : Yaml for Modern C/C++"
author: djoezeke
published: true
branches:
  - main
  - dev
tutorial:
  - yaml:
      name: "YAML Ain't Markup Language"
      type: awesome
      born: 2001
  - json:
      name: JavaScript Object Notation
      type: great
      born: 2001
  - toml:
      name: Tom's Obvious Minimal Language
      type: great
      born: 2013
  - xml:
      name: Extensible Markup Language
      type: good
      born: 1996
``` -->

<!-- ## Design Goals

Myyaml has been developed with these design goals:

- 💼 **Portable** The library depends only on C/C++ standards, and is carefully designed to work on many platforms so that Myyaml can be imported into existing C or C++ projects written in either C11, C++11 or later.
  Furthermore, the project supports [CMake](https://cmake.org/) and provides [the documentation exclusively for CMake integration](https://djoezeke.github.io/myyaml/tutorials/cmake_integration).
  Those characteristics allow existing C or C++ project using CMake for builds to quickly incorporate YAML support with just a little effort.

- 🪰 **Heavily Tested** Myyaml has been [unit-tested](https://github.com/djoezeke/myyaml/tree/main/tests/) and its test suite covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](#).)
  We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.
  Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://releases.llvm.org/14.0.0/tools/clang/tools/extra/docs/clang-tidy/index.html), [CodeQL](https://codeql.github.com/docs/) and [Codacy](https://www.codacy.com/).
  [GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers, operating systems and C/C++ standards.
  See the [prerequisites](#-prerequisites) section for more details.

- 📝 **Well Documented** [The documentation](https://djoezeke.github.io/myyaml/) provides plenty of information so that users can understand what Myyaml is and what they can achieve with Myyaml.
  For instance, [the tutorial pages](https://djoezeke.github.io/myyaml/tutorials/) show how you can read/write/customize using Myyaml to handle YAML documents while creating a simple C/C++ project using CMake.
  Also, [API reference pages](https://djoezeke.github.io/myyaml/api/) provide more detailed, exclusive descriptions with example usages for each Myyaml API.

## ✨ Features

- ⚡ **Fast**: Efficient parsing and serialization
- 🧩 **Type-safe**: Strong C++ type system for all YAML types
- 🛠️ **Easy API**: Intuitive interface for manipulating YAML objects and arrays
- 📝 **Modern C++**: Uses smart pointers, `std::variant`, and other C++17 features
- 🧪 **Tested**: Includes unit tests and examples

More detailed descriptions are to be found on [GitHub Pages](https://djoezeke.github.io/myyaml).

Here is the list you might want to know:

- [Tutorial](https://djoezeke.github.io/myyaml/tutorials) - getting started
- [API references](https://djoezeke.github.io/myyaml/api/) - all the details with examples

### 📓 Note
 -->

<details closed>

<summary> 📁 Project Structure</summary>

```sh
myyaml/
├── CMakeLists.txt
├── LICENSE
├── myyaml.hpp
├── myyaml.cpp
├── tests/
│   ├── test.c
│   └── test.cpp
└── examples/
    ├── basic.c
    ├── basic.cpp
    └── sample.cpp
```

</details>

## 🚀 Getting Started

### ☑️ Prerequisites

Please ensure your runtime environment meets the following requirements:

- **C++11** or newer compiler
- **CMake** (for building examples/tests)

Myyaml uses following software, tools and services:

- [CMake](https://cmake.org/) as a general build tool
- (optional) [Catch2](https://github.com/catchorg/Catch2.git) for unit and performance testing
- (optional) [Mkdocs](http://) to build web documentation
- (optional) [Doxygen](http://www.doxygen.org) to build code documentation

### 🛠️ Integration

1. **Clone the repository:**

   ```sh
   git clone https://github.com/djoezeke/myyaml.git
   ```

2. **Add to your project:**

   - Copy `myyaml.hpp` and `myyaml.cpp` into your project.
   - Or add as a submodule and include in your build.

3. **Include in your code:**

   ```cpp
   #include "myyaml.hpp"
   ```

4. **Compile:**
   ```sh
   g++ my_source.cpp myyaml.cpp -std=c++17 -o main
   ```

<!-- #### Using [CMake](https://cmake.org/)

<details closed>
<summary>Configuring and Building</summary>

</details>

#### Using [Meson]()

<details closed>
<summary>Configuring and Building</summary>

</details>

#### Using [Makefile](https://www.gnu.org/software/make/)

<details closed>
<summary>Testing and Building</summary>

> [!NOTE]
> Makefile support is limited

</details> -->

## Support

🤔 If you have a **question**, please check if it is already answered in the [**FAQ**](https://djoezeke.github.io/myyaml/faq/) or the [**Q&A**](https://github.com/djoezeke/myyaml/discussions/categories/q-a) section. If not, please [**ask a new question**](https://github.com/djoezeke/myyaml/discussions/new) there.

📓 If you want to **learn more** about how to use the library, check out the rest of the [**README**](#examples), have a look at [**code examples**](https://github.com/djoezeke/myyaml/tree/main/examples), or browse through the [**help pages**](https://djoezeke.github.io/myyaml/faq/).

🚧 If you want to understand the **API** better, check out the [**API Reference**](https://djoezeke.github.io/myyaml/api/).
🐞 If you found a **bug**, please check the [**FAQ**](https://djoezeke.github.io/myyaml/faq/) if it is a known issue or the result of a design decision. Please also have a look at the [**issue list**](https://github.com/djoezeke/myyaml/issues) before you [**create a new issue**](https://github.com/djoezeke/myyaml/issues/new/choose). Please provide as much information as possible to help us understand and reproduce your issue.

## 🤝 Contributing

- **💬 [Join the Discussions](https://github.com/djoezeke/myyaml/discussions)**: Share your insights, provide feedback, or ask questions.
- **🐛 [Report Issues](https://github.com/djoezeke/myyaml/issues)**: Submit bugs found or log feature requests for the `myyaml` project.
- **💡 [Submit Pull Requests](https://github.com/djoezeke/myyaml/blob/main/.github/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your github account.
2. **Clone Locally**: Clone the forked repository to your local machine using a git client.
   ```sh
   git clone https://github.com/djoezeke/myyaml
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to github**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the main branch. Congratulations on your contribution!

See the [CONTRIBUTING.md](https://github.com/djoezeke/myyaml/blob/main/.github/CONTRIBUTING.md) file for detailed information.

</details>

<details closed>
<summary>Contributor Graph</summary>
<br>
<p align="left">
   <a href="https://github.com{/djoezeke/myyaml/}graphs/contributors">
      <img src="https://contrib.rocks/image?repo=djoezeke/myyaml">
   </a>
</p>
</details>

## 🙌 Acknowledgments

We would like to express our gratitude to the following projects and individuals whose work made this project possible:

- [**Catch2**](https://github.com/catchorg/Catch2) as a unit-test framework.
- [**Clang**](https://clang.llvm.org) for coding style checks, and static/runtime analysis.
- [**CMake**](https://cmake.org) for build automation.
- [**Google Benchmark**](https://github.com/google/benchmark) to implement the benchmarks.
- [**Material for MkDocs**](https://squidfunk.github.io/mkdocs-material/) for the style of the documentation site
- [**MkDocs**](https://www.mkdocs.org) for the documentation site.
- [**Valgrind**](https://valgrind.org) for memory leak check & correct memory management.
- [**GitHub Changelog Generator**](https://github.com/skywinder/github-changelog-generator) to generate the [ChangeLog]().

The open-source community for their invaluable libraries, tutorials, and support.

### With thanks to:

<!-- - **[](https://github.com/)** - Fixed a bug -->

> [!NOTE]
> If you feel your work should be acknowledged here, please open an issue or pull request.

### References

- [myyaml](https://github.com/djoezeke/myxml/) : A C/C++ YAML Library.
- [myjson](https://github.com/djoezeke/myjson/) : A C/C++ JSON Library.
- [mytoml](https://github.com/djoezeke/mytoml/) : A C/C++ TOML Library.

### More Reading

## 📄 License

This project is protected under the [MIT](LICENSE) License.

<!-- <picture>
  <source
    media="(prefers-color-scheme: dark)"
    srcset="
      https://api.star-history.com/svg?repos=myyaml/djoezeke&type=Date&theme=dark
    "
  />
  <source
    media="(prefers-color-scheme: light)"
    srcset="
      https://api.star-history.com/svg?repos=myyaml/djoezeke&type=Date
    "
  />
  <img
    alt="Star History Chart"
    src="https://api.star-history.com/svg?repos=myyaml/djoezeke&type=Date"
  />
</picture> -->
