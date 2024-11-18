
-- highlight the border between split windows
--vim.cmd([[highlight VertSplit guifg=#A9A9A9 gui=NONE " Color of the vertical split separator]])
--vim.cmd [[highlight VertSplit guifg=#A9A9A9 guibg=NONE]]

-- Set highlight for search matches
-- DarkRed
vim.cmd [[
  highlight Search ctermfg=Black ctermbg=Yellow guifg=Black guibg=Yellow
  highlight IncSearch ctermfg=Black ctermbg=Yellow guifg=Black guibg=Yellow
]]
--vim.cmd [[
--  highlight Search guifg=Yellow guibg=DarkRed
--  highlight IncSearch guifg=Black guibg=Yellow
--]]
-- Set highlight groups in Lua
--vim.api.nvim_set_hl(0, 'Search', { fg = 'Yellow', bg = 'DarkRed' })
--vim.api.nvim_set_hl(0, 'IncSearch', { fg = 'Black', bg = 'Yellow' })


-- By Default .cpp, .hpp, .cc, .cxx, .c++ files are usually detected as cpp.
-- but sometimes it detects .h files as c language files
-- This ensures it detects .h files as part of the cpp language
vim.api.nvim_create_autocmd({ "BufRead", "BufNewFile" }, {
  pattern = { "*.h" },
  callback = function()
    vim.bo.filetype = "cpp"
  end,
})


-- trigger the hover documentation (which often shows type,
-- function signatures, or other useful info) when hovering over a symbol.
--map('n', 'K', '<cmd>lua vim.lsp.buf.hover()<CR>', opts)

--hover documentation to appear automatically after hovering
--the cursor over a variable or function for a brief time
vim.cmd [[ autocmd CursorHold * lua vim.lsp.buf.hover() ]]

-- Diagnostics hover window
--vim.diagnostic.config({ virtual_text = false, })
--vim.cmd [[ autocmd CursorHold * lua vim.diagnostic.open_float(nil, { focusable = false }) ]]


-- Set the delay time in milliseconds (e.g., 1000 ms = 1 second)
-- vim.o.updatetime = 690 --660 --590

-- Set dashboard hyper theme weekend day color to white
--vim.cmd [[
--  highlight DashboardWeekdayHeader guifg=#FFFFFF guibg=NONE ctermfg=White ctermbg=NONE
--]]

---------- Because i cant see cmp select line while typing in a file -------------------
-- Override highlights for nvim-cmp
--[[
vim.cmd([[
  highlight! default link CmpPmenu Pmenu
  highlight! default link CmpSel PmenuSel
  highlight! default link CmpDoc NormalFloat
  highlight! default link CmpSignature NormalFloat
  highlight! default link CmpDocBorder FloatBorder
  highlight! default link CmpSignatureBorder FloatBorder
]] --
--)


-- Explicitly Define Highlights for Selected Items
--[[
vim.cmd([[
  highlight! CmpPmenu guibg=#1f1d2e guifg=#e0def4
  highlight! CmpSel guibg=#c4a7e7 guifg=#191724 gui=bold
  highlight! CmpDoc guibg=#2a273f guifg=#e0def4
  highlight! CmpSignature guibg=#2a273f guifg=#e0def4
  highlight! CmpDocBorder guifg=#c4a7e7
  highlight! CmpSignatureBorder guifg=#c4a7e7
]] --
--)

-- If you're using the rose-pine theme and it's causing these issues, you can load
-- the theme and then override just these highlights without changing the
-- rest of the theme
-- (apply your custom highlights)

vim.cmd([[
  highlight! CmpPmenu guibg=#1f1d2e guifg=#e0def4
  highlight! CmpSel guibg=#c4a7e7 guifg=#191724 gui=bold
  highlight! CmpDoc guibg=#2a273f guifg=#e0def4
  highlight! CmpSignature guibg=#2a273f guifg=#e0def4
  highlight! CmpDocBorder guifg=#c4a7e7
  highlight! CmpSignatureBorder guifg=#c4a7e7
]])

----------------------------------------------------------------------

----------------------------------------------------------------------
-- [[ Basic Autocommands ]]
--  See `:help lua-guide-autocommands`

-- Highlight when yanking (copying) text
--  Try it with `yap` in normal mode
--  See `:help vim.highlight.on_yank()`
vim.api.nvim_create_autocmd('TextYankPost', {
  desc = 'Highlight when yanking (copying) text',
  group = vim.api.nvim_create_augroup('kickstart-highlight-yank', { clear = true }),
  callback = function()
    vim.highlight.on_yank()
  end,
})


--------------------------------


-- Auto command to open nvim-tree when Neovim starts
--vim.api.nvim_create_autocmd('VimEnter', {
--  callback = function()
-- Open nvim-tree using the Vim command
--    vim.cmd('NvimTreeOpen')
--  end,
--  once = true,  -- Ensure it runs only once on startup
--})


--vim.api.nvim_create_autocmd('BufWinEnter', {
--  pattern = 'NvimTree_*',
--  callback = function()
--    vim.cmd('cd D:/')  -- Change D:/ to your desired default directory
--    vim.cmd('NvimTreeRefresh')
--  end,
--})


-- OPPEN NVIMTREE ON STARTUP
--[[vim.api.nvim_create_autocmd('VimEnter', {
  callback = function()
    -- Change to the desired directory
    vim.cmd('cd D:/')  -- Adjust this path to your desired directory

    -- Open NvimTree
    vim.cmd('NvimTreeOpen')
  end,
  once = true,  -- Ensure it runs only once on startup
})
--]]

-----------------------------------------------------------------------------

-- https://github.com/nvim-tree/nvim-tree.lua/wiki/Recipes#workaround-when-using-rmagattiauto-session
-- Workaround when using rmagatti/auto-session
--[[
vim.api.nvim_create_autocmd({ 'BufEnter' }, {
  pattern = 'NvimTree*',
  callback = function()
    local api = require('nvim-tree.api')
    local view = require('nvim-tree.view')

    if not view.is_visible() then
      api.tree.open()
    end
  end,
})
]]--


-- Make :bd and :q behave as usual when tree is visible
-- Is this compatible with nvim-bufdelete ???
vim.api.nvim_create_autocmd({ 'BufEnter', 'QuitPre' }, {
  nested = false,
  callback = function(e)
    local tree = require('nvim-tree.api').tree
    --local autosession = require('auto-session').sa

    -- Nothing to do if tree is not opened
    if not tree.is_visible() then
      return
    end

    -- Do nothing if we are focused over the nvim-tree window
    local current_win_id = vim.api.nvim_get_current_win()
    local tree_win_id = require('nvim-tree.view').get_winnr()
    if current_win_id == tree_win_id then
      return
    end

    -- How many focusable windows do we have? (excluding e.g. incline status window)
    local winCount = 0
    for _, winId in ipairs(vim.api.nvim_list_wins()) do
      if vim.api.nvim_win_get_config(winId).focusable then
        winCount = winCount + 1
      end
    end

    -- Check if there are any unsaved changes
    -- local has_unsaved_changes = false
    -- for _, buf in ipairs(vim.api.nvim_list_bufs()) do
      -- if vim.api.nvim_buf_is_modified(buf) then
        -- has_unsaved_changes = true
        -- break
      -- end
    -- end

    -- We want to quit and only one window besides tree is left
    if e.event == 'QuitPre' and winCount == 2 then
--      if (has_unsaved_changes) then
--        return
--      else

        --vim.api.nvim_cmd({ cmd = 'qall' }, {})

        -- This should fix quitting on unsaved buffers and save on session when the tree is open
        tree.toggle({ find_file = true, focus = true }) -- close nvim-tree: will go to the last buffer used before closing
        --vim.cmd('NvimTreeClose')
        vim.cmd('SessionSave')
        vim.api.nvim_cmd({ cmd = 'q' }, {})

--      end
    end

    -- :bd was probably issued an only tree window is left
    -- Behave as if tree was closed (see `:h :bd`)
    if e.event == 'BufEnter' and winCount == 1 then
      -- Required to avoid "Vim:E444: Cannot close last window"
      vim.defer_fn(function()
        -- close nvim-tree: will go to the last buffer used before closing
        tree.toggle({ find_file = true, focus = true })
        -- re-open nivm-tree
        tree.toggle({ find_file = true, focus = false })
      end, 10)
    end
  end
})


-- Make Nvim-Tree always focus the file that you are currently in
vim.api.nvim_create_autocmd({ 'BufEnter', 'WinEnter' }, {
  callback = function()
    local tree_api = require('nvim-tree.api')
    local tree = tree_api.tree
    local current_buf = vim.api.nvim_get_current_buf()
    local current_file = vim.api.nvim_buf_get_name(current_buf)

    -- Check if nvim-tree is visible and if the current buffer is valid (not empty and not modified)
--    if tree.is_visible() and current_file ~= "" and not vim.api.nvim_buf_get_option(current_buf, 'mod') then
      -- Focus on the current file in nvim-tree
--      tree.find_file(current_file)
--    end

    -- Check if nvim-tree is visible and if the current buffer has a valid file path
    if tree.is_visible() and current_file ~= "" then
      -- Check if the file exists using vim.fn.filereadable
      if vim.fn.filereadable(current_file) == 1 then
        -- Focus on the current file in nvim-tree
        tree.find_file(current_file)
      end
    end

  end,
})


-- Make Nvim-Tree to always focus the file that you are currently in
-- But if nvim-tree is not loaded(if its lazy loaded), it will not trigger
--[[
vim.api.nvim_create_autocmd({ 'BufEnter', 'WinEnter' }, {
  callback = function()
    local status_ok, tree_api = pcall(require, 'nvim-tree.api')
    if not status_ok then return end  -- Exit if nvim-tree is not loaded

    local tree = tree_api.tree
    local current_buf = vim.api.nvim_get_current_buf()
    local current_file = vim.api.nvim_buf_get_name(current_buf)

    -- Only focus if nvim-tree is visible
    if tree.is_visible() and current_file ~= "" and not vim.api.nvim_buf_get_option(current_buf, 'mod') then
      tree.find_file(current_file)  -- Focus on the current file
    end
  end,
})
]]--



--local function save_session()
--  vim.cmd("AutoSessionSave")
  -- Schedule the next save
--  vim.defer_fn(save_session, 900000)  -- 900000 ms = 15 minutes
--end

-- Start the first save
--save_session()





