-- https://github.com/nvim-tree/nvim-tree.lua
-- https://medium.com/@shaikzahid0713/file-explorer-for-neovim-c324d2c53657

return {

  { "nvim-tree/nvim-web-devicons" }, -- it is a dependancy for other plugins

  {
    "nvim-tree/nvim-tree.lua",

    --lazy = true,
    -- Lazy-load when one of these commands is used
    -- cmd = { 'NvimTreeToggle', 'NvimTreeFocus', 'NvimTreeFindFile' },

    opts = {

      sort = {
        sorter = "case_sensitive",
      },
      view = {
        width = 24,
      },
      renderer = {
        group_empty = true,
      },
      filters = {
        dotfiles = true,
        custom = { "compile_commands.json" },
      },

    },

  }

}
