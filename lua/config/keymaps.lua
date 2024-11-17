-- print("KeyMaps are Loaded !") -- dubug message

-- list of all modes: { 'n', 'i', 'v', 'x', 's', 'o', 'c' }

--  See `:help vim.keymap.set()`

-- Clear highlights on search when pressing <Esc> in normal mode
--  See `:help hlsearch`
vim.keymap.set('n', '<Esc>', '<cmd>nohlsearch<CR>')

-- Diagnostic keymaps
vim.keymap.set('n', '<leader>ds', vim.diagnostic.setloclist, { desc = 'Open diagnostic [Q]uickfix list' })
--vim.keymap.set('n', '<leader>q', vim.diagnostic.setloclist, { desc = 'Open diagnostic [Q]uickfix list' })
vim.keymap.set('n', '<leader>q', ':q<CR>', { desc = 'Close Window', noremap = true, silent = true })


-- Exit terminal mode in the builtin terminal with a shortcut that is a bit easier
-- for people to discover. Otherwise, you normally need to press <C-\><C-n>, which
-- is not what someone will guess without a bit more experience.
--
-- NOTE: This won't work in all terminal emulators/tmux/etc. Try your own mapping
-- or just use <C-\><C-n> to exit terminal mode
--vim.keymap.set('t', '<Esc><Esc>', '<C-\\><C-n>', { desc = 'Exit terminal mode' })
vim.keymap.set('t', '<Esc>', '<C-\\><C-n>', { desc = 'Exit terminal mode' })


-- TIP: Disable arrow keys in normal mode
-- vim.keymap.set('n', '<left>', '<cmd>echo "Use h to move!!"<CR>')
-- vim.keymap.set('n', '<right>', '<cmd>echo "Use l to move!!"<CR>')
-- vim.keymap.set('n', '<up>', '<cmd>echo "Use k to move!!"<CR>')
-- vim.keymap.set('n', '<down>', '<cmd>echo "Use j to move!!"<CR>')

-- Keybinds to make split navigation easier.
--  Use CTRL+<hjkl> to switch between windows
--
--  See `:help wincmd` for a list of all window commands
vim.keymap.set('n', '<C-h>', '<C-w><C-h>', { desc = 'Move focus to the left window' })
vim.keymap.set('n', '<C-l>', '<C-w><C-l>', { desc = 'Move focus to the right window' })
vim.keymap.set('n', '<C-j>', '<C-w><C-j>', { desc = 'Move focus to the lower window' })
vim.keymap.set('n', '<C-k>', '<C-w><C-k>', { desc = 'Move focus to the upper window' })



--================================= NVCHAD =======================================--

local map = vim.keymap.set
local opts = { noremap = true, silent = true }


map("i", "<C-b>", "<ESC>^i", { desc = "move beginning of line" })
map("i", "<C-e>", "<End>", { desc = "move end of line" })
map("i", "<C-h>", "<Left>", { desc = "move left" })
map("i", "<C-l>", "<Right>", { desc = "move right" })
map("i", "<C-j>", "<Down>", { desc = "move down" })
map("i", "<C-k>", "<Up>", { desc = "move up" })

map("n", "<C-h>", "<C-w>h", { desc = "switch window left" })
map("n", "<C-l>", "<C-w>l", { desc = "switch window right" })
map("n", "<C-j>", "<C-w>j", { desc = "switch window down" })
map("n", "<C-k>", "<C-w>k", { desc = "switch window up" })

map("n", "<Esc>", "<cmd>noh<CR>", { desc = "General Clear highlights" })

map("n", "<C-s>", "<cmd>w<CR>", { desc = "General Save file" })
map("n", "<C-c>", "<cmd>%y+<CR>", { desc = "General Copy whole file" })

--map("n", "<leader>n", "<cmd>set nu!<CR>", { desc = "Toggle line number" })
map("n", "<leader>ln", "<cmd>set nu!<CR>", { desc = "Toggle line number" })
map("n", "<leader>rn", "<cmd>set rnu!<CR>", { desc = "Toggle relative number" })
--map("n", "<leader>ch", "<cmd>NvCheatsheet<CR>", { desc = "Toggle nvcheatsheet" })


-- global lsp mappings
--map("n", "<leader>q", vim.diagnostic.setloclist, { desc = "LSP Diagnostic loclist" })

-- tabufline
--map("n", "<leader>b", "<cmd>enew<CR>", { desc = "buffer new" })
--

--map("n", "<tab>", function()
--  require("nvchad.tabufline").next()
--end, { desc = "buffer goto next" })

--map("n", "<S-tab>", function()
--  require("nvchad.tabufline").prev()
--end, { desc = "buffer goto prev" })


-- Go to the next buffer
map('n', '<Tab>', ':bnext<CR>', { noremap = true, silent = true, desc = "Go to next buffer" })

-- Go to the previous buffer
map('n', '<S-Tab>', ':bprevious<CR>', { noremap = true, silent = true, desc = "Go to previous buffer" })

-- Delete a buffer (works directly out of neovim)
--map('n', '<leader>x', ':bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })



-- Comment
map("n", "<leader>/", "gcc", { desc = "Toggle Comment", remap = true })
map("v", "<leader>/", "gc", { desc = "Toggle comment", remap = true })

-- NvimTree
--map("n", "<C-n>", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
-- map("n", "<leader>n", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
map("n", "<leader>n", function() require('nvim-tree.api').tree.toggle({ find_file = true, focus = true }) end,
  { desc = "nvimtree toggle window" })
--map("n", "<leader>e", "<cmd>NvimTreeFocus<CR>", { desc = "nvimtree focus window" })

-- telescope
map("n", "<leader>fw", "<cmd>Telescope live_grep<CR>", { desc = "telescope live grep" })
map("n", "<leader>fb", "<cmd>Telescope buffers<CR>", { desc = "telescope find buffers" })
map("n", "<leader>fh", "<cmd>Telescope help_tags<CR>", { desc = "telescope help page" })
--map("n", "<leader>ma", "<cmd>Telescope marks<CR>", { desc = "telescope find marks" })
map("n", "<leader>fm", "<cmd>Telescope marks<CR>", { desc = "telescope find marks" })
map("n", "<leader>fo", "<cmd>Telescope oldfiles<CR>", { desc = "telescope find oldfiles" })
map("n", "<leader>fz", "<cmd>Telescope current_buffer_fuzzy_find<CR>", { desc = "telescope find in current buffer" })
map("n", "<leader>cm", "<cmd>Telescope git_commits<CR>", { desc = "telescope git commits" })
map("n", "<leader>gt", "<cmd>Telescope git_status<CR>", { desc = "telescope git status" })
map("n", "<leader>pt", "<cmd>Telescope terms<CR>", { desc = "telescope pick hidden term" })
map("n", "<leader>th", "<cmd>Telescope themes<CR>", { desc = "telescope nvchad themes" })
map("n", "<leader>ff", "<cmd>Telescope find_files<cr>", { desc = "telescope find files" })
map(
  "n",
  "<leader>fa",
  "<cmd>Telescope find_files follow=true no_ignore=true hidden=true<CR>",
  { desc = "telescope find all files" }
)
--[[map("n", "<leader>fm", function()
  require("conform").format { lsp_fallback = true }
end, { desc = "General Format file" })]]--

map("n", "<leader>fs", '<cmd>Telescope session-lens search_session<CR>', { desc = "telescope session lens" })


map("n", "<leader>m", ":messages<CR>", { desc = "Messages" })

-- terminal
map("t", "<C-x>", "<C-\\><C-N>", { desc = "terminal escape terminal mode" })

-- new terminals
map("n", "<leader>h", function()
  require("nvchad.term").new { pos = "sp" }
end, { desc = "terminal new horizontal term" })

map("n", "<leader>v", function()
  require("nvchad.term").new { pos = "vsp" }
end, { desc = "terminal new vertical window" })


-- toggleable
-- map({ "n", "t" }, "<A-v>", function()
-- require("nvchad.term").toggle { pos = "vsp", id = "vtoggleTerm" }
-- end, { desc = "terminal toggleable vertical term" })

-- map({ "n", "t" }, "<A-h>", function()
-- require("nvchad.term").toggle { pos = "sp", id = "htoggleTerm" }
-- end, { desc = "terminal new horizontal term" })

-------------------------------------------------------------------------------------

-- TODO: Make an equivalent of this for your build terminal in build.lua

--[[
-- Function to check if a terminal buffer with a specific name is open
local function is_terminal_open(name)
  for _, bufnr in ipairs(vim.api.nvim_list_bufs()) do
    if vim.api.nvim_buf_get_option(bufnr, "buftype") == "terminal" then
      local term_name = vim.api.nvim_buf_get_name(bufnr)
      if term_name:match(name) then
        return bufnr
      end
    end
  end
  return nil
end

-- Function to toggle terminal
local function toggle_terminal(name, pos)
  local bufnr = is_terminal_open(name)
  if bufnr then
    -- Close the terminal
    vim.api.nvim_buf_delete(bufnr, { force = true })
  else
    -- Open a new terminal
    require("nvchad.term").toggle { pos = pos, name = name }
  end
end

-- Toggleable vertical terminal
map({ "n", "t" }, "<A-v>", function()
  toggle_terminal("vtoggleTerm", "vsp")
end, { desc = "Toggle vertical terminal" })

-- Toggleable horizontal terminal
map({ "n", "t" }, "<A-h>", function()
  toggle_terminal("htoggleTerm", "sp")
end, { desc = "Toggle horizontal terminal" })
]] --

-------------------------------------------------------------------------------------

--map({ "n", "t" }, "<A-i>", function()
--  require("nvchad.term").toggle { pos = "float", id = "floatTerm" }
--end, { desc = "terminal toggle floating term" })

-- whichkey
map("n", "<leader>wK", "<cmd>WhichKey <CR>", { desc = "whichkey all keymaps" })

map("n", "<leader>wk", function()
  vim.cmd("WhichKey " .. vim.fn.input "WhichKey: ")
end, { desc = "whichkey query lookup" })

-- blankline
map("n", "<leader>cc", function()
  local config = { scope = {} }
  config.scope.exclude = { language = {}, node_type = {} }
  config.scope.include = { node_type = {} }
  local node = require("ibl.scope").get(vim.api.nvim_get_current_buf(), config)

  if node then
    local start_row, _, end_row, _ = node:range()
    if start_row ~= end_row then
      vim.api.nvim_win_set_cursor(vim.api.nvim_get_current_win(), { start_row + 1, 0 })
      vim.api.nvim_feedkeys("_", "n", true)
    end
  end
end, { desc = "blankline jump to current context" })

-----------------------------------------------------------------------------------
--===============================================================================--


-- add yours here

-- Bind 'Lazy update' to the 'u' key globally
--map('n', 'u', ':Lazy update<CR>', opts)


--[[
-- Set horizontal scrolling
vim.opt.ttymouse = 'xterm2'
-- Optional: Custom command to adjust scrolling speed
-- You can define custom commands or keybindings here
map('n', '<S-ScrollWheelLeft>', ':exec "normal! " . (v:count1 + 5) . "zl"<CR>', opts)
map('n', '<S-ScrollWheelRight>', ':exec "normal! " . (v:count1 + 5) . "zr"<CR>', opts)
]] --

-- Sideways Mouse Scroll
-- https://stackoverflow.com/questions/43915661/how-to-move-screen-left-right-or-center-it-horizontally-without-moving-cursor-in
-- https://neovim.io/doc/user/scroll.html#_6.-scrolling-with-a-mouse-wheel
-- https://github.com/neovim/neovim/issues/6211


--map('n', '<C-ScrollWheelUp>', '10zh', opts)
--map('n', '<C-ScrollWheelDown>', '10zl', opts)

--map('n', '<S-ScrollWheelUp>', '10zh', opts)
--map('n', '<S-ScrollWheelDown>', '10zl', opts)

--map('n', '<ScrollWheelLeft>', '10zh', opts)
--map('n', '<ScrollWheelRight>', '10zl', opts)

map('n', '<A-ScrollWheelUp>', '10zh', opts)
map('n', '<A-ScrollWheelDown>', '10zl', opts)

map('i', '<A-ScrollWheelUp>', '<Esc>10zhi', opts)
map('i', '<A-ScrollWheelDown>', '<Esc>10zli', opts)

map('v', '<A-ScrollWheelUp>', '10zh', opts)
map('v', '<A-ScrollWheelDown>', '10zl', opts)


--shift+arrow selection
--https://stackoverflow.com/questions/9721732/mapping-shift-arrows-to-selecting-characters-lines
--[[
-- Normal mode mappings
map('n', '<S-Up>', 'v<Up>', opts)
map('n', '<S-Down>', 'v<Down>', opts)
map('n', '<S-Left>', 'v<Left>', opts)
map('n', '<S-Right>', 'v<Right>', opts)

-- Visual mode mappings
map('v', '<S-Up>', '<Up>', opts)
map('v', '<S-Down>', '<Down>', opts)
map('v', '<S-Left>', '<Left>', opts)
map('v', '<S-Right>', '<Right>', opts)

-- Insert mode mappings
map('i', '<S-Up>', '<Esc>v<Up>', opts)
map('i', '<S-Down>', '<Esc>v<Down>', opts)
map('i', '<S-Left>', '<Esc>v<Left>', opts)
map('i', '<S-Right>', '<Esc>v<Right>', opts)
]] --



-- Toggle between insert and normal mode (Double Tab \)
-- Inspired by jj in here:
-- https://medium.com/@adamregaszrethy/vim-where-to-remap-esc-6f556d11fb75
--map('i', '\\\\', '<Esc>', opts)
--map('n', '\\', 'i', opts)


-- Replace wordA with wordB when <leader>rp is pressed
-- vim.keymap.set('n', '<leader>rp', function()
-- vim.cmd('%s/wordA/wordB/g')
-- end, { noremap = true, silent = true, desc = "Replace wordA with wordB" })

-- Create a custom command to replace 'wordA' with 'wordB'
--[[
vim.api.nvim_create_user_command(
  'Replace',
  function(opts)
    local wordA = opts.args:match("([^ ]+)")
    local wordB = opts.args:match(" ([^ ]+)$")
    vim.cmd('%s/' .. wordA .. '/' .. wordB .. '/g')
  end,
  { nargs = 1, desc = "Replace wordA with wordB in the current file" }
)
]] --

-- Create a custom command to replace 'wordA' with 'wordB' as whole words only
vim.api.nvim_create_user_command(
--'ReplaceWholeWords',
  'Replace',
  function(opts)
    local wordA = opts.args:match("([^ ]+)")
    local wordB = opts.args:match(" ([^ ]+)$")
    vim.cmd('%s/\\<' .. wordA .. '\\>/' .. wordB .. '/g')
  end,
  { nargs = 1, desc = "Replace whole wordA with wordB in the current file" }
)


--map('n', '\\', '<Tab>', opts) -- \ acts as a second Tab


map('v', '<C-c>', '"*y', opts)
map('i', '<C-s>', '<Esc>:w<CR>a', opts)

-- select whole file in visual mode
map('i', '<C-a>', '<Esc>ggVG', opts)
map('n', '<C-a>', 'ggVG', opts)


map('n', '<C-z>', 'u', opts) -- Map Ctrl + Z to undo
-- map('n', '<C-S-z>', '<C-r>', opts) -- Map Ctrl + Shift + Z to redo

-- Note: Some terminals or Neovim setups might not support Ctrl + Shift + Z directly
map('i', '<C-z>', '<Esc>ui', opts) -- Map Ctrl + Z to undo
-- map('i', '<C-S-z>', '<C-r>', opts) -- Map Ctrl + Shift + Z to redo

map('v', '<C-x>', 'd', opts) -- Map Ctrl + X to cut (delete and yank) the selected text in visual mode

-- Map Backspace to delete the selected text without yanking in visual mode
map('v', '<BS>', '"_d', opts)

-- make Ctrl + r in input mode the same as Ctrl + r in normal mode (Undo)
map('i', '<C-r>', '<Esc><C-r>i', opts)



--map('n', '<leader>cm', ':CMakeBuild<CR>', opts)
--map('n', '<leader>f', ':Format<CR>', opts)


--<leader>db: The keybinding uses <leader> followed by d and b.
-- This is a common pattern in many Neovim configurations, where <leader>
-- is a custom key (usually mapped to \ or , by default).
--:Dashboard<CR>: This is the command that opens the Dashboard.
-- The <CR> part simulates the "Enter" key, executing the command.
--opts: { noremap = true, silent = true } ensures the mapping is
-- non-recursive and silent, meaning it won't print the command in the
-- command line when triggered.

-- Previous Dashboard keymap
map('n', '<leader>db', ':Dashboard<CR>', opts)

-- Function to close all windows except the current one
local function close_all_splits()
  local current_buf = vim.api.nvim_get_current_buf()
  local windows = vim.api.nvim_list_wins()
  for _, win in ipairs(windows) do
    local buf = vim.api.nvim_win_get_buf(win)
    if buf ~= current_buf then
      vim.api.nvim_win_close(win, true)
    end
  end
end

-- Function to open dashboard and close all splits
local function open_dashboard()
  close_all_splits()
  vim.cmd('Dashboard') -- Adjust if your dashboard plugin uses a different command
end

-- Map <Leader>db to the function
-- vim.api.nvim_set_keymap('n', '<Leader>db', '<Cmd>lua open_dashboard()<CR>', opts)




-- To change the CWD(Current Working Directory)
-- to the one containing the currently opened file
map('n', '<leader>cd', ':cd %:p:h<CR>', opts)


-- Use Telescope to search through command history
--  You can also do that with the up and down arrow keys !
-- This binds <leader>fc to open a fuzzy finder for your command history,
--  making it easy to re-run commands without retyping them.
-- map('n', '<leader>fc', ':Telescope command_history<CR>', opts)

-- Clear highlights on search when pressing <Esc> in normal mode
--  See `:help hlsearch`
vim.keymap.set('n', '<Esc>', '<cmd>nohlsearch<CR>')


-- Exit terminal mode in the builtin terminal with a shortcut that is a bit easier
-- for people to discover. Otherwise, you normally need to press <C-\><C-n>, which
-- is not what someone will guess without a bit more experience.
--
-- NOTE: This won't work in all terminal emulators/tmux/etc. Try your own mapping
-- or just use <C-\><C-n> to exit terminal mode
-- vim.keymap.set('t', '<Esc><Esc>', '<C-\\><C-n>', { desc = 'Exit terminal mode' })
vim.keymap.set('t', '<Esc>', '<C-\\><C-n>', { desc = 'Exit terminal mode' })


-- TIP: Disable arrow keys in normal mode
-- vim.keymap.set('n', '<left>', '<cmd>echo "Use h to move!!"<CR>')
-- vim.keymap.set('n', '<right>', '<cmd>echo "Use l to move!!"<CR>')
-- vim.keymap.set('n', '<up>', '<cmd>echo "Use k to move!!"<CR>')
-- vim.keymap.set('n', '<down>', '<cmd>echo "Use j to move!!"<CR>')




-- Keybinds to make split navigation easier.
--  Use CTRL+<hjkl> or CTRL + <arrow keys> to switch between windows
--
--  See `:help wincmd` for a list of all window commands
--[[ -- setup for using hjkl
map('n', '<C-h>', '<C-w><C-h>', { desc = 'Move focus to the left window' })
map('n', '<C-l>', '<C-w><C-l>', { desc = 'Move focus to the right window' })
map('n', '<C-j>', '<C-w><C-j>', { desc = 'Move focus to the lower window' })
map('n', '<C-k>', '<C-w><C-k>', { desc = 'Move focus to the upper window' })
]] --

--[[
map('n', '<C-j>', '<C-w><C-h>', { desc = 'Move focus to the left window' })
map('n', '<C-;>', '<C-w><C-l>', { desc = 'Move focus to the right window' })
map('n', '<C-k>', '<C-w><C-j>', { desc = 'Move focus to the lower window' })
map('n', '<C-l>', '<C-w><C-k>', { desc = 'Move focus to the upper window' })
]] --

map('n', '<C-Left>', '<C-w><C-h>', { desc = 'Move focus to the left window, using arrow keys' })
map('n', '<C-Right>', '<C-w><C-l>', { desc = 'Move focus to the right window, using arrow keys' })
map('n', '<C-Down>', '<C-w><C-j>', { desc = 'Move focus to the lower window, using arrow keys' })
map('n', '<C-Up>', '<C-w><C-k>', { desc = 'Move focus to the upper window, using arrow keys' })



-- SHIFT THE hjkl keys to jkl; (one key to the right)
-- Remap `j` to move left
--vim.api.nvim_set_keymap('n', 'j', 'h', { noremap = true, silent = true })
-----map('n', 'j', 'h', { noremap = true, silent = true })

-- Remap `k` to move down
--vim.api.nvim_set_keymap('n', 'k', 'j', { noremap = true, silent = true })
-----map('n', 'k', 'j', { noremap = true, silent = true })

-- Remap `l` to move up
--vim.api.nvim_set_keymap('n', 'l', 'k', { noremap = true, silent = true })
-----map('n', 'l', 'k', { noremap = true, silent = true })

-- Remap `;` to move right
--vim.api.nvim_set_keymap('n', ';', 'l', { noremap = true, silent = true })
-----map('n', ';', 'l', { noremap = true, silent = true })

-- Now remap the unused H to be what ; did
--map('n', 'h', ';', { noremap = true, silent = true })

---------------------------------------------------------
-- Ctrl + ; in insert mode doesnt work !!!

-- Do the same mappings for the insert mode + Ctrl
--map('i', '<C-j>', '<Left>', { noremap = true, silent = true })
--map('i', '<C-k>', '<Down>', { noremap = true, silent = true })
--map('i', '<C-l>', '<Up>', { noremap = true, silent = true })
--map('i', '<C-;>', '<Right>', { noremap = true, silent = true })

----------------------------------------------------------

-- Make Enter in normal mode to enter command mode

--ctrl + v for normal, insert and visual modes
map("n", "<C-v>", "p", { desc = "Paste text" })
map("i", "<C-v>", "<Esc>pi", { desc = "Paste text" })
map("v", "<C-v>", '"_dp', { desc = "Paste text" })


map("n", "<CR>", "i<CR><Esc>", { desc = "New line in insert mode" })
map("n", "<BS>", "i<BS><Esc>", { desc = "Backspace in insert mode" })

--map("n", "<CR>", ":", { desc = "CMD enter command mode" })

--map('n', '<C-;>', ':!', { noremap = true, silent = false })
--map('n', '<C-/>', ':!', { noremap = true, silent = false })

--map("n", ";", ":", { desc = "CMD enter command mode" })
--map("n", ",", ":", { desc = "CMD enter command mode" })
--map("n", "h", ":", { desc = "CMD enter command mode" })
--map("i", "jk", "<ESC>")

-- map({ "n", "i", "v" }, "<C-s>", "<cmd> w <cr>")



-- Add key bindings for compiling and running C++

-- For Unix Shell
-- Compile and Run
--map('n', '<F5>', ':!g++ -o %< % && if [ -x %< ]; then ./%<; fi<CR>', { noremap = true, silent = true })
-- Compile Only
--map('n', '<F6>', ':!g++ -o %< % && echo "Compiled successfully!" || echo "Compilation failed!"<CR>', { noremap = true, silent = true })
-- Run Only
--map('n', '<F7>', ':!./%<<CR>', { noremap = true, silent = true })

-- For Windows Command Prompt
-- Compile and Run
--map('n', '<F5>', ':!g++ -o hello hello.cpp && hello.exe<CR>', { noremap = true, silent = true })
-- Compile Only
--map('n', '<F6>', ':!g++ -o hello hello.cpp && echo Compilation successful!<CR>', { noremap = true, silent = true })
-- Run Only
--map('n', '<F7>', ':!hello.exe<CR>', { noremap = true, silent = true })

-- For Windows PowerShell
-- Compile and Run
-- map('n', '<F5>', ':!g++ -o hello hello.cpp if (Test-Path ./hello.exe) { ./hello }<CR>', { noremap = true, silent = true })
-- Compile Only
-- map('n', '<F6>', ':!g++ -o hello hello.cpp echo "Compilation successful!"<CR>', { noremap = true, silent = true })
-- Run Only
-- map('n', '<F7>', ':!./hello<CR>', { noremap = true, silent = true })


--[[
-- Function to compile and run the project
local function build_and_run()
    vim.cmd('!cmake --build build')
    --vim.cmd('!build\\myprogram.exe')  -- Adjust `myprogram.exe` to your executable's name
	vim.cmd('!build\\myprogram.exe')  -- Adjust `myprogram.exe` to your executable's name
end

-- Function to compile the project
local function build_only()
    vim.cmd('!cmake --build build')
end

-- Function to run the project
local function run_only()
    vim.cmd('!build\\myprogram.exe')  -- Adjust `myprogram.exe` to your executable's name
end
]] --

-- Key mappings
--vim.api.nvim_set_keymap('n', '<F5>', [[:lua build_and_run()<CR>]], { noremap = true, silent = true })
--vim.api.nvim_set_keymap('n', '<F6>', [[:lua build_only()<CR>]], { noremap = true, silent = true })
--vim.api.nvim_set_keymap('n', '<F7>', [[:lua run_only()<CR>]], { noremap = true, silent = true })
