----------------------------- KICKSTARTER -----------------------------------------

-- If you never want Neovim's Python settings
-- This disables all built-in filetype settings, not just Python
-- vim.g.did_load_filetypes = 1
-- vim.g.did_load_ftplugin = 1

-- BUG:
-- This places a second border on top of windows that already have a border
--  like Telescope and Lazy windows
-- vim.o.winborder = 'rounded' -- doesnt affect much

-- Folding options
-- zc folds by method, h or l on the folded line, unfolds it
-- https://www.reddit.com/r/neovim/comments/1jmqd7t/sorry_ufo_these_7_lines_replaced_you/?rdt=46694

vim.o.foldcolumn = '0' -- '0' is not bad, '1' is default | "auto" | "auto:number"
vim.o.foldlevel = 99 -- Using ufo provider need a large value, feel free to decrease the value
vim.o.foldlevelstart = 99 -- Starts with all folds open
-- vim.o.foldlevelstart = 0 -- Starts with all folds closed
vim.o.foldenable = true -- Automatic folding when entering a buffer

vim.o.foldmethod = 'expr' -- indent(treesitter) | expr(LSP) | syntax | manual(fold yourself only)
-- vim.o.foldexpr = 'v:lua.vim.lsp._get_fold_level()'
-- vim.o.foldexpr = 'v:lua.vim.lsp.foldexpr()'
vim.o.foldexpr = 'v:lua.vim.treesitter.foldexpr()'

-- Prefer LSP foldexpr, if supported
-- vim.api.nvim_create_autocmd('LspAttach', {
--	callback = function(args)
--		local client = vim.lsp.get_client_by_id(args.data.client_id)
--		if client and client.supports_method('textDocument/foldingRange') then
--			local win = vim.api.nvim_get_current_win()
--			vim.wo[win][0].foldmethod = 'expr'
--			vim.wo[win][0].foldexpr = 'v:lua.vim.lsp.foldexpr()'
--		end
--	end,
-- })
-- vim.api.nvim_create_autocmd('LspDetach', { command = 'setl foldexpr<' })


-- function _G.custom_fold_text()
--   local line = vim.fn.getline(vim.v.foldstart)
--   local line_count = vim.v.foldend - vim.v.foldstart + 1
--   return " ⚡ " .. line .. ": " .. line_count .. " lines"
-- end

-- vim.o.foldtext = 'v:lua.custom_fold_text()'
-- vim.o.foldtext = "getline(v:foldstart) .. '...'"
vim.o.foldtext = ""

--vim.o.fillchars = 'fold: '
-- vim.o.statuscolumn = ''
-- vim.o.fillchars = 'eob: ' -- Clear unused end-of-buffer space
vim.opt.fillchars:append({fold = " "})

---------------------------------------------------------------------

-- vim.o.signcolumn = 'no' -- Hide the sign column
vim.o.signcolumn = 'auto' -- Hide the sign column, when not needed
-- vim.o.signcolumn = 'yes' -- Keep signcolumn on by default
-- vim.o.signcolumn = 'number' -- Draw Signs in the number column


-- quick-scope (if you dont set those, it is always visible)
--vim.g.qs_highlight_on_keys = { 'f', 'F', 't', 'T' }

-- https://vi.stackexchange.com/questions/14451/move-cursor-to-new-line-character
-- see :help 've
 -- vim.o.virtualedit = 'onemore'

-- Set to true if you have a Nerd Font installed and selected in the terminal
vim.g.have_nerd_font = true
vim.opt.wrap = false

-- automatically close the existing buffer when a file is
-- reopened from the dashboard or anywhere else
vim.o.hidden = true

-- See `:help vim.opt`
-- NOTE: You can change these options as you wish!
--  For more options, you can see `:help option-list`

-- Make line numbers default
--vim.opt.number = true
vim.opt.number = false
-- You can also add relative line numbers, to help with jumping.
--  Experiment for yourself to see if you like it!
vim.opt.relativenumber = true



-- Enable mouse mode, can be useful for resizing splits for example!
vim.opt.mouse = 'a'

-- Disable mouse for specific window types
--vim.api.nvim_set_option('mouse', 'a') -- Deprecated

-- Don't show the mode, since it's already in the status line
vim.opt.showmode = false

-- Sync clipboard between OS and Neovim.
--  Schedule the setting after `UiEnter` because it can increase startup-time.
--  Remove this option if you want your OS clipboard to remain independent.
--  See `:help 'clipboard'`
vim.schedule(function()
  vim.opt.clipboard = 'unnamedplus'
end)

-- Enable break indent
vim.opt.breakindent = true

-- Save undo history
vim.opt.undofile = true

-- Case-insensitive searching UNLESS \C or one or more capital letters in the search term
vim.opt.ignorecase = true
vim.opt.smartcase = true


-- Decrease update time
vim.opt.updatetime = 250 -- Default is 4000

-- Decrease mapped sequence wait time
-- Displays which-key popup sooner
vim.opt.timeoutlen = 300

-- Configure how new splits should be opened
vim.opt.splitright = true
vim.opt.splitbelow = true

-- Sets how neovim will display certain whitespace characters in the editor.
--  See `:help 'list'`
--  and `:help 'listchars'`
vim.opt.list = true
--vim.opt.listchars = { tab = '» ', trail = '·', nbsp = '␣' }
vim.opt.listchars = { tab = '  ', trail = ' ', nbsp = '␣' }

-- Preview substitutions live, as you type!
vim.opt.inccommand = 'split'

-- Show which line your cursor is on
vim.opt.cursorline = false

-- Minimal number of screen lines to keep above and below the cursor.
vim.opt.scrolloff = 8
vim.opt.sidescrolloff = 18

--------------------------------------------------------------------------


-- Fix those errors while using the terminal
-- It cleans its buffer when you close the nterminal
-- after you have used Alt + h to open the terminal
-- E948: Job still running
-- E676: No matching autocommands for buftype= buffer

--vim.cmd([[autocmd TermClose * stopinsert]])

--[[
vim.cmd([[
  augroup TerminalCleanup
    autocmd!
    autocmd TermClose * lua require('my_terminal').cleanup()
  augroup END
]] --)


----------------------------------- NVCHAD ---------------------------------------------

local opt = vim.opt
local o = vim.o
local g = vim.g

-------------------------------------- globals -----------------------------------------
g.toggle_theme_icon = "   "

-------------------------------------- options ------------------------------------------

-- https://github.com/neovim/neovim/pull/17266
o.laststatus = 3 -- 2 means - always show the status line

o.showmode = false

o.clipboard = "unnamedplus"
o.cursorline = true
o.cursorlineopt = "number"

-- Indenting
o.expandtab = true    -- (true) Convert tabs to spaces
o.shiftwidth = 2      -- Number of spaces for each step of (auto)indent
vim.o.softtabstop = 2 -- Number of spaces a <Tab> counts for while editing
o.tabstop = 2         -- Number of spaces a <Tab> in the file counts for
o.softtabstop = 2

-- https://vi.stackexchange.com/questions/5818/what-is-the-difference-between-autoindent-and-smartindent-in-vimrc
o.smartindent = true
o.autoindent = true

-- These ingore the value of smartindent if true
--o.cindent = false
--o.indentexpr = false -- This should be a string


opt.fillchars:append({eob = " "})
-- opt.fillchars:append("space:*")

--[[
opt.fillchars = {
  eob = " ",        -- Existing setting for end-of-buffer
  horiz = '─',      -- Character for horizontal separators
  horizup = '┬',    -- Character for horizontal separator pointing up
  horizdown = '┴',  -- Character for horizontal separator pointing down
  vertleft = '├',   -- Character for vertical separator pointing left
  vertright = '┤',  -- Character for vertical separator pointing right
  verthoriz = '┼'   -- Character for vertical separator connecting horizontal
}
]] --

--o.ignorecase = true
--o.smartcase = true
--o.mouse = "a"

-- Numbers
o.number = true
o.numberwidth = 1 -- (default: 2) Minimum width of the line number column
o.ruler = false   -- Show the cursor position all the time

-- disable nvim intro
opt.shortmess:append "sI"

-- o.signcolumn = "yes"
o.splitbelow = true
o.splitright = true
o.timeoutlen = 400
o.undofile = true

-- interval for writing swap file to disk, also used by gitsigns
o.updatetime = 250


-- go to previous/next line with h,l,left arrow and right arrow
-- when cursor reaches end/beginning of line
-- you need to feed it all the keys appended one after another
opt.whichwrap:append "<>[]hl"
--opt.whichwrap:append "<C-h><C-l>" -- Illegal to have <C-*> here


-- disable some default providers
g.loaded_node_provider = 0
g.loaded_python3_provider = 0
g.loaded_perl_provider = 0
g.loaded_ruby_provider = 0


-- add yours here!

-- local o = vim.o
-- o.cursorlineopt ='both' -- to enable cursorline!

--vim.o.sessionoptions = "blank,buffers,curdir,folds,help,tabpages,winsize,winpos,terminal,localoptions"
--vim.o.sessionoptions = "buffers,curdir,folds,help,tabpages,winsize,localoptions"

-------------------------- Quick Fix List -------------------------------

-- When Cmake runs a build tool like make, ninja or msbuild
-- They print lines like /path/to/file.cpp:42:10: error: 'x' was not declared in this scope
-- Then when you use setqflist, it parses them with regex based rules
-- With the default error format, the diagnostic type in the quickfixlist
-- is not parsed correctly and instead, is put in the beginning of the text

-- Check current one with :verbose set errorformat?
-- Check the quickfixlist table :lua print(vim.inspect(vim.fn.getqflist()))

-- vim.opt.errorformat = "%f:%l:%c:%m,%f:%l:%m,%f  %l:%m,%f:%l%m,%f  %l%m" -- Default
-- vim.opt.errorformat = "%f:%l:%c: %t%*[^:]: %m" -- Clang and GCC error formats

--[[vim.opt.errorformat = table.concat({
  "%f:%l:%c:%t%*[^:]:%m",
  "%-G%f:%l:%c: note:%m",
  "%f:%l:%m",
  "%f(%l):%m",
  "%f:%l%m",
}, ",")]]--

--[[vim.opt.makeprg = "ninja -C build"
vim.opt.errorformat = table.concat({
  "%f:%l:%c: %t%*[^:]: %m",   -- clang/gcc style
  "%-G%f:%l:%c: note: %m",    -- skip notes
  "%f:%l:%m",                 -- fallback
}, ",")]]--

-- vim.opt.makeprg = "cmake --build build -- -k 0"
--[[vim.opt.errorformat = table.concat({
  -- ✅ Clang/GCC/Ninja output
  "%f:%l:%c: %t%*[^:]: %m",   -- matches: file:line:col: error/warning: message
  "%f:%l:%c: %m",             -- fallback: file:line:col: message
  "%f:%l:%m",                 -- fallback: file:line: message
  "%f(%l): %m",               -- MSVC-style fallback

  -- ⚙️ CMake status/info lines
  "%-G-- %m",                 -- ignore “-- …” messages (optional)
  "%-G%[%d/%d%]% %m",         -- ignore Ninja progress lines like [1/10] Building…
  "%E-- %m",                  -- treat “-- …” as success/info if you want them included
}, ",")]]--

-- vim.opt.errorformat = table.concat({
--   "%f:%l:%c: %t%*[^:]: %m",   -- clang/gcc style
--   "%f:%l:%c: %m",             -- fallback
--   "%f:%l:%m",
--   "%f(%l): %m",
--
--   -- "%-G%[%*[^]]%]% %m",        -- ignore [n/m] ninja progress
--   "%-G%*[^]] %m",
--
--   "%-G-- %m",                 -- ignore cmake status
--   "%E-- %m",                  -- optional: include cmake info as entries
-- }, ",")

--[[vim.opt.errorformat = table.concat({
  -- ✅ Clang/GCC style errors & warnings
  "%f:%l:%c: %t%*[^:]: %m",   -- file:line:col: error|warning: message
  "%f:%l:%c: %m",             -- fallback with no type
  "%f:%l:%m",                 -- fallback with no col
  "%f(%l): %m",               -- MSVC-style fallback

  -- ⚙️ Ninja progress lines like [1/10] … (ignored)
  "%-G%*[^:]: Building%*[^:]",

  -- ⚙️ Generic Ninja/CMake info lines (ignored)
  "%-G-- %m",                 -- CMake status lines
  "%-G%*[^:]: Linking%*[^:]", -- linker progress

  -- 🏗️ Optional: include CMake “done” messages as quickfix entries
  "%E-- %m",                  -- CMake entries if you want them in qf

  -- "%-G%*[^:]: note%*[^:]",     -- Clang notes
}, ",")]]--

vim.opt.errorformat = table.concat({
  -- ✅ Clang/GCC style errors & warnings
  "%f:%l:%c: %t%*[^:]: %m",   -- file:line:col: error|warning: message
  "%f:%l:%c: %m",             -- fallback with no type
  "%f:%l:%m",                 -- fallback with no col
  "%f(%l): %m",               -- MSVC-style fallback
  "%-G%*[^:]: note%*[^:]",     -- Clang notes (ignored for type but preserved)

  -- ⚙️ Ninja progress lines
  "%-G%*[^:]: Building%*[^:]", -- [n/m] Building…
  "%-G%*[^:]: Linking%*[^:]",  -- Linking…

  -- ⚙️ CMake informational lines
  "%-G-- %m",                  -- CMake status lines like -- Configuring done
  "%-G-- Build files have been written", -- explicit CMake completion line
  "%-G-- Generating done",      -- optional
  "%E-- %m",                   -- include as quickfix entry if desired

  -- 🏗️ Optional: everything else fallback
  "%f:%l%m",                   -- generic fallback
}, ",")


-- Display the quickfix list in a floating window:
--[[vim.fn.setqflist({}, "r", {lines = vim.fn.getqflist()})
vim.api.nvim_open_win(vim.fn.getqflist(), true, {
  relative = "editor",
  width = 80,
  height = 15,
  row = 3,
  col = 10,
  style = "minimal",
  border = "single"
})]]--



