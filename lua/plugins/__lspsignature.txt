-- Do NOT use this plugin if you use cmp-snip-signature-helper plugin
-- Because they do the exact same thing and may interfere

-- See completion.lua line 160, if you want it to integrate with nvim-cmp

-- https://github.com/ray-x/lsp_signature.nvim

-- Alternative plugin
-- https://github.com/Issafalcon/lsp-overloads.nvim
--  It is build on top of the neovim build-in lsp signature helper handler
--   and adds additional functionality


return {

  --https://github.com/Issafalcon/lsp-overloads.nvim
  -- This is an extension of nvim-lsp-signature-help, to enchance overload visibility
  -- But since you use lsp_signature, you cant use it
  --{ "Issafalcon/lsp-overloads.nvim" },

    -- Comment it out, because i will use lsp-signature
    -- This was before in completion.lua (may be nvim-cmp related)
    --{
    --  "hrsh7th/cmp-nvim-lsp-signature-help",
    --  event = "LspAttach", -- Load when LSP server attaches
    --},

  {
    "ray-x/lsp_signature.nvim",

    --event = "InsertEnter",
    event = "LspAttach", -- Load when LSP server attaches

    opts = {
      bind = true,
      handler_opts = {
        border = "rounded"
      },

--      floating_window = true, -- Default: true
--      hint_enable = true, -- Virtual Hint, Default: true
      hint_prefix = {
        above = "↙ ",  -- when the hint is on the line above the current line
        current = "← ",  -- when the hint is on the same line
        --current = "↖ ",
        below = "↖ "  -- when the hint is on the line below the current line
      },

--      always_trigger = true, -- Default: false
--      fix_pos = true, -- Default: false -- set to true, the floating window will not auto-close until finish all parameters
--      select_signature_key = '<S-Tab>', -- cycle to next signature, e.g. '<M-n>' function overloading (Default: nil)

    },

    --config = function(_, opts) require 'lsp_signature'.setup(opts) end
    config = function(_, opts) require 'lsp_signature'.on_attach(opts) end -- Load when LSP server attaches
  }

}
