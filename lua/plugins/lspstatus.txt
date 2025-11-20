-- https://github.com/nvim-lua/lsp-status.nvim
-- This is a Neovim plugin/library for generating
--  statusline components from the built-in LSP client.
-- So basically, it is a lsp to lualine integration
-- This is the plugin responsible for showing
-- the number of warnings and errors in lualine

return {
  "nvim-lua/lsp-status.nvim",
  lazy = true, -- lualine will lazy load it

  config = function()
    local lsp_status = require("lsp-status")

    lsp_status.register_progress() -- register default handlers

    --[[vim.api.nvim_create_autocmd("LspAttach", {
      callback = function(args)
        local client = vim.lsp.get_client_by_id(args.data.client_id)
        if client then
          lsp_status.on_attach(client)
        end
      end,
    })]]--

    lsp_status.config {
      select_symbol = function(cursor_pos, symbol)
        if symbol.valueRange then
          local value_range = {
            ["start"] = {
              character = 0,
              line = vim.fn.byte2line(symbol.valueRange[1])
            },
            ["end"] = {
              character = 0,
              line = vim.fn.byte2line(symbol.valueRange[2])
            }
          }

          return require("lsp-status.util").in_range(cursor_pos, value_range)
        end
      end
    }
  end,
}
