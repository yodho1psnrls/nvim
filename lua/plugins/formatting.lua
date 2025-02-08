-- Autoformat

return {

  {
    'stevearc/conform.nvim',
    --event = { 'BufWritePre' },
    cmd = { 'ConformInfo' },

    -- vim.keymap.set("n", "<leader>f", vim.lsp.buf.format)

    keys = {
      {
        '<leader>f',
        function()
          require('conform').format { async = true, lsp_format = 'fallback' }
        end,
        mode = '',
        desc = '[F]ormat buffer',
      },
    },
    opts = {
      notify_on_error = false,
      format_on_save = function(bufnr)
        return false -- Disable formatting on save
        --[[
        -- Disable "format_on_save lsp_fallback" for languages that don't
        -- have a well standardized coding style. You can add additional
        -- languages here or re-enable it for the disabled ones.
        local disable_filetypes = { c = true, cpp = true }
        local lsp_format_opt
        if disable_filetypes[vim.bo[bufnr].filetype] then
          lsp_format_opt = 'never'
        else
          lsp_format_opt = 'fallback'
        end
        return {
          timeout_ms = 500,
          lsp_format = lsp_format_opt,
        }]] --
      end,
      formatters_by_ft = {
        lua = { 'stylua' },
        -- Conform can also run multiple formatters sequentially
        -- python = { "isort", "black" },
        --
        -- You can use 'stop_after_first' to run the first available formatter from the list
        -- javascript = { "prettierd", "prettier", stop_after_first = true },
      },
    },
  },

  {
    -- Extract Method/Function functionality !!!

    -- https://github.com/ThePrimeagen/refactoring.nvim
    "ThePrimeagen/refactoring.nvim",
    dependencies = {
      "nvim-lua/plenary.nvim",
      "nvim-treesitter/nvim-treesitter",
    },
    lazy = true,
    keys = {
      {'<leader>rr', desc = "Refactor" },
    },
    cmd = {
      'Refactor',
    },
    config = function()

      -- No matter which configuration option you use,
      --  you must first call the setup function.
      require("refactoring").setup()

      require('refactoring').setup({
        prompt_func_return_type = {
          go = false,
          java = false,

          cpp = false,
          c = false,
          h = false,
          hpp = false,
          cxx = false,
        },
        prompt_func_param_type = {
          go = false,
          java = false,

          cpp = false,
          c = false,
          h = false,
          hpp = false,
          cxx = false,
        },
        printf_statements = {},
        print_var_statements = {},
        show_success_message = false, -- shows a message with information about the refactor on success
        -- i.e. [Refactor] Inlined 3 variable occurrences
      })

      -- load refactoring Telescope extension
      require("telescope").load_extension("refactoring")

      vim.keymap.set(
        { "n", "x" },
        "<leader>rr",
        function() require('telescope').extensions.refactoring.refactors() end,
        {desc = "Refactor", silent = true, noremap = true}
      )

    end,
  },

}
