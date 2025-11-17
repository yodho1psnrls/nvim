-- NOTE: Fix vowels with brackets in normal and visual mode

vim.keymap.set({'n', 'v'}, 'á', "'a")
vim.keymap.set({'n', 'v'}, 'ä', '"a')
vim.keymap.set({'n', 'v'}, 'Á', "'A")
vim.keymap.set({'n', 'v'}, 'Ä', '"a')

vim.keymap.set({'n', 'v'}, 'é', "'e")
vim.keymap.set({'n', 'v'}, 'ë', '"e')
vim.keymap.set({'n', 'v'}, 'É', "'E")
vim.keymap.set({'n', 'v'}, 'Ë', '"E')

vim.keymap.set({'n', 'v'}, 'í', "'i")
vim.keymap.set({'n', 'v'}, 'ï', '"i')
vim.keymap.set({'n', 'v'}, 'Í', "'I")
vim.keymap.set({'n', 'v'}, 'Ï', '"I')

vim.keymap.set({'n', 'v'}, 'ó', "'o")
vim.keymap.set({'n', 'v'}, 'ö', '"o')
vim.keymap.set({'n', 'v'}, 'Ó', "'O")
vim.keymap.set({'n', 'v'}, 'Ö', '"O')

vim.keymap.set({'n', 'v'}, 'ú', "'u")
vim.keymap.set({'n', 'v'}, 'ü', '"u')
vim.keymap.set({'n', 'v'}, 'Ú', "'U")
vim.keymap.set({'n', 'v'}, 'Ü', '"U')

-----------------------------------------------

-- This works, it update at every cursor hold
--[[
-- Lower case characters (Local buffer marks)
for ci = string.byte('a'), string.byte('z') do
  local c = string.char(ci)
  vim.fn.sign_define("mark_" .. c, {text=c})
end

-- Upper case characters (Global project marks)
for ci = string.byte('A'), string.byte('Z') do
  local c = string.char(ci)
  vim.fn.sign_define("mark_" .. c, {text=c})
end

local function should_show_mark(mark)
  local mi = string.byte(mark)
  return (string.byte('a') <= mi and mi <= string.byte('z'))
    or (string.byte('A') <= mi and mi <= string.byte('Z'))
end

local function set_mark_signs(bufnr)
  local marks
  if bufnr then marks = vim.fn.getmarklist(bufnr)
  else marks = vim.fn.getmarklist() end
  for _, m in ipairs(marks) do
    local mark_char = m.mark:sub(-1)
    if should_show_mark(mark_char) then
      -- vim.fn.sign_placelist
      vim.fn.sign_place(
        string.byte(mark_char),
        "MarkGroup",
        "mark_" .. mark_char,
        vim.fn.bufname(m.pos[1]),
        -- Most signs priority default to 10
        -- Info-10, Warning-11, Error-12
        { lnum = m.pos[2], priority = 13 }
      )
    end
  end
end

vim.api.nvim_create_autocmd("CursorHold", {
  -- callback = require('utilities').profile_func("Mark Signs", function()
  callback = function()
    local bufnr = vim.api.nvim_get_current_buf()
    -- vim.fn.sign_unplace("MarkGroup", { buffer=vim.fn.bufname(bufnr) }) -- Clear signs
    vim.fn.sign_unplace("MarkGroup") -- Clear signs
    set_mark_signs()
    set_mark_signs(bufnr)
  end,
})
]]--

-- Incrementable (tries to trigger on BufRead and new mark assignments,
-- but fails when deleting lines with set icons and then undo doesnt reverse them,
-- also its very hard to hook into the default mark related commands)
--[[
local signs = {}

-- Lower case characters (Local buffer marks)
for ci = string.byte('a'), string.byte('z') do
  local c = string.char(ci)
  signs[#signs+1] = c
  vim.fn.sign_define("mark_" .. c, {text=c})
end

-- Upper case characters (Global project marks)
for ci = string.byte('A'), string.byte('Z') do
  local c = string.char(ci)
  signs[#signs+1] = c
  vim.fn.sign_define("mark_" .. c, {text=c})
end

local function place_mark_sign(sign, bufnr)
  -- bufnr = bufnr or vim.api.nvim_get_current_buf()
  bufnr = bufnr or 0
  local mark_pos = vim.fn.getpos("'" .. sign)
  local mark_bufnr = mark_pos[1] -- 0 for non-global marks
  local mark_line = mark_pos[2]
  if mark_line > 0 then
    -- BUG: is_cursor_on_mark always true
    -- local cursor_pos = vim.fn.getpos(".")
    -- local is_cursor_on_mark = (cursor_pos[1] == mark_bufnr) and (cursor_pos[2] == mark_line)
    -- print(is_cursor_on_mark)
    -- if toggle and is_cursor_on_mark then
    --   vim.fn.sign_unplace("MarkGroup", {
    --     buffer=vim.fn.bufname(mark_bufnr),
    --     id=string.byte(sign)
    --   })
    -- else
      vim.fn.sign_place(
        string.byte(sign),
        "MarkGroup",
        "mark_" .. sign,
        vim.fn.bufname(mark_bufnr == 0 and bufnr or mark_bufnr),
        -- Default sign priority is 10
        { lnum = mark_line, priority = 11 }
      )
    -- end
  end
end

-- for _, s in ipairs(signs) do
--   vim.keymap.set('n', 'm' .. s, function ()
--     vim.cmd("normal! m" .. s)
--     place_mark_sign(s, nil, true)
--   end)
-- end

-- vim.api.nvim_create_autocmd("BufAdd", {
--   callback = function(args)
--     for _, s in ipairs(signs) do
--       place_mark_sign(s, args.buf)
--     end
--   end,
-- })

vim.api.nvim_create_autocmd("CursorHold", {
  callback = function()
    local bufnr = vim.api.nvim_get_current_buf()
    local bufname = vim.fn.bufname(bufnr)
    vim.fn.sign_unplace("MarkGroup", { buffer=bufname }) -- Clear signs
    for _, s in ipairs(signs) do place_mark_sign(s, bufnr) end
  end,
})
]]--

