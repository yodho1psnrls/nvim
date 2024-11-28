-- A hacky way to make whichwrap to not activate while skipping whole words

--vim.keymap.set('n', '<Shift>', function() print("Pressed Shift") end, { noremap = true, silent = false })

-- (This is dependant on the Terminal !!!)
-- b is move back one word
-- w is move forward one word
--vim.api.nvim_set_keymap('n', '<S-Left>', '<cmd>lua vim.opt.whichwrap = ""<CR>b', { noremap = true, silent = true })  -- Moves left one word

--[[
local disable_whichwrap_on_shift = function(mode, key)
  --return '<cmd>lua local ww_app_keys = vim.opt.whichwrap:get(); vim.opt.whichwrap = ""; vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("' .. key .. '", true, false, true), "' .. mode .. '", false); vim.opt.whichwrap = ww_app_keys<CR>'
  return string.format(
    "<cmd>lua local ww_app_keys = vim.opt.whichwrap:get(); vim.opt.whichwrap = ''; vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes('%s', true, false, true), '%s', false); vim.opt.whichwrap = ww_app_keys<CR>",
    key, mode)
end]] --

--[[local ww_app_keys = vim.opt.whichwrap:get()
local disable_whichwrap_on_shift = function(mode, key)
  vim.opt.whichwrap = ""
--  vim.wait(1000)
  vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes(key, true, true, true), mode, false)
--  vim.opt.whichwrap = ww_app_keys
end]] --

--[[for _, key in ipairs({"<S-Left>", "<S-Right>", "H", "L"}) do
  --vim.keymap.set('n', key, function() disable_whichwrap_on_shift('n', key) end,
  vim.keymap.set('n', key, disable_whichwrap_on_shift('n', key),
    { noremap = true, silent = true })
end]] --


--vim.api.nvim_create_user_command('TestLeft', function()
--  vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<S-Left>", true, false, true), "n", false)
--end, {})

----------------------------------------------------------------------------------------

--[[
local first_word_begin = function ()
  local cursor = vim.api.nvim_win_get_cursor(0)  -- Get current cursor position (row, col)
  local line = vim.fn.getline(cursor[1])  -- Get the current line content
  -- Find the position of the first non-space character (the start of the first word)
  return line:find("%S")
end

vim.api.nvim_create_user_command('FirstPos', function()
  print(first_word_begin())
end, {})
]] --



--[[
local last_word_begin = function ()
  -- Get the current line under the cursor
--  local line = vim.fn.getline('.')
--  local last_word_start, last_word_end = line:find("%S+%s*$")

  -- If a last word is found, return its position, otherwise return nil
--  if last_word_start == nil then return nil end
--  return line:find(last_word_start)


  local cursor = vim.api.nvim_win_get_cursor(0)  -- Get current cursor position (row, col)
  local line = vim.fn.getline(cursor[1])  -- Get the current line content
  -- Find the position of the first non-space character (the start of the first word)

  --return line:find("%S")
  local last_word_start = line:match(".*%s()(%S+)$")
  return last_word_start

end

vim.api.nvim_create_user_command('LastPos', function()
  print(last_word_begin())
end, {})
]] --

--[[
local function get_word_positions()
  local cursor = vim.api.nvim_win_get_cursor(0) -- Get current cursor position (row, col)
  local line = vim.fn.getline(cursor[1])        -- Get the current line content

  local last_word_start = line:match(".*%s()(%S+)$")
  --local last_word_start, last_word_end = line:find("%S+%s*$")

  vim.api.nvim_win_set_cursor(0, { cursor[1], last_word_start - 1 })
end

vim.keymap.set('n', '<leader>t', get_word_positions, { noremap = true, silent = false })
]] --

--[[
local function left_shift_whichwrap()
  local cursor = vim.api.nvim_win_get_cursor(0) -- Get current cursor position (row, col)
  local line = vim.fn.getline(cursor[1])        -- Get the current line content

  -- Find the position of the first non-space character (the start of the first word)
  local first_word_start = line:find("%S") -- this is nil at empty line, but because of the first check we dont care

  -- Check if the cursor is at the beginning of the line or before the first word
  if cursor[2] == 0 then
    --vim.api.nvim_win_set_cursor(0, {vim.fn.line('.') - 1, vim.fn.col('$') - 1})
    vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<Left>", true, false, true), "n", false)
  elseif cursor[2] < first_word_start then
    -- Move the cursor to the beginning of the line
    vim.api.nvim_win_set_cursor(0, { cursor[1], 0 })
  else
    vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<S-Left>", true, false, true), "n", false)
  end
end


local function right_shift_whichwrap()
  local cursor = vim.api.nvim_win_get_cursor(0) -- Get current cursor position (row, col)
  local line = vim.fn.getline(cursor[1])        -- Get the current line content

  -- Find the position of the first non-space character (the start of the first word)
  local first_word_start = line:find("%S")

  -- Check if the cursor is at the beginning of the line or before the first word
  if cursor[2] == 0 then
    --vim.api.nvim_win_set_cursor(0, {vim.fn.line('.') - 1, vim.fn.col('$') - 1})
    vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<Left>", true, false, true), "n", false)
  elseif cursor[2] < first_word_start then
    -- Move the cursor to the beginning of the line
    vim.api.nvim_win_set_cursor(0, { cursor[1], 0 })
  else
    vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<S-Left>", true, false, true), "n", false)
  end
end


-- Set the keymap to trigger the function on <S-Left>
vim.keymap.set('n', '<S-Left>', left_shift_whichwrap, { noremap = true, silent = true })
]] --


-- My logic of how whichwrap should behave during Shift hold

-- normal b, w, h, j, k, l 

local M = {}

function M.left_shift_whichwrap()
  -- Get current previous position (row, col)
  local cursor0 = vim.api.nvim_win_get_cursor(0)

  -- Check if the cursor is at the beginning of the line
  if cursor0[2] == 0 then
    --vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<Left>", true, false, true), "n", false)    
    --vim.cmd('normal! k')
    --vim.cmd('normal! $')
    vim.cmd('normal! h')
    cursor0 = vim.api.nvim_win_get_cursor(0)
    vim.api.nvim_win_set_cursor(0, { cursor0[1], cursor0[2] + 1 })
  else
    --vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<S-Left>", true, false, true), "n", false)
    vim.cmd('normal! b')
    local cursor1 = vim.api.nvim_win_get_cursor(0)

    if cursor0[1] ~= cursor1[1] then
      vim.api.nvim_win_set_cursor(0, { cursor0[1], 0 })
    end
  end
end


function M.right_shift_whichwrap(offset)
return function()
  -- Get current previous position (row, col)
  local cursor0 = vim.api.nvim_win_get_cursor(0)
  local line_len = #vim.fn.getline(cursor0[1])
  --local end_pos = #line - 1
  local end_pos = line_len + offset

  -- Check if the cursor is at the end of the line
  if line_len == 0 or cursor0[2] == end_pos then
    --vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<Left>", true, false, true), "n", false)
    vim.cmd('normal! l')
  else
    --vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<S-Left>", true, false, true), "n", false)
    vim.cmd('normal! w')
    local cursor1 = vim.api.nvim_win_get_cursor(0)

    if cursor0[1] ~= cursor1[1] then
      vim.api.nvim_win_set_cursor(0, { cursor0[1], end_pos })
      --vim.cmd('normal! h')
    end
  end
end
end

return M

--[[
-- 's' - select mode

local opts = { noremap = true, silent = true }

vim.keymap.set('n', '<S-Left>', left_shift_whichwrap, opts)
vim.keymap.set('n', '<S-Right>', right_shift_whichwrap(-1), opts)
vim.keymap.set('n', 'b', left_shift_whichwrap, opts)
vim.keymap.set('n', 'w', right_shift_whichwrap(-1), opts)

--oiaengoinaegoin  oiaenfonaefon oeaoaeg

-- How to skip words efficiently in insert mode ?
-- suggestions:
-- Ctrl + [ and ] (okay, but [ and ] serve specific purpose in normal)
-- Ctrl + b and w (okay, but Ctrl + b in insert goes to begin of line )
-- Best Option
-- Ctrl + n and (m or .) (these dont do anything reasonable neither in insert or normal mode)






vim.keymap.set({'i', 't', 'v'}, '<S-Left>', left_shift_whichwrap, opts)
vim.keymap.set({'i', 't', 'v'}, '<S-Right>', right_shift_whichwrap(0), opts)
vim.keymap.set({'i', 't', 'v'}, '<C-Left>', left_shift_whichwrap, opts)
vim.keymap.set({'i', 't', 'v'}, '<C-Right>', right_shift_whichwrap(0), opts)
]]--



--aefuaenfunaeufn





