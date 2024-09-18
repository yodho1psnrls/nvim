-- Do NOT use this plugin if you use cmp-snip-signature-helper plugin
-- Because they do the exact same thing and may interfere

-- https://github.com/ray-x/lsp_signature.nvim
return {

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
