-- Do NOT use this plugin if you use cmp-snip-signature-helper plugin
-- Because they do the exact same thing and may interfere

-- See completion.lua line 160, if you want it to integrate with nvim-cmp

-- https://github.com/ray-x/lsp_signature.nvim
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
    event = "InsertEnter",
    opts = {
      bind = true,
      handler_opts = {
        border = "rounded"
      }
    },
    config = function(_, opts) require 'lsp_signature'.setup(opts) end
  }

}
