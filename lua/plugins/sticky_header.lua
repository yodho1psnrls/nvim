-- https://github.com/nvim-treesitter/nvim-treesitter-context
-- A sticky header plugin
-- Use the one that extends from tree-sitter
--
-- Predecessor - https://github.com/wellle/context.vim
--
-- But if you have some issues with it,
--  https://github.com/uga-rosa/sticky-header.nvim
--  is also an alternative

return {

  "nvim-treesitter/nvim-treesitter-context",

--  lazy = false,

  dependencies = { "nvim-treesitter/nvim-treesitter" }, -- Ensure treesitter is loaded as a dependency
  config = function()
    require('treesitter-context').setup {
      enable = true,          -- Enable this plugin
      multiwindow = false,    -- Enable multiwindow support
      max_lines = 0,          -- No limit on lines spanned
      min_window_height = 0,  -- No limit on minimum window height
      line_numbers = true,
      multiline_threshold = 20, -- Maximum number of lines for a single context
      trim_scope = 'outer',   -- Which context lines to discard
      mode = 'cursor',        -- Line used to calculate context
      separator = nil,        -- No separator
      zindex = 20,            -- Z-index of the context window
      on_attach = nil,        -- Callback for disabling attachment
    }
  end,
}
