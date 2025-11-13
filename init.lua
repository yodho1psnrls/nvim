-- Set <space> as the leader key
-- See `:help mapleader`
--  NOTE: Must happen before plugins are loaded (otherwise wrong leader will be used)
vim.g.mapleader = ' '
vim.g.maplocalleader = ' '

-- Using :lua vim.fn.system('git --version') does nothing
-- so it looks like vim's api cannot recognize my sys vars on PATH
-- Hope this fixes the issue with gitsigns not recognizing i have git
-- , even though i have it installed with pacman and its on PATH
-- vim.env.PATH = 'C:/msys64/usr/bin;' .. vim.env.PATH
-- Use :lua print(vim.env.Path) and you will see that this is already there

-- auto-seesion option
--vim.o.sessionoptions = "buffers,curdir,folds,help,tabpages,winsize,localoptions"
vim.o.sessionoptions = "blank,buffers,curdir,folds,help,tabpages,winsize,winpos,terminal,localoptions"

-- nvim-tree options
-- disable netrw at the very start of your init.lua
vim.g.loaded_netrw = 1
vim.g.loaded_netrwPlugin = 1
vim.opt.termguicolors = true -- optionally enable 24-bit colour

-- Moving options up here, because of quick-scope plugin
require "config.options"

require("config.lazy")
require("lazy").setup("plugins")


--opts.rocks.enabled = false

--[[require("lazy").setup("plugins", {
  rocks = {
    enabled = false, -- Disable luarocks support
  },
})]]--




-- Load additional configurations
 -- require "config.options"
require "config.autocmds"
require "config.commands"
--require "config.mappings"
require "config.border"



-- load theme (if necessary for your setup)
--dofile(vim.g.base46_cache .. "defaults")
--dofile(vim.g.base46_cache .. "statusline")


---------------------------------


-- Schedule mappings to be loaded after all configurations
vim.schedule(function()
  require "config.keymaps"
  require "config.build"
  require "config.whichwrap"
  --require "config.recent_buffers"
  require "config.compiler_profiling"
end)


