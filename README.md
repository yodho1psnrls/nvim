# nvim-config
my nvim config, enhanced for c++, shaders, and python development

Links that may come in handy:
https://www.reddit.com/r/neovim/comments/1crdv93/neovim_on_windows_using_windows_terminal_and/

Installation:
0) Install Neovim

1) Install lua-language-server (so you can easily confige nvim)

2) Install python, which should come with pip
  - install pylsp with pip (pip install python-lsp-server)
  - install required linter flake8 (pip install flake8)
  - to configure flake8:

# Ignore formatting-related rules
# To test it:
#   On PowerShell: flake8 --config $env:USERPROFILE\AppData\Roaming\flake8 --version
#   On CMD:        flake8 --config %USERPROFILE%\AppData\Roaming\flake8 --version
[flake8]
ignore = E121, E123, E126, E226, E24, E704, E501, W291, W293, W391

  - install debugpy with pip (nvim-dap-python works with debugpy)

3) Install CMake from the official website
4) Install MSYS2 (you can follow this video for the proper
   MSYS2 installation, but after that you dont have to install
   mingw-gcc and mingw-make, we will use mingw-clang and
   mingw-ninja instead - https://www.youtube.com/watch?v=jnI1gMxtrB4)

    - Open MSYS2
    - (add the MSYS2, MSYS2/usr/bin and MSYS2\mingw64\bin folders to path)
    - pacman -Syu (initial update)
    - (close and reopen MSYS2)
    - pacman -Sy  (secondary update)
    - pacman -Su  (secondary update)
    - pacman -S mingw-w64-x86_64-clang
    - pacman -S mingw-w64-x86_64-ninja

5) Install clangd(LSP) from the official release binaries on github
6) Install codelldb(DAP) from the official release binaries on github 
   (https://github.com/vadimcn/codelldb/releases) and then just unzip it

7) Optional: Install git and/or GitHubCLI (optional)

8) Install nerdfont(JetBrainsMonoExtraBold is my preffered one)

9) Optional: Install ripgrep through pacman

10) Install the config
    cd ~/AppData/Local
    git clone https://github.com/yodho1psnrls/nvim

11) Install Windows Terminal

NOTE: All those things should be set on the PATH environmental variable on windows !!!
       and some additional environmental variables such as:
        - name: CMAKE_C_COMPILER, value: clang
        - name: CMAKE_CXX_COMPILER, value: clang++
        - name: CMAKE_GENERATOR, value: Ninja or MinGW Makefiles (depends on which build system you choose)
        - name: CC, value: path/to/clang.exe
        - name: CXX, value: path/to/clang++.exe

NOTE: Use the powershell from the terminal, so it displays icons properly !

NOTE: Set up a default directory, when you open powershell
       Run "echo $profile" in powershell
       Then open this ps1 file and type: Set-Location -Path "D:\"

NOTE: If there are any problems use:
    :checkhealth lazy
    :checkhealth lsp
    :checkhealth dap

https://stackoverflow.com/questions/62578458/vim-nvim-not-filling-windows-terminal
NOTE: If you use windows terminal and there are back bars 
       at the right and the bottom, then you should either:
1) Go to the terminal setting (both the default and the powershell)
    set padding to 0 and scrollbar visibility to hidden
2) If 1) doesnt work, then go to 
    C:\Users\Todor\AppData\Local\Packages
    \Microsoft.WindowsTerminal\LocalState\settings.json
    and set
    "padding": "0, 0, -10, -10",
    "scrollbarState": "hidden",

TIP: On Windows 10/11 the Windows Defender can significantly
      slow down nvim's startup time
      see - https://www.reddit.com/r/neovim/comments/1gtbiko/windows_defender_making_neovim_slow_on_windows_11/
     But you dont have to stop the entire windows defender.
     So to Fix it:
      Win Button -> type defender -> virus and threat protection ->
      -> virus and threat protection setting (Manage Settings) ->
      -> Exclusions (Add or remove exclusions) ->
      -> Add an exclusin process -> type "nvim" -> Done
     Also you can exclude all other processes that nvim triggers like:
      git, grep, gcc, g++, clang, clang++, ninja, cmake, clangd, pylsp, codelldb, debugpy


NOTE: You can use ccache for caching during building of a project
       for faster build times, but it is not so benefitial for largely
       templated header files
       1) Install it: pacman -S mingw-w64-x86_64-ccache
       2) Check cache size and location: ccache -s
       3) Change caches size: ccache --max-size=20G (Default is 5G)
       4) After running a build, check ccache statistics: ccache -s
       5) You can also configure ccache with a "~/.ccache/ccache.conf" file
            This is how ccache.conf should look like:
            max_size = 20.0G         # Set cache size to 20GB
            compression = true       # Enable compression for cache files
            compression_level = 5    # Set compression level
       6) After a while you may need to clean the cache, so
           to fully clean: ccache -C
           to clean only old or least-used cache entries: ccache -c
