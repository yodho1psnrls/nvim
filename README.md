# nvim-config
my nvim config, enhanced for c++, shaders, and python development

Installation:
1) Install lua-language-sertver (so you can easily confige nvim)

2) Install python, which should come with pip
  - install pylsp with pip
  - install debugpy with pip (nvim-dap-python works with debugpy)

3) Install CMake from the official website
4) Install MSYS2
  - install mingw from it (you can follow the tutorial that installs mingw for olcPGE to work)
    , it includes gcc, g++ and other handy tools
    (thats why we install mingw from MSYS2 and not from the official site)
5) Install clangd(LSP) from the official release binaries on github
6) Install codelldb(DAP) from the official release binaries on github and then just unzip it

7) Optional: Install git and/or GitHubCLI

8) Install nerdfont(JetBrainsMonoExtraBold is my preffered one)

NOTE: All those things should be set on PATH as an environmental variable on windows !!!

NOTE: Use the powershell from the terminal, so it displays icons properly !

NOTE: If there are any problems use:
    :checkhealth lazy
    :checkhealth lsp
    :checkhealth dap


