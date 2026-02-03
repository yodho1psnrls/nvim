-- https://www.reddit.com/r/neovim/comments/yh8abd/what_is_this_sticky_header_thing/
-- https://youtu.be/8BGr_umDTco?si=SpUe9jENOXB-yE6D&t=195

-- https://www.reddit.com/r/neovim/comments/wlwdwu/sticky_scroll/
-- https://www.reddit.com/r/vscode/comments/v7tes6/would_love_to_see_this_sticky_function_header_as/
-- https://www.reddit.com/r/emacs/comments/p6scs3/topsyel_simple_sticky_header_showing_definition/

-- https://github.com/nvim-treesitter/nvim-treesitter-context
-- A sticky header plugin
-- Use the one that extends from tree-sitter
--
-- Predecessor - https://github.com/wellle/context.vim
--
-- But if you have some issues with it,
--  https://github.com/uga-rosa/sticky-header.nvim
--  is also an alternative

-- BUG:
-- When in insert mode and going up and down with the arrow keys
--  sometimes the line number at your current cursor position flickers
--  with the previous line number
-- I am not quite sure, but this may be from this plugin, because it
--  uses the line numbers for display at the top and before installing
--  this plugin, i had not seen such an issue

-- BUG: This plugin is responsible for number line flickering
-- on scrolling with J and K | <C-u> and <C-d> | MouseScroll

-- Instead of sticky headers and context windows,
-- just jump quickly to it with [[ and return with <C-o>
-- https://stackoverflow.com/questions/33699049/display-current-function-in-vim-status-line
-- Or use symbol/context jumper plugin like "nvim-treesitter/nvim-treesitter-textobjects"
--  and use [m or ]m to jump to previous or next method
-- Or just use a symbols outline plugin which can show you your current and near contexts/symbols
--  like "hedyhli/outline.nvim" or "simrat39/symbols-outline.nvim"

return {
  {
    "nvim-treesitter/nvim-treesitter-context",

    lazy = true,
    dependencies = { "nvim-treesitter/nvim-treesitter" }, -- Ensure treesitter is loaded as a dependency

    config = function()
      require('treesitter-context').setup {
        enable = true,          -- Enable this plugin

        --multiwindow = false,    -- Enable multiwindow support
        multiwindow = true,

        --max_lines = 0,          -- No limit on lines spanned
        max_lines = 1,  -- 3

        min_window_height = 0,  -- No limit on minimum window height
        line_numbers = true,    -- Try this to be false, may fix flickering
        multiline_threshold = 20, -- Maximum number of lines for a single context
        trim_scope = 'outer',   -- Which context lines to discard
        mode = 'cursor',        -- Line used to calculate context
        separator = nil,        -- No separator, you can also try —
        zindex = 20,            -- Z-index of the context window
        on_attach = nil,        -- Callback for disabling attachment
      }
    end,
  },

  -- {
  --   "SmiteshP/nvim-navic",
  --   lazy = true,
  --   event = { "LspAttach" },
  --
  --   opts = {
  --     -- icons = {
  --     --   File          = "󰈙 ",
  --     --   Module        = " ",
  --     --   Namespace     = "󰌗 ",
  --     --   Package       = " ",
  --     --   Class         = "󰌗 ",
  --     --   Method        = "󰆧 ",
  --     --   Property      = " ",
  --     --   Field         = " ",
  --     --   Constructor   = " ",
  --     --   Enum          = "󰕘",
  --     --   Interface     = "󰕘",
  --     --   Function      = "󰊕 ",
  --     --   Variable      = "󰆧 ",
  --     --   Constant      = "󰏿 ",
  --     --   String        = "󰀬 ",
  --     --   Number        = "󰎠 ",
  --     --   Boolean       = "◩ ",
  --     --   Array         = "󰅪 ",
  --     --   Object        = "󰅩 ",
  --     --   Key           = "󰌋 ",
  --     --   Null          = "󰟢 ",
  --     --   EnumMember    = " ",
  --     --   Struct        = "󰌗 ",
  --     --   Event         = " ",
  --     --   Operator      = "󰆕 ",
  --     --   TypeParameter = "󰊄 ",
  --     --   enabled       = true,
  --     -- },
  --     -- VSCode-like icons
  --     icons = {
  --       File = ' ',
  --       Module = ' ',
  --       Namespace = ' ',
  --       Package = ' ',
  --       Class = ' ',
  --       Method = ' ',
  --       Property = ' ',
  --       Field = ' ',
  --       Constructor = ' ',
  --       Enum = ' ',
  --       Interface = ' ',
  --       Function = ' ',
  --       Variable = ' ',
  --       Constant = ' ',
  --       String = ' ',
  --       Number = ' ',
  --       Boolean = ' ',
  --       Array = ' ',
  --       Object = ' ',
  --       Key = ' ',
  --       Null = ' ',
  --       EnumMember = ' ',
  --       Struct = ' ',
  --       Event = ' ',
  --       Operator = ' ',
  --       TypeParameter = ' '
  --     },
  --     lsp = {
  --       auto_attach = false,
  --       preference = nil,
  --     },
  --     highlight = true,
  --     separator = "  ", -- >▸>›/»
  --     depth_limit = 0,
  --     depth_limit_indicator = "..",
  --     safe_output = true,
  --     lazy_update_context = false,
  --     click = false,
  --     format_text = function(text)
  --       return text
  --     end,
  --   },
  --
  --   config = function (_, opts)
  --     vim.api.nvim_create_autocmd("ColorScheme", {
  --       callback = function ()
  --         -- vim.cmd("highlight! link NavicSeparator PmenuBorder")
  --         -- vim.cmd("highlight! NavicText guifg=#F0F0F0")
  --         -- vim.cmd("highlight! NavicText guifg=#D7D7D7")
  --         -- vim.api.nvim_set_hl(0, "NavicText", { link = "QuickFixLine" })
  --         vim.api.nvim_set_hl(0, "NavicText", { link = "SignColumn" })
  --         vim.cmd("highlight! link NavicSeparator NavicText")
  --         -- vim.cmd("highlight! link NavicSeparator WinSeparator")
  --         -- vim.api.nvim_set_hl(0, "NavicSeparator", { fg = "#61AFEF" })
  --       end
  --     })
  --     local navic = require("nvim-navic")
  --     navic.setup(opts)
  --     vim.api.nvim_create_autocmd("LspAttach", {
  --       callback = function (ev)
  --         local client = vim.lsp.get_client_by_id(ev.data.client_id)
  --         if client == nil then return end
  --         if client.server_capabilities.documentSymbolProvider then
  --           navic.attach(client, ev.buf)
  --         end
  --       end
  --     })
  --   end
  -- },
}
