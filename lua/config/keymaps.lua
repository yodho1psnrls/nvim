local map = vim.keymap.set
local opts = { noremap = true, silent = true }
local util = require("utilities")

-- NOTE: The local(location) list can be opened with :lopen
-- While the global(quickfix) list can be opened with :copen
-- You can navigate it quickly with
-- :cnext, :cprev, :lnext, :lprev
-- ]q      [q      ]l      [l

-- <C-k> used for window navigation
-- map("n", "<C-k>", function()
--   vim.lsp.buf.hover({
--     border = 'rounded',
--     -- focusable = true, -- Can you focus it with a mouse
--     focusable = false, -- Can you focus it with a mouse
--     focus = false,    -- Should it focus the window | Can you focus it with jump through windows keys
--     silent = true,
--   })
-- end, opts)

map("n", "<leader>cm", "<cmd>OpenCMD<CR>", opts)

-- NOTE: ALT == ESC
map({'i', 't', 'c'}, '<A-*>', '<Esc>', { noremap = true });

map("n", "<leader>lz", "<cmd>Lazy<CR>",
  {desc = "Open Lazy", noremap = true, silent = true})

-- map({"n", "v"}, "<S-h>", "20zh", { noremap = true, silent = true })
-- map({"n", "v"}, "<S-l>", "20zl", { noremap = true, silent = true })

-- NOTE: Use <C-U> and <C-D>
-- map({"n", "v"}, "<S-h>", "zH", { noremap = true, silent = true })
-- map({"n", "v"}, "<S-l>", "zL", { noremap = true, silent = true })
-- map({"n", "v"}, "<S-j>", "<S-Down>", { noremap = true, silent = true })
-- map({"n", "v"}, "<S-k>", "<S-Up>", { noremap = true, silent = true })

-- NOTE: Overwriting the default one, because it has no borders
map('n', '<S-k>', function()
  vim.lsp.buf.hover({
    border = 'rounded',
    -- focusable = true, -- Can you focus it with a mouse
    focusable = false, -- Can you focus it with a mouse
    focus = false,    -- Should it focus the window | Can you focus it with jump through windows keys
    silent = true,
  })
end, opts)

map('n', '<leader>ss', function()
  -- TODO:
  -- -- local nvim_tree = util.safe_require("nvim-tree.view")
  -- local nvim_tree = util.safe_require("nvim-tree")
  -- local is_nvim_tree_open = false
  -- if nvim_tree then
  --  -- is_nvim_tree_open = nvim_tree.is_visible()
  --  is_nvim_tree_open = nvim_tree.view.is_visible()
  -- end
  --
  -- if is_nvim_tree_open then nvim_tree.toggle() end

  -- vim.cmd("Lazy load auto-session") -- Its loaded from start
  -- vim.cmd("SessionSave")
  vim.cmd("AutoSession save")

  -- if is_nvim_tree_open then nvim_tree.toggle() end
end, { desc = "SessionSave", noremap = true, silent = true })

map('n', '<leader>rt', function ()
  print("Project Root: " .. util.get_project_root())
end, { desc = "Project roo[T]", noremap = true, silent = true })

-- map('n', '<leader>oo', util.outline_cpp_inline_methods,
map('n', '<leader>oo', util.move_current_fold_contents_after,
  { desc = "[O]utline cpp inline methods", noremap = true, silent = true })

-- map('n', '<leader>on', function ()
--   vim.cmd("edit " .. util.get_project_root() .. "/notes.txt")
-- end, { desc = "Open [N]otes", noremap = true, silent = true })
vim.keymap.set("n", "<leader>j", "<cmd>Notes<CR>", { desc = "Open Notes" })

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
Normal  n
Visual  v
Visual Line V
Visual Block   (Ctrl-V)
Select  s
Select Line S
Select Block   (Ctrl-S)
Insert  i
Replace R
Virtual Replace Rv
Command-line  c
Terminal  t
Ex-Mode !
]]--

-- print("KeyMaps are Loaded !") -- dubug message

-- list of all modes: { 'n', 'i', 'v', 'x', 's', 'o', 'c' }

--  See `:help vim.keymap.set()`


map('n', '<leader>q', '<cmd>q<CR>', { desc = '[Q]uit Window', noremap = true, silent = true })
map('n', '<leader>Q', '<cmd>qa<CR>', { desc = '[Q]uit All Windows', noremap = true, silent = true })
-- map('n', '<leader>x', '<cmd>Bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })
map('n', '<leader>x', function ()
-- map('n', '<C-w>', function () -- NOTE: In most apps Ctrl + W closes the tab
  if vim.bo.buftype == 'terminal' then
    require("bufdelete").bufdelete(0, true) -- Force close of terminal buffers
  else
    require("bufdelete").bufdelete(0, false)
  end
end, { noremap = true, silent = true, desc = "Buffer delete" })

-- TODO:
-- map('n', '<leader>X', ' x q',
--   { noremap = false, silent = true, desc = "Delete buffer and close window" })

map('n', '<leader>cd', '<cmd>cd %:p:h<CR>', {desc = "Set cwd to current file", noremap = true })

-- map("n", "<leader>m", "<cmd>messages<CR>", { desc = "Messages", noremap = true })

map("n", "<leader>m", function () -- <leader>wm
  util.open_messages_in_buffer()
  vim.keymap.set('n', '<leader>m', util.close_messages_buffer
    , { buffer = 0, noremap = true, silent = true })
end, { desc = "[M]essages in buffer", noremap = true, silent = true })


-- Mainly used for Compiler error messages
-- vim.keymap.set("n", "<leader>k", function() require("quicker").toggle() end,
--   { desc = "[O]pen Quickfix list", noremap = true, silent = true })

-- Mainly used for Compiler error messages
map("n", "<leader>k", "<cmd>copen<CR>",
  { desc = "[O]pen Quickfix list", noremap = true, silent = true })
map("n", "<leader>K", "<cmd>copen | only<CR>",
  { desc = "[O]pen Quickfix list", noremap = true, silent = true })
-- map("n", "<leader>k", "<cmd>copen | only<CR>",
--   { desc = "[O]pen Quickfix list", noremap = true, silent = true })

--[[map("n", "<leader>k", function ()
  local qf_list = vim.fn.getqflist()
  if vim.tbl_isempty(qf_list) then
    vim.notify("Quickfix list is empty", vim.log.levels.INFO, { title = "Quickfix" })
    return
  end

  -- Toggle, doesnt work properly
  if util.is_qf_open() then
    vim.cmd("cclose")
  else
    vim.cmd("copen")
  end
end, { desc = "[O]pen quickfix list", noremap = true, silent = true })]]--

vim.api.nvim_create_autocmd("FileType", {
  pattern = "qf",
  callback = function()
    -- Set undo and redo to act as :colder and :cnewer when in the quickfix lists
    vim.keymap.set('n', 'u', ':colder<CR>', { buffer = true, silent = true })
    vim.keymap.set('n', '<C-r>', ':cnewer<CR>', { buffer = true, silent = true })

    vim.opt_local.number = false
    vim.opt_local.relativenumber = false
  end,
})


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

map('n', '<C-ScrollWheelUp>', '10zh', opts)
map('n', '<C-ScrollWheelDown>', '10zl', opts)

map('i', '<C-ScrollWheelUp>', '<Esc>10zhi', opts)
map('i', '<C-ScrollWheelDown>', '<Esc>10zli', opts)

map('v', '<C-ScrollWheelUp>', '10zh', opts)
map('v', '<C-ScrollWheelDown>', '10zl', opts)


-- Create a custom command to replace 'wordA' with 'wordB' as whole words only
vim.api.nvim_create_user_command(
--'ReplaceWholeWords',
  'Replace',
  function(opts)
    local wordA = opts.args:match("([^ ]+)")
    local wordB = opts.args:match(" ([^ ]+)$")
    vim.cmd('%s/\\<' .. wordA .. '\\>/' .. wordB .. '/g')
    vim.cmd('nohlsearch')
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
map("n", "<BS>", "<C-w>h", opts) -- If <C-h> gets interpreted as backspace by the terminal
map("n", "<C-j>", "<C-w>j", opts)
map("n", "<C-k>", "<C-w>k", opts)
map("n", "<C-l>", "<C-w>l", opts)

-- Optional: Also make it work in terminal mode if using terminal buffers
-- map("t", "<C-h>", "<C-\\><C-N><C-w>h", opts)
-- map("t", "<C-j>", "<C-\\><C-N><C-w>j", opts)
-- map("t", "<C-k>", "<C-\\><C-N><C-w>k", opts)
-- map("t", "<C-l>", "<C-\\><C-N><C-w>l", opts)

--================================================================================--


map('n', '<C-s>', '<cmd>w<CR>', { noremap = true, silent = true, desc = "Close buffer" })
map('i', '<C-s>', '<Esc><cmd>w<CR>a', { noremap = true, silent = true, desc = "Close buffer" })



--=============================== PLUGINS ========================================--
-- The following keymaps are for plugins that are lazy loaded, and the key binding
--  should be set up here, so it loads the plugin, if used



