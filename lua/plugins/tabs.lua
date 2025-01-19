-- https://www.reddit.com/r/neovim/comments/13iy0p0/why_some_people_use_buffers_for_tabs_and_not_vim/
-- https://joshldavis.com/2014/04/05/vim-tab-madness-buffers-vs-tabs/

return {
  {
    'akinsho/bufferline.nvim',
    version = "*",
    dependencies = {
      'nvim-tree/nvim-web-devicons'
    },

    lazy = true,
    event = { "BufReadPre", "BufNewFile" }, -- Only triggers for actual file-related events

    opts = {
      options = {

        --sort_by = 'relative_directory', -- or 'tabs', use other sorting as preferred

        sort_by = function(buffer_a, buffer_b)
          -- Get the last written time of the buffers
          local function get_last_written(bufnr)
            local filename = vim.api.nvim_buf_get_name(bufnr)
            if vim.fn.filereadable(filename) == 1 then
              return vim.fn.getftime(filename)
            end
            return -1 -- Default for unsaved or nonexistent files
          end

          local time_a = get_last_written(buffer_a.id)
          local time_b = get_last_written(buffer_b.id)

          return time_a > time_b -- Sort descending by last written time
        end,

        always_show_bufferline = true,
        show_clone_icon = false,
        diagnostics = "nvim_lsp",
        --[[diagnostics_indicator = function(count, level, diagnostics_dict, context)
          local icon = level:match("error") and " " or " "
          return " " .. icon .. count
        end]]--
      },
    },

    config = function (_, opts)
      vim.opt.termguicolors = true
      require("bufferline").setup(opts)

      vim.api.nvim_set_keymap(
        'n',
        '<Tab>',
        ':BufferLineCycleNext<CR>',
        { noremap = true, silent = true, desc = 'Next buffer in the recent lineup' }
      )

      vim.api.nvim_set_keymap(
        'n',
        '<S-Tab>',
        ':BufferLineCyclePrev<CR>',
        { noremap = true, silent = true, desc = 'Previous buffer in the recent lineup' }
      )

    end,

  }
}



