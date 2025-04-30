-- Set borders for floating windows
vim.api.nvim_set_hl(0, 'FloatBorder', { fg = '#ffffff', bg = 'NONE' }) -- Set the border color

-- Configure Neovim's built-in LSP and other features to use the border function
--[[
vim.lsp.handlers['textDocument/hover'] = vim.lsp.with(vim.lsp.handlers.hover, {
  border = border('FloatBorder')
})
vim.lsp.handlers['textDocument/signatureHelp'] = vim.lsp.with(vim.lsp.handlers.signature_help, {
  border = border('FloatBorder')
})
]] --

-- BUG: After NVIM 0.11:
-- https://www.reddit.com/r/neovim/comments/1jmsl3j/switch_to_011_now_not_showing_borders_on/

-- NOTE: Since NVIM 0.11, these hover opts below are moved into the vim.lsp.buf.hover autocmd as arguments
-- Customize hover window appearance (Hover handler)
--[[vim.lsp.handlers["textDocument/hover"] = vim.lsp.with(vim.lsp.handlers.hover, {
  border = "rounded",
  --border = "none",  -- Disable border for floating windows
  focusable = false, -- Non-focusable floating windows
  -- https://www.reddit.com/r/neovim/comments/1182s32/is_it_possible_to_disable_no_information/
  silent = true,
})]]--

--[[vim.lsp.handlers["textDocument/hover"] = function(err, result, ctx, config)
  config = config or {}
  config.border = "rounded"
  config.focusable = false
  config.silent = true
  return vim.lsp.handlers.hover(err, result, ctx, config)
end]]--

-- Signature help handler (cmp-lsp-signature-helper)
--vim.lsp.handlers["textDocument/signatureHelp"] = vim.lsp.with(vim.lsp.handlers.signature_help, {
--  border = "rounded", -- You can change this to "none" if you don't want a border
--  focusable = false,  -- Ensure this is set to false
--})

-- window borders
--vim.o.fillchars = "vert:│,hor:─,horizup:┴,horizdown:┬,vertleft:┤,vertright:├"
--[[
vim.opt.fillchars = {
  vert = '│',
  horiz = '─',
  horizup = '┴',
  horizdown = '┬',
  vertleft = '┤',
  vertright = '├',
}
]] --

--[[
-- This sets the characters used for vertical and horizontal window separators
vim.opt.fillchars = {
  vert = '│',     -- Vertical separator between windows (default is |)
  horiz = '─',    -- Horizontal separator between windows (default is -)
  horizup = '┴',  -- Upward-facing junction between horiz and vert split
  horizdown = '┬',-- Downward-facing junction between horiz and vert split
  vertleft = '┤', -- Rightward-facing junction between vert and horiz split
  vertright = '├' -- Leftward-facing junction between vert and horiz split
}
]] --

---------------------------------------------------------------------------------------------


-- Function to create a custom border style
local function border(hl_name)
  return {
    { "╭", hl_name },
    { "─", hl_name },
    { "╮", hl_name },
    { "│", hl_name },
    { "╯", hl_name },
    { "─", hl_name },
    { "╰", hl_name },
    { "│", hl_name },
  }
end
