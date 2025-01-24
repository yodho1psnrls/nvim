local map = vim.keymap.set
local opts = { noremap = true, silent = true }
local util = require("utilities")


-- map('n', '<leader>oo', util.outline_cpp_inline_methods,
map('n', '<leader>oo', util.move_current_fold_contents_after,
  { desc = "[O]utline cpp inline methods", noremap = true, silent = true })

map('n', '<leader>on', function ()
  vim.cmd("edit " .. util.get_project_root() .. "/notes.txt")
end, { desc = "Open [N]otes", noremap = true, silent = true })


map("n", "<leader>fe", function()
    local path = vim.fn.expand("%:p:h") -- Get the current file's directory
    if path == "" then
        path = vim.fn.getcwd() -- Fall back to the working directory
    end
    local cmd
    if vim.loop.os_uname().sysname == "Windows_NT" then
        cmd = "start explorer " .. vim.fn.shellescape(path) -- Windows
    elseif vim.fn.has("mac") == 1 then
        cmd = "open " .. vim.fn.shellescape(path) -- macOS
    else
        cmd = "xdg-open " .. vim.fn.shellescape(path) -- Linux | ensure xdg-open is installed
    end
    vim.fn.system(cmd)
end, { noremap = true, silent = true, desc = "File [E]xplorer" })



--[[-- Keymap Modes:
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


map('n', '<leader>q', '<cmd>q<CR>', { desc = '[Q]uit Window', noremap = true, silent = true })
-- map('n', '<leader>x', '<cmd>Bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })
map('n', '<leader>x', function ()
  if vim.bo.buftype == 'terminal' then
    require("bufdelete").bufdelete(0, true) -- Force close of terminal buffers
  else
    require("bufdelete").bufdelete(0, false)
  end
end, { noremap = true, silent = true, desc = "Buffer delete" })

map('n', '<leader>cd', '<cmd>cd %:p:h<CR>', {desc = "Set cwd to current file", noremap = true })

-- map("n", "<leader>m", "<cmd>messages<CR>", { desc = "Messages", noremap = true })

map("n", "<leader>m", function () -- <leader>wm
  util.open_messages_in_buffer()
  vim.keymap.set('n', '<leader>m' , function()
    vim.cmd('Bdelete')
    end, { buffer = 0, noremap = true, silent = true })
end, { desc = "[M]essages in buffer", noremap = true, silent = true })


--map('n', '<Tab>', '<C-o>', { noremap = true, silent = true, desc = "Go to next jump" })
--map('n', '<S-Tab>', '<C-i>', { noremap = true, silent = true, desc = "Go to previous jump" })

--map('n', '<Tab>', '<cmd>bnext<CR>', { noremap = true, silent = true, desc = "Go to next buffer" })
--map('n', '<S-Tab>', '<cmd>bprevious<CR>', { noremap = true, silent = true, desc = "Go to previous buffer" })

--[[map('n', '<Tab>', function()
    vim.cmd('Telescope buffers')
    vim.defer_fn(function()
      vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<CR>", true, true, true), "n", true)
    end, 150)
  end,
  { noremap = false , silent = true, desc = "Go to next buffer" })

map('n', '<S-Tab>',
  '<cmd>Telescope buffers<CR><Esc><S-Tab><CR>',
  { noremap = false, silent = true, desc = "Go to previous buffer" })]]--


map("n", "<Esc>", "<cmd>nohlsearch<CR>", { noremap = false, desc = "Clear search highlights" })
map('t', '<Esc>', '<C-\\><C-n>', { noremap = false, desc = 'Exit terminal mode' })


-- NOTE: This mapping is toggled ON only when the cursor is in a writable buffer
--  See config/autocmds.lua
map("n", "<CR>", "i<CR><Esc>", { noremap = false, desc = "Enter as in Insert Mode" })
--map("t", "<CR>", "i<CR><Esc>", { noremap = false, desc = "Enter as in Insert Mode" })


map("n", "<leader>ln", "<cmd>set nu!<CR>", { desc = "Toggle line number" })
map("n", "<leader>rn", "<cmd>set rnu!<CR>", { desc = "Toggle relative number" })

-- Toggle comment on line/s
map("n", "<leader>/", "gcc", { desc = "Toggle Comment", remap = true })
map("v", "<leader>/", "gc", { desc = "Toggle comment", remap = true })


-----------------------------------------------------------------------------------
--===============================================================================--

-- Sideways Mouse Scroll
-- https://stackoverflow.com/questions/43915661/how-to-move-screen-left-right-or-center-it-horizontally-without-moving-cursor-in
-- https://neovim.io/doc/user/scroll.html#_6.-scrolling-with-a-mouse-wheel
-- https://github.com/neovim/neovim/issues/6211

map('n', '<A-ScrollWheelUp>', '10zh', opts)
map('n', '<A-ScrollWheelDown>', '10zl', opts)

map('i', '<A-ScrollWheelUp>', '<Esc>10zhi', opts)
map('i', '<A-ScrollWheelDown>', '<Esc>10zli', opts)

map('v', '<A-ScrollWheelUp>', '10zh', opts)
map('v', '<A-ScrollWheelDown>', '10zl', opts)


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
-- map('n', '<leader>rp', '<cmd>Replace ', opts)


map("v", "p", "P", opts)
map("v", "P", "p", opts)

-- https://www.reddit.com/r/neovim/comments/v7s1ts/how_do_i_avoid_replacing_the_content_of_my/
-- https://www.reddit.com/r/neovim/comments/1e1dmpw/what_are_the_keymaps_that_you_replaced_default/

map({'n', 'v'}, 'x', '"_x', opts)
map({'n', 'v'}, 'X', '"_X', opts)

map({"n", "v"}, "s", '"_s', opts)
map({"n", "v"}, "S", '"_S', opts)
map({"n", "v"}, "c", '"_c', opts)
map({"n", "v"}, "C", '"_C', opts)


--==========================================================================--

map({'n', 'v'}, "<C-d>", "<C-d>zz", opts)
map({'n', 'v'}, "<C-u>", "<C-u>zz", opts)


-- (ARROWS) Window Navigation
map('n', '<C-Left>', '<C-w><C-h>', opts)
map('n', '<C-Right>', '<C-w><C-l>', opts)
map('n', '<C-Down>', '<C-w><C-j>', opts)
map('n', '<C-Up>', '<C-w><C-k>', opts)

-- (HJKL) Window Switch
--  See `:help wincmd` for a list of all window commands
map("n", "<C-h>", "<C-w>h", opts)
map("n", "<C-j>", "<C-w>j", opts)
map("n", "<C-k>", "<C-w>k", opts)
map("n", "<C-l>", "<C-w>l", opts)



--================================================================================--


map('n', '<C-s>', '<cmd>w<CR>', { noremap = true, silent = true, desc = "Close buffer" })
map('i', '<C-s>', '<Esc><cmd>w<CR>a', { noremap = true, silent = true, desc = "Close buffer" })





--=============================== PLUGINS ========================================--
-- The following keymaps are for plugins that are lazy loaded, and the key binding
--  should be set up here, so it loads the plugin, if used

--map("n", "<C-n>", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
-- map("n", "<leader>n", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
vim.keymap.set("n", "<leader>n", function()
    require('nvim-tree.api').tree.toggle({ find_file = true, focus = true })
    --require('nvim-tree.api').tree.toggle({ find_file = true, focus = false })
  end,
{ desc = "[N]vimtree toggle window" })
--map("n", "<leader>e", "<cmd>NvimTreeFocus<CR>", { desc = "nvimtree focus window" })


-- Keymap to jump to the folder containing the current file under cursor
--[[vim.keymap.set("n", "<leader>ft", function()
  local api = require("nvim-tree.api")
  local node = api.tree.get_node_under_cursor()
  if node and node.type == "file" and node.parent then
    api.tree.toggle(node.parent) -- Toggle the parent folder open
    api.tree.focus(node.parent) -- Move the cursor to the parent folder
  end
end, { desc = "Jump to the folder containing current file", silent = true })]]--


-- Keymap to collapse the folder containing the current file
vim.keymap.set("n", "<leader>fd", function()
  local api = require("nvim-tree.api")
  local node = api.tree.get_node_under_cursor()
  if node and node.parent and not node.nodes then
    api.node.navigate.parent_close()
  end
end, { desc = "Collapse folder containing current file", silent = true })

-- Toggle-able Keymap to toggle the folder containing the current file
--[[vim.keymap.set("n", "<leader>tf", function()
  local api = require("nvim-tree.api")
  local node = api.tree.get_node_under_cursor()
  if not node then return end

  -- Check if the cursor is over a folder and toggle its state
  if node.type == "directory" then
    if node.open then
      api.node.navigate.parent_close()
    else
      api.node.open.edit()
    end
  elseif node.parent then
    -- For a file, toggle its parent folder
    if node.parent.open then
      api.node.navigate.parent_close()
    else
      api.node.open.edit()
    end
  end
end, { desc = "Toggle folder containing current file", silent = true })]]--



