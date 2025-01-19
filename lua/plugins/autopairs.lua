-- this plugin automatically types closing brackets when you start
-- opening a new bracket

-- NOTE: Alternative
-- https://www.reddit.com/r/neovim/comments/167xgag/ultimateautopairnvim_nextgen_autopairing_plugin/
-- https://github.com/altermo/ultimate-autopair.nvim
  
-- https://github.com/windwp/nvim-autopairs
return {
  'windwp/nvim-autopairs',
  event = 'InsertEnter',
  -- Optional dependency
  dependencies = { 'hrsh7th/nvim-cmp' },
  config = function()
    require('nvim-autopairs').setup {
      fast_wrap = {},                              -- Enables wrapping functionality
      disable_filetype = { "TelescopePrompt", "vim" }, -- Avoid specific filetypes
      check_ts = true,                             -- Use Treesitter for intelligent pairing
    }
    -- If you want to automatically add `(` after selecting a function or method
    local cmp_autopairs = require 'nvim-autopairs.completion.cmp'
    local cmp = require 'cmp'
    cmp.event:on('confirm_done', cmp_autopairs.on_confirm_done())

    -- A Quick fix for the auto pairs, on windows default behaiviour
    vim.keymap.set({'i', 't'}, "''", "''<Left>", {noremap = true})
    vim.keymap.set({'i', 't'}, '""', '""<Left>', {noremap = true})
    vim.keymap.set({'c'}, "''", "''<Left>", {noremap = false})
    vim.keymap.set({'c'}, '""', '""<Left>', {noremap = false})

  end,
}
