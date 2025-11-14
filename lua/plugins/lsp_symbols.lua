-- Alternatives:
--  https://neovimcraft.com/plugin/Wansmer/symbol-usage.nvim/
--  NOTE: This is a plugin which shows a window which gives you
--  all lsp-based symbols in the current file like functions and classes

return {
  {
    "simrat39/symbols-outline.nvim",
    dependencies = {
      -- "neovim/nvim-lspconfig"
    },
    lazy = true,
    keys = {
      {"<leader>,", desc = "LSP Symbols Outline"},
    },
    cmd = {
      "SymbolsOutlineOpen",
      "SymbolsOutline",
    },
    opts = {
      highlight_hovered_item = true,
      show_guides = true,
      auto_preview = false, -- Focus the cursor on open/toggle
      position = 'right',
      relative_width = false, -- Default: true
      width = 32,
      auto_close = false, -- Default: false
      show_numbers = false,
      show_relative_numbers = false,
      show_symbol_details = true, -- Default: true (Shows additional symbol info)
      preview_bg_highlight = 'Pmenu',
      autofold_depth = nil,
      auto_unfold_hover = true,
      fold_markers = { '', '' },
      wrap = false,
      keymaps = { -- These keymaps can be a string or a table for multiple keys
        close = {"<Esc>", "q"},
        goto_location = "<Cr>",
        focus_location = "o",
        hover_symbol = "<C-space>",
        toggle_preview = "K",
        rename_symbol = "r",
        code_actions = "a",
        fold = "h",
        unfold = "l",
        fold_all = "W",
        unfold_all = "E",
        fold_reset = "R",
      },
      lsp_blacklist = {},
      symbol_blacklist = {},
      symbols = {
        File = { icon = "", hl = "@text.uri" },
        Module = { icon = "", hl = "@namespace" },
        Namespace = { icon = "", hl = "@namespace" },
        Package = { icon = "", hl = "@namespace" },
        Class = { icon = "𝓒", hl = "@type" },
        Method = { icon = "ƒ", hl = "@method" },
        Property = { icon = "", hl = "@method" },
        Field = { icon = "", hl = "@field" },
        Constructor = { icon = "", hl = "@constructor" },
        Enum = { icon = "ℰ", hl = "@type" },
        Interface = { icon = "ﰮ", hl = "@type" },
        Function = { icon = "", hl = "@function" },
        Variable = { icon = "", hl = "@constant" },
        Constant = { icon = "", hl = "@constant" },
        String = { icon = "𝓐", hl = "@string" },
        Number = { icon = "#", hl = "@number" },
        Boolean = { icon = "⊨", hl = "@boolean" },
        Array = { icon = "", hl = "@constant" },
        Object = { icon = "⦿", hl = "@type" },
        Key = { icon = "🔐", hl = "@type" },
        Null = { icon = "NULL", hl = "@type" },
        EnumMember = { icon = "", hl = "@field" },
        Struct = { icon = "𝓢", hl = "@type" },
        Event = { icon = "🗲", hl = "@type" },
        Operator = { icon = "+", hl = "@operator" },
        TypeParameter = { icon = "𝙏", hl = "@parameter" },
        Component = { icon = "", hl = "@function" },
        Fragment = { icon = "", hl = "@constant" },
      },
    },
    config = function (_, opts)
      require("symbols-outline").setup(opts)
      -- local util = require("utilities")
      -- vim.keymap.set("n", "<leader>,", "<cmd>SymbolsOutline<CR>",
      vim.keymap.set("n", "<leader>,", function ()
        vim.cmd("SymbolsOutline")
        -- if util.is_loaded("nvim-tree") then
        --   vim.cmd("NvimTreeClose")
        -- end
      end,
        { desc = "LSP Symbols Outline", noremap = true, silent = true })
    end
  }
}
