----------------------------- KICKSTARTER -----------------------------------------

-- If you never want Neovim's Python settings
-- This disables all built-in filetype settings, not just Python
-- vim.g.did_load_filetypes = 1
-- vim.g.did_load_ftplugin = 1


-- Folding options
-- zc folds by method, h or l on the folded line, unfolds it

vim.o.foldcolumn = '0' -- '0' is not bad, '1' is default | "auto" | "auto:number"
-- vim.o.foldcolumn = 'auto' -- '0' is not bad, '1' is default
vim.o.foldlevel = 99 -- Using ufo provider need a large value, feel free to decrease the value
vim.o.foldlevelstart = 99 -- Starts with all folds open
-- vim.o.foldlevelstart = 0 -- Starts with all folds closed
vim.o.foldenable = true -- Automatic folding when entering a buffer

--vim.o.foldmethod = 'indent' -- With treesitter
vim.o.foldmethod = 'expr'     -- With LSP
-- vim.o.foldmethod = 'manual'-- If you want only manual folding (fold them yourself only)
vim.o.foldexpr = 'v:lua.vim.lsp._get_fold_level()'

-- vim.o.signcolumn = 'no' -- Hide the sign column
vim.o.signcolumn = 'auto' -- Hide the sign column, when not needed
-- vim.o.signcolumn = 'yes'
-- vim.o.signcolumn = 'number' -- Draw Signs in the number column

--vim.o.fillchars = 'fold: '
-- vim.o.statuscolumn = ''
-- vim.o.fillchars = 'eob: ' -- Clear unused end-of-buffer space




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

-- Keep signcolumn on by default
-- vim.opt.signcolumn = 'yes'

-- Decrease update time
vim.opt.updatetime = 250

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
o.expandtab = true    -- Convert tabs to spaces
o.shiftwidth = 2      -- Number of spaces for each step of (auto)indent
o.smartindent = true
vim.o.softtabstop = 2 -- Number of spaces a <Tab> counts for while editing
o.tabstop = 2         -- Number of spaces a <Tab> in the file counts for
o.softtabstop = 2

o.autoindent = true

opt.fillchars = { eob = " " }
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



