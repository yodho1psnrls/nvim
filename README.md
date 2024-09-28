# nvim-config
my nvim config, enhanced for c++, shaders, and python development

Installation:
1) Install lua-language-server (so you can easily confige nvim)

2) Install python, which should come with pip
  - install pylsp with pip
  - install debugpy with pip (nvim-dap-python works with debugpy)

3) Install CMake from the official website
4) Install MSYS2 (you can follow this video for the proper
   MSYS2 installation, but after that you dont have to install
   mingw-gcc and mingw-make, we will use mingw-clang and
   mingw-ninja instead - https://www.youtube.com/watch?v=jnI1gMxtrB4)

    - (add the MSYS2 and MSYS2/usr/bin folders to path)
    - pacman -Syu (initial update)
    - (close and reopen the terminal)
    - pacman -Su  (secondary update)
    - pacman -S mingw-w64-x86_64-clang
    - pacman -S mingw-w64-x86_64-ninja

5) Install clangd(LSP) from the official release binaries on github
6) Install codelldb(DAP) from the official release binaries on github 
   (https://github.com/vadimcn/codelldb/releases) and then just unzip it

7) Optional: Install git and/or GitHubCLI (optional)

8) Install nerdfont(JetBrainsMonoExtraBold is my preffered one)

9) Optional: Install ripgrep through pacman

NOTE: All those things should be set on the PATH environmental variable on windows !!!
       and some additional environmental variables such as:
        - name: CMAKE_C_COMPILER, value: clang
        - name: CMAKE_CXX_COMPILER, value: clang++
        - name: CMAKE_GENERATOR, value: Ninja or MinGW Makefiles (depends on which build system you choose)
        - name: CC, value: path/to/clang.exe
        - name: CXX, value: path/to/clang++.exe

NOTE: Use the powershell from the terminal, so it displays icons properly !

NOTE: If there are any problems use:
    :checkhealth lazy
    :checkhealth lsp
    :checkhealth dap


