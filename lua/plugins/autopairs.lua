-- https://www.reddit.com/r/neovim/comments/1ogdu9m/how_to_jump_out_of_quotes_or_brackets_after/
-- this plugin automatically types closing brackets when you start
-- opening a new bracket

-- NOTE: Alternative
-- https://www.reddit.com/r/neovim/comments/167xgag/ultimateautopairnvim_nextgen_autopairing_plugin/
-- https://github.com/altermo/ultimate-autopair.nvim

-- BUG: Native nvim lsp signature help autocmd makes autopairs to fail with ()

-- https://github.com/windwp/nvim-autopairs
return {
  'windwp/nvim-autopairs',
  event = 'InsertEnter',
  -- dependencies = { 'hrsh7th/nvim-cmp' }, -- Optional dependency
  config = function()
    require('nvim-autopairs').setup {
      fast_wrap = {},                              -- Enables wrapping functionality
      disable_filetype = { "TelescopePrompt", "vim" }, -- Avoid specific filetypes
      check_ts = true,                             -- Use Treesitter for intelligent pairing
      map_cr = true, -- add option map_cr (Without completion plugin)
      enable_check_bracket_line = true,  -- Don't add pairs if it already has a close pair in the same line
      -- ignored_next_char = "[%w%.]",  -- will ignore alphanumeric and `.` symbol | ignore when next char is a word or dot

      -- Defaults of fast_wrap (The above fast_wrap = {} uses the defaults)
      -- fast_wrap = {
      --   map = '<M-e>',
      --   chars = { '{', '[', '(', '"', "'" },
      --   pattern = [=[[%'%"%>%]%)%}%,]]=],
      --   end_key = '$',
      --   before_key = 'h',
      --   after_key = 'l',
      --   cursor_pos_before = true,
      --   keys = 'qwertyuiopzxcvbnmasdfghjkl',
      --   manual_position = true,
      --   highlight = 'Search',
      --   highlight_grey='Comment'
      -- },
    }

    -- NOTE: If you want to automatically add `(` after selecting a function or method

    -- local cmp_autopairs = require 'nvim-autopairs.completion.cmp'
    -- local cmp = require 'cmp'
    -- cmp.event:on('confirm_done', cmp_autopairs.on_confirm_done())

    -- vim.lsp.handlers["textDocument/completionItem/resolve"] = function(err, item, ctx, config)
    --   if not item then return end
    --   if item.kind == vim.lsp.protocol.CompletionItemKind.Function then
    --     print("You selected a function!")
    --   end
    --   return vim.lsp.handlers.completionItemResolve(err, item, ctx, config)
    -- end

    -- A Quick fix for the auto pairs, on windows default behaiviour
    vim.keymap.set({'i', 't'}, "''", "''<Left>", {noremap = true})
    vim.keymap.set({'i', 't'}, '""', '""<Left>', {noremap = true})
    vim.keymap.set({'c'}, "''", "''<Left>", {noremap = false})
    vim.keymap.set({'c'}, '""', '""<Left>', {noremap = false})

  end,
}
