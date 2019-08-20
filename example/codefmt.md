# C++ 代码规范工作流

为了规范工作流程，提高工作效率，这里根据 Google 提出的 C++ 编码风格，配合若干检查、自动修改工具，制定了一套代码规范工作流。

## Google C++ Style Guide

Google 代码规范的详情，请参考

<https://google.github.io/styleguide/cppguide.html>

## 自动化检查纠正工具

下面介绍几个会用到的自动化工具。

#### ClangFormat

<https://clang.llvm.org/docs/ClangFormat.html>

可以自动格式化代码，使其符合 Google 代码规范，不包括：

- 语法的修改
- 命名修改

#### Cpplint

<https://github.com/cpplint/cpplint>

可以检查代码是否符合 Google 代码规范，极其严格，但不包括：

- 语法特性的规范

#### Clang-Tidy

<https://clang.llvm.org/extra/clang-tidy/#using-clang-tidy>

可以从语法层面来对代码进行检查，可以做到，检查是否使用了某个禁用的语法特性。

## 基于 Git 的工作流程

1. **编辑阶段**：使用编辑器编辑代码文件，保存前可以依次进行下面的操作和检查：
   1. 使用 **CLangFormat** 自动修改当前代码文件成 Google 建议的风格（**命名风格** 并不会被纠正）。
   2. 使用 **Cpplint** 检查当前保存的文件是否符合 Google 建议的风格
   3. 使用 **Clang-Tidy** 检查当前保存的文件是否符合要求的编码规范
2. **编译阶段**：代码保存成功后，执行 `make` 编译代码，这里可以配置 CMakeLists.txt，确保每次编译文件时，都会进行一下检查：
   1. 使用 **Cpplint** 检查当前编译的文件是否符合 Google 建议的风格
   2. 使用 **Clang-Tidy** 检查当前编译的文件是否符合要求的编码规范
3. **提交阶段**：代码保存成功后，执行  `git commint` 提交代码，这里可以配置 **git pre-commit hook** 来触发下面的检查操作，通过则提交成功，否则驳回
   1. 使用 **Cpplint** 检查本次提交的文件是否符合 Google 建议的风格
   2. 使用 **Clang-Tidy** 检查本次提交的文件是否符合要求的编码规范
4. **上传阶段（服务器端）**：`git commit` 执行成功后，执行 `git push` 推送代码至远程服务器，这边可以配置 **git pre-receive hook** 来出发下面的检查操作，通过则 push 成功，否则驳回
   1. 使用 **Cpplint** 检查本次 push 的文件是否符合 Google 建议的风格
   2. 使用 **Clang-Tidy** 检查本次 push 的文件是否符合要求的编码规范

## 基于 SVN 的工作流

1. **编辑阶段**：使用编辑器编辑代码文件，保存前可以依次进行下面的操作和检查：
   1. 使用 **CLangFormat** 自动修改当前代码文件成 Google 建议的风格（**命名风格** 并不会被纠正）。
   2. 使用 **Cpplint** 检查当前保存的文件是否符合 Google 建议的风格
   3. 使用 **Clang-Tidy** 检查当前保存的文件是否符合要求的编码规范
2. **编译阶段**：代码保存成功后，执行 `make` 编译代码，这里可以配置 CMakeLists.txt，确保每次编译文件时，都会进行一下检查：
   1. 使用 **Cpplint** 检查当前编译的文件是否符合 Google 建议的风格
   2. 使用 **Clang-Tidy** 检查当前编译的文件是否符合要求的编码规范
3. **提交阶段**：代码保存成功后，执行  `svn commit` 提交代码，这里可以配置 **svn pre-commit hook** 来触发下面的检查操作，通过则提交成功，否则驳回
   1. 使用 **Cpplint** 检查本次提交的文件是否符合 Google 建议的风格
   2. 使用 **Clang-Tidy** 检查本次提交的文件是否符合要求的编码规范

## 开发环境配置

Todo.