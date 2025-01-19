-- If you want to use non-lsp static analyzers like ikos, then you
--  should use something like nullls or nonels

return {

  -- https://github.com/nvimtools/none-ls.nvim
  --[[{ "nvimtools/none-ls.nvim", config = function()
      local none_ls = require("none-ls")
      none_ls.setup({
        sources = {
          none_ls.builtins.diagnostics.from_command({
            -- Configure IKOS command
            command = "ikos",
            args = {
              "analyze",
              "--output", "-",
              vim.fn.getcwd(),
            },
            format = "line", -- Change format as per IKOS output
            to_stdin = false, -- Ensure command isn't treated as stdin
            from_stderr = false, -- Capture only stdout
          }),
        },
      })
    end,
  },]]--


  --{ "jose-elias-alvarez/null-ls.nvim" },

  --[[
  setup = function()
    local null_ls = require("null-ls")

    null_ls.setup({
      sources = {
        null_ls.builtins.formatting.stylua,
        null_ls.builtins.diagnostics.eslint,
        null_ls.builtins.completion.spell,
      },
    })
  end,
  ]] --

}
