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
    require('nvim-autopairs').setup {}
    -- If you want to automatically add `(` after selecting a function or method
    local cmp_autopairs = require 'nvim-autopairs.completion.cmp'
    local cmp = require 'cmp'
    cmp.event:on('confirm_done', cmp_autopairs.on_confirm_done())
  end,
}
