-- https://www.reddit.com/r/neovim/comments/ry9qxi/lsp_linesnvim_show_diagnostics_using_virtual/
-- https://www.reddit.com/r/neovim/comments/1if024i/theres_now_a_builtin_virtual_lines_diagnostic/

-- https://git.sr.ht/%7Ewhynothugo/lsp_lines.nvim
-- https://github.com/maan2003/lsp_lines.nvim

-- https://www.reddit.com/r/neovim/comments/1if024i/comment/mamvams/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
-- https://neovim.io/doc/user/diagnostic.html#vim.diagnostic.Opts.VirtualLines

-- Alternatives:
-- https://neovimcraft.com/plugin/rachartier/tiny-inline-diagnostic.nvim/

return {
  {
    "https://git.sr.ht/~whynothugo/lsp_lines.nvim",
    -- "maan2003/lsp_lines.nvim", -- This version seems snappier
    -- And it also doesnt prefix the diagnostic.code before the diagnostic.message
    -- So we dont have to change the source code and forbid it from updating
    -- But it also shows some weird error

    lazy = true,

    -- I changed lua/lsp_lines/render.lua lines 188 to 193
    --  which prefix the diagnostic message with diagnostic.code
    -- I hate that, so i made it such that
    --  local msg = diagnostic.message
    -- And because of that we cant update it,
    --  because it will override this change
    update = false,

    config = function ()
      local lsp_lines = require("lsp_lines")
      lsp_lines.setup()

      local is_enabled = true
      vim.keymap.set("n", "<leader>ll", function()
        lsp_lines.toggle()
        is_enabled = not is_enabled
      end, { desc = "Toggle lsp_lines" })

      -- local last_virtual_lines = vim.diagnostic.virtual_lines

      -- Autocommand to disable lsp_lines in insert mode
      --[[vim.api.nvim_create_autocmd({ "InsertEnter", "InsertLeave" }, {
        -- callback = lsp_lines.toggle
        callback = function ()
          if not is_enabled then return end
          local diag_conf = vim.diagnostic.config()
          if diag_conf then
            diag_conf.virtual_lines = not diag_conf.virtual_lines
            -- if diag_conf.virtual_lines then
            --   last_virtual_lines = diag_conf.virtual_lines
            --   diag_conf.virtual_lines = nil
            -- else
            --   diag_conf.virtual_lines = last_virtual_lines
            -- end
          end
          vim.diagnostic.config(diag_conf)
        end
      })]]--

      -- TODO: Disable it for Lazy window

    end,
  },
}
