

--7AA62C --BD5637
vim.cmd [[
  highlight QuickScopePrimary guifg=#7AA62C gui=underline ctermfg=155 cterm=underline
  highlight QuickScopeSecondary guifg=#CA5C3B gui=underline ctermfg=81 cterm=underline
]]

vim.cmd("colorscheme rose-pine-moon")



vim.api.nvim_create_user_command("PrintBuffers", function()
  local bufs = vim.fn.getbufinfo({ bufloaded = 1 })
  vim.notify(vim.inspect(bufs), vim.log.levels.INFO)
end, {})

-- TODO: Make it such that when you open a new buffer
--  if previously the only open buffer was empty buffer
--  to remove it

-- "BufAdd", "BufEnter"
--[[
vim.api.nvim_create_autocmd("BufReadPost", {
  callback = function()
    -- Get all listed buffers
    local bufs = vim.fn.getbufinfo({ buflisted = 1 })

    -- Filter out hidden buffers
    local non_hidden_bufs = vim.tbl_filter(function(buf)
      return buf.hidden == 0 and buf.listed == 1
    end, bufs)

    if #non_hidden_bufs == 2 then
      local empty_buf = nil
      for _, buf in ipairs(non_hidden_bufs) do
        -- Check if the buffer is empty (no name, 1 line)
        if buf.name == "" and buf.linecount == 1 then
          empty_buf = buf.bufnr
        end
      end
      if empty_buf then
        vim.schedule(function()
          -- Delete the empty buffer
          vim.cmd("Bdelete " .. empty_buf)
        end)
      end
    end
  end,
})
]]--

-- NOTE: Trying to make diagnostics virtual text to also show in input mode

-- Show diagnostics when entering insert mode
--vim.cmd [[ autocmd InsertEnter * lua vim.diagnostic.open_float(nil, { focusable = false, scope = "cursor", border = "rounded", width = 40 }) ]]
-- Hide diagnostics when leaving insert mode
--vim.cmd [[ autocmd InsertLeave * lua vim.diagnostic.hide(nil) ]]

-- Optional: Customize Diagnostic Floating Window Behavior
--[[
vim.lsp.handlers["textDocument/publishDiagnostics"] = function(_, result, ctx, config)
  -- Make the diagnostic window appear a little less intrusive
  vim.diagnostic.set(qf_list, result.diagnostics, bufnr)
  vim.diagnostic.open_float(nil, {
    focusable = false,
    scope = "cursor",  -- show at the cursor position
    border = "rounded",  -- make the border rounded for a nicer UI
    width = 40,  -- control window width
  })
end
]]--


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

-- Lazy load gitsigns, when you change directories and it happens
--  that there is a .git folder inside your current working directory
vim.api.nvim_create_autocmd({'VimEnter', 'DirChanged'}, {
  pattern = '*',
  callback = function()
    -- Check if the directory contains a .git folder and trigger lazy load
    if vim.fn.isdirectory(vim.fn.expand('.git')) == 1 then
      require('lazy').load({ plugins = { 'gitsigns.nvim' } })
    end
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

-- Most of the themes dont have such highlights for nvim-cmp
--  (the autocomplete suggestions window), so keep them centralized
--  here in autocmds.lua, and not per theme config

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





