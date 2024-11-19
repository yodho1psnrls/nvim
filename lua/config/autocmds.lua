
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


--local function save_session()
--  vim.cmd("AutoSessionSave")
  -- Schedule the next save
--  vim.defer_fn(save_session, 900000)  -- 900000 ms = 15 minutes
--end

-- Start the first save
--save_session()





