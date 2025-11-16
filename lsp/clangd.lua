return {
  -- The background-index tag makes clang to use cache indexing (as ccls)
  --cmd = { "clangd", "--background-index", "--query-driver=D:/Program Files/MSYS2/mingw64/bin/clang++.exe" },
  -- pacman -S mingw-w64-x86_64-clang-tools-extra (To install clang-tidy(static analysis tool, that compliments the lsp))
  -- Other tools from clang-tools-extra that you need to consider:
  --  Clang-Format, Clang-Apply-Replacements, Clang-Check, Clang-Scan-Dependencies, Clang-Syntax
  --  Extra clang-tidy checks: modernize-*, readability-*, performance-*
  --  Run this below in the bash to see full list of available checks in the modernize, readability, and performance categories
  --  clang-tidy -list-checks | grep "modernize\|readability\|performance"
  cmd = { -- NOTE: See :!clangd --help for all tags
    "clangd",
    "--background-index", -- cache
    '--query-driver=**/clang++.exe', -- glob pattern for the compiler, so it loads its own std library implementation
    -- '--query-driver=D:\\Program Files\\MSYS2\\mingw64\\bin\\clang++.exe',

    "--clang-tidy",
    -- "--clang-tidy-checks=*",  -- aggressive clang-tidy (Obsolete and Ignored !)
    -- Enable all performance-related checks for clang-tidy
    --"--checks=performance-*",     
    -- Or you can enable performance related checks one by one
    --'--checks="*,clang-analyzer-optin.performance.GCDAntipattern,clang-analyzer-optin.performance.Padding"',

    -- To support Inlay Hints
    -- "--inlay-hints",            -- enables hints globally (Obsolete and Ignored !)
    -- "--header-insertion=never", -- iwyu, never, always (Optional) Disable Unused Header Warning, But for some reason, it has no effect
    "--completion-style=detailed", -- nicer completion + hints
    "--function-arg-placeholders=false",  -- Set to true for Inlay Hints
    -- "--fallback-style=none", -- Controls clang-format style fallback. Other options: LLVM, Google, Mozilla, WebKit, ...

    -- Disable all clang-tidy checks, enabling only this specific check, which warns about deprecated c headers
    -- "--clang-tidy-checks=-*,modernize-deprecated-headers",

    --"--log=verbose", -- For Debugging
    -- "--header-insertion=never", -- (iwyu: default | never) automatically inserting #include directives for missing headers when completing symbols
    -- "--completion-style=detailed", -- (detailed: default(displays function signatures) | bundled(less detailed, only displays function, variable, etc))

    --"-Wunused-include-directive", -- Dont show unused headed warnings
    --"--disable=clang-diagnostic-unused-include-directive", -- Dont show unused headed warnings
    --"--disable-tidy-checks=llvm-header-guard",

    -- https://www.reddit.com/r/neovim/comments/m5x31e/unable_to_get_snippets_working_for_clangdusing/
    -- https://www.reddit.com/r/neovim/comments/m5x31e/comment/gr3yhyr/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
    -- "--compile-commands-dir=" .. vim.g.cmake_build_dir,
    -- "--all-scopes-completion=true", -- If set to true, code completion will include index symbols that are not defined in the scopes
  },
  root_markers = {
    'compile_commands.json',
    'compile_flags.txt',
    '.git',
  },
  filetypes = {
    'c',
    'cpp'
  },
}

