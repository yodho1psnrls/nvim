-- https://github.com/nvim-lualine/lualine.nvim
-- https://medium.com/@shaikzahid0713/lualine-for-neovim-776b79861699

return {
  {
    "nvim-lualine/lualine.nvim",
    dependencies = {
      "nvim-tree/nvim-web-devicons", -- Icons support
      "archibate/lualine-time",      -- Clock extension
    },
    config = function()

      -- Ensure the time plugin is properly loaded
      require('lualine').setup {
        options = {
          icons_enabled = true,
          theme = 'auto',  -- You can set this to 'rose-pine' or any theme you prefer
          --component_separators = { left = '', right = ''},
          component_separators = { left = '|', right = '|'},
          section_separators = { left = '', right = ''},
          disabled_filetypes = {
            statusline = {},
            winbar = {},
          },
          always_divide_middle = true,
          globalstatus = false,
        },
        sections = {
          lualine_a = {'mode'},
          lualine_b = {'branch', 'diff', 'diagnostics'},
          lualine_c = {'filename'},
          -- Make sure `cdate` and `ctime` are available from lualine-time
          --lualine_x = {'encoding', 'fileformat', 'filetype', 'cdate', 'ctime'}, 
          --lualine_x = {'cdate', 'ctime' },
          lualine_x = {'ctime'},
          lualine_y = {'progress'},
          lualine_z = {'location'}
        },
        inactive_sections = {
          lualine_a = {},
          lualine_b = {},
          lualine_c = {'filename'},
          lualine_x = {'location'},
          lualine_y = {},
          lualine_z = {}
        },
        tabline = {},
        winbar = {},
        inactive_winbar = {},
        extensions = {}
      }

    end,
  }
}