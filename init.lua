-- Set <space> as the leader key
-- See `:help mapleader`
--  NOTE: Must happen before plugins are loaded (otherwise wrong leader will be used)
vim.g.mapleader = ' '
vim.g.maplocalleader = ' '


-- auto-seesion option
vim.o.sessionoptions = "buffers,curdir,folds,help,tabpages,winsize,localoptions"

-- nvim-tree options
-- disable netrw at the very start of your init.lua
vim.g.loaded_netrw = 1
vim.g.loaded_netrwPlugin = 1
vim.opt.termguicolors = true -- optionally enable 24-bit colour


require("config.lazy")
require("lazy").setup("plugins")


-- Load additional configurations
require "config.options"
require "config.autocmds"
--require "config.mappings"
require "config.border"

require "config.build"


-- load theme (if necessary for your setup)
--dofile(vim.g.base46_cache .. "defaults")
--dofile(vim.g.base46_cache .. "statusline")


---------------------------------


-- Schedule mappings to be loaded after all configurations
vim.schedule(function()
  --require "nvchad.options"   -- Load NVChad options
  --require "nvchad.mappings"  -- Load NVChad mappings
  require "config.keymaps"
  --require "nvchad.autocmds"  -- Load NVChad autocommands
end)
