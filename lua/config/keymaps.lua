local map = vim.keymap.set
local opts = { noremap = true, silent = true }

--[[
Normal	n
Visual	v
Visual Line	V
Visual Block	 (Ctrl-V)
Select	s
Select Line	S
Select Block	 (Ctrl-S)
Insert	i
Replace	R
Virtual Replace	Rv
Command-line	c
Terminal	t
Ex-Mode	!
]]--

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



--=============================== MOVEMENT ========================================--
-- :help ins-special-keys.
-- https://github.com/unblevable/quick-scope
-- https://stackoverflow.com/questions/19204396/jump-to-the-next-word-in-insert-mode
-- https://www.reddit.com/r/neovim/comments/13bzn8i/moving_around_efficiently_while_inserting_text/

-- TIP: Disable arrow keys in normal mode
-- vim.keymap.set('n', '<left>', '<cmd>echo "Use h to move!!"<CR>')
-- vim.keymap.set('n', '<right>', '<cmd>echo "Use l to move!!"<CR>')
-- vim.keymap.set('n', '<up>', '<cmd>echo "Use k to move!!"<CR>')
-- vim.keymap.set('n', '<down>', '<cmd>echo "Use j to move!!"<CR>')

map({"i", "n"}, "<C-b>", "<ESC>^i", { desc = "move beginning of line" })
--map("i", "<C-e>", "<End>", { desc = "move end of line" })
map({"i", "n"}, "<C-e>", "<End>", { desc = "move end of line" })

map({"i", 'n', 't', 'v'}, "<A-h>", "<Left>", opts)
map({"i", 'n', 't', 'v'}, "<A-l>", "<Right>", opts)
map({"i", 'n', 't', 'v'}, "<A-j>", "<Down>", opts)
map({"i", 'n', 't', 'v'}, "<A-k>", "<Up>", opts)

-- for some reason, if noremap is true, it doesnt work
-- https://stackoverflow.com/questions/6920943/navigating-in-vims-command-mode
map('c', "<A-h>", "<Left>", {noremap = false})
map('c', "<A-j>", "<Down>", {noremap = false})
map('c', "<A-k>", "<Up>", {noremap = false})
map('c', "<A-l>", "<Right>", {noremap = false})


-- Scroll up and down half screen, while centered (Thanks Priemagean)
map({'n', 'v'}, "<C-j>", "<C-d>zz", opts)
map({'n', 'v'}, "<C-k>", "<C-u>zz", opts)
map({'i'}, "<C-j>", "<Esc><C-d>zzi", opts)
map({'i'}, "<C-k>", "<Esc><C-u>zzi", opts)

--map({"i", 'n', 't', 'v'}, "<C-h>", "<S-Left>", opts)
--map({"i", 'n', 't', 'v'}, "<C-l>", "<S-Right>", opts)

local ww = require('config.whichwrap')

map('n', '<C-h>', ww.left_shift_whichwrap, opts)
map('n', '<C-l>', ww.right_shift_whichwrap(-1), opts)
map({'i', 'v'}, '<C-h>', ww.left_shift_whichwrap, opts)
map({'i', 'v'}, '<C-l>', ww.right_shift_whichwrap(0), opts)

map({'c', 't'}, "<C-h>", "<S-Left>", {noremap = false})
map({'c', 't'}, "<C-l>", "<S-Right>", {noremap = false})

-- Non-Normal Mode easy to reach keys for erasing a character
map({'i', 't', 'c'}, '<C-x>', '<Del>', {noremap = false})
map({'i', 't', 'c'}, '<C-d>', '<BS>', {noremap = false})




--  See `:help wincmd` for a list of all window commands
--map("n", "<C-h>", "<C-w>h", { desc = "switch window left" })
map("n", "<S-h>", "<C-w>h", opts)
map("n", "<S-l>", "<C-w>l", opts)
map("n", "<S-j>", "<C-w>j", opts)
map("n", "<S-k>", "<C-w>k", opts)
map('n', '<S-Left>', '<C-w><C-h>', opts)
map('n', '<S-Right>', '<C-w><C-l>', opts)
map('n', '<S-Down>', '<C-w><C-j>', opts)
map('n', '<S-Up>', '<C-w><C-k>', opts)


---------------------------------------------------------------------------

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



-- Go to the next buffer
map('n', '<Tab>', ':bnext<CR>', { noremap = true, silent = true, desc = "Go to next buffer" })

-- Go to the previous buffer
map('n', '<S-Tab>', ':bprevious<CR>', { noremap = true, silent = true, desc = "Go to previous buffer" })

-- Delete a buffer (works directly out of neovim)
--map('n', '<leader>x', ':bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })



-- Comment
map("n", "<leader>/", "gcc", { desc = "Toggle Comment", remap = true })
map("v", "<leader>/", "gc", { desc = "Toggle comment", remap = true })





map("n", "<leader>m", ":messages<CR>", { desc = "Messages" })

-- terminal
map("t", "<C-x>", "<C-\\><C-N>", { desc = "terminal escape terminal mode" })


-------------------------------------------------------------------------------------


-- whichkey
map("n", "<leader>wK", "<cmd>WhichKey <CR>", { desc = "whichkey all keymaps" })

map("n", "<leader>wk", function()
  vim.cmd("WhichKey " .. vim.fn.input "WhichKey: ")
end, { desc = "whichkey query lookup" })


--[[ -- This works only for NvChad !
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
]]--

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
map({'n', 'v'}, '<Del>', '"_x', opts);

-- make Ctrl + r in input mode the same as Ctrl + r in normal mode (Undo)
map('i', '<C-r>', '<Esc><C-r>i', opts)

map("n", "<CR>", "i<CR><Esc>", opts)
--map("n", "<BS>", "i<BS><Esc>", { noremap = true, desc = "Backspace in insert mode" })
map("n", "<BS>", "i<Right><BS><Esc>", opts)
--map("n", "<BS>", "i<BS><Right><Esc>", { noremap = true, desc = "Backspace in insert mode" })


--ctrl + v for normal, insert and visual modes
map("n", "<C-v>", "p", opts)
map("i", "<C-v>", "<Esc>pi", opts)
map("v", "p", "P", opts)
map("v", "P", "p", opts)

-- https://www.reddit.com/r/neovim/comments/v7s1ts/how_do_i_avoid_replacing_the_content_of_my/
--map("v", "<C-v>", '"_dp', { noremap = true, desc = "Paste text" })
--map("v", "<C-v>", 'P', { noremap = true, desc = "Paste text" })
--map("v", "<C-v>", '"0p', { noremap = true})
--map("v", "<C-p>", "P", {noremap = true})
--map("v", "<C-v>", 'P', { noremap = false, desc = "Paste text" })
 -- map("v", "<C-v>", 'p', { noremap = false, desc = "Paste text" })

--map('n', '<S-Space>', 'i n', { noremap = true, silent = true })
--map('n', '<S-Space>', '<cmd>lua print("jaja") <CR>', { noremap = true, silent = true })





--map("n", "<CR>", ":", { desc = "CMD enter command mode" })

--map('n', '<C-;>', ':!', { noremap = true, silent = false })
--map('n', '<C-/>', ':!', { noremap = true, silent = false })

--map("n", ";", ":", { desc = "CMD enter command mode" })
--map("n", ",", ":", { desc = "CMD enter command mode" })
--map("n", "h", ":", { desc = "CMD enter command mode" })
--map("i", "jk", "<ESC>")

-- map({ "n", "i", "v" }, "<C-s>", "<cmd> w <cr>")



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


-- To change the CWD(Current Working Directory)
-- to the one containing the currently opened file
map('n', '<leader>cd', ':cd %:p:h<CR>', opts)


-- TIP: Disable arrow keys in normal mode
--vim.keymap.set('n', '<left>', '<cmd>echo "Use h to move!!"<CR>')
--vim.keymap.set('n', '<right>', '<cmd>echo "Use l to move!!"<CR>')
--vim.keymap.set('n', '<up>', '<cmd>echo "Use k to move!!"<CR>')
--vim.keymap.set('n', '<down>', '<cmd>echo "Use j to move!!"<CR>')


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


---------------------------------------------------------------------------------

-- NvimTree
--map("n", "<C-n>", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
-- map("n", "<leader>n", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
map("n", "<leader>n", function() require('nvim-tree.api').tree.toggle({ find_file = true, focus = true }) end,
  { desc = "nvimtree toggle window" })
--map("n", "<leader>e", "<cmd>NvimTreeFocus<CR>", { desc = "nvimtree focus window" })

-- telescope
map("n", "<leader>fg", "<cmd>Telescope live_grep<CR>", { desc = "telescope Live Grep" })
map("n", "<leader>fb", "<cmd>Telescope buffers<CR>", { desc = "telescope find buffers" })
map("n", "<leader>fh", "<cmd>Telescope help_tags<CR>", { desc = "telescope help page" })
--map("n", "<leader>ma", "<cmd>Telescope marks<CR>", { desc = "telescope find marks" })
map("n", "<leader>fm", "<cmd>Telescope marks<CR>", { desc = "telescope find marks" })
map("n", "<leader>fo", "<cmd>Telescope oldfiles<CR>", { desc = "telescope find oldfiles" })
map("n", "<leader>fz", "<cmd>Telescope current_buffer_fuzzy_find<CR>", { desc = "telescope find in current buffer" })

map("n", "<leader>gc", "<cmd>Telescope git_commits<CR>", { desc = "telescope git commits" })
map("n", "<leader>gs", "<cmd>Telescope git_status<CR>", { desc = "telescope git status" })
map("n", "<leader>gg", "<cmd>Gitsigns<CR>", { desc = "telescope git signs" })
map("n", "<leader>gf", require('telescope.builtin').git_files, { desc = "telescope git signs" })

map("n", "<leader>pt", "<cmd>Telescope terms<CR>", { desc = "telescope pick hidden term" })

-- This is NvChad Specific
--map("n", "<leader>th", "<cmd>Telescope themes<CR>", { desc = "telescope nvchad themes" })

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

-- This doesnt save any buffers, nor tell you for unsaved buffers between switching sessions !
map("n", "<leader>fs", '<cmd>Telescope session-lens search_session<CR>', { desc = "telescope session lens" })


-- https://www.reddit.com/r/neovim/comments/ypaq3e/lsp_find_reference_results_in_telescope/
-- https://github.com/Slotos/telescope-lsp-handlers.nvim
map('n', '<leader>fr', function() require('telescope.builtin').lsp_references() end,
  { noremap = true, silent = true, desc = "LSP Find References" })

-- https://github.com/parmardiwakar150/neovim-config/blob/main/lua/core/plugin_config/telescope.lua#L37
map('n', '<leader>ch', function() require('telescope.builtin').command_history() end,
  { noremap = true, silent = true, desc = "Command History" })




