-- https://www.reddit.com/r/neovim/comments/191f5o0/what_are_the_most_popular_colorschemes_for_neovim/
-- https://dotfyle.com/neovim/colorscheme/top
-- https://www.reddit.com/r/neovim/comments/1d0axyn/is_there_any_way_to_make_the_default_themes_more/

-- https://www.lazyvim.org/plugins/colorscheme

-- See nvim/lua/config/autocmds for some hardcoded highlight groups
--  if you see weird non-consistent with the theme colors !!!

return {

  -- the colorscheme should be available when starting Neovim

  --[[{
    "folke/tokyonight.nvim",
    lazy = false,    -- make sure we load this during startup if it is your main colorscheme
    priority = 1000, -- make sure to load this before all the other start plugins
    --    opts = { style = "moon" },
    config = function()
      -- load the colorscheme here
      vim.cmd([[colorscheme tokyonight]]--[[)
    end,
  },]]--

  --[[
  {
    "catppuccin/nvim",
    lazy = false,
    name = "catppuccin",
    priority = 1000,
    opts = {
      integrations = {
        aerial = true,
        alpha = true,
        cmp = true,
        dashboard = true,
        flash = true,
        grug_far = true,
        gitsigns = true,
        headlines = true,
        illuminate = true,
        indent_blankline = { enabled = true },
        leap = true,
        lsp_trouble = true,
        mason = true,
        markdown = true,
        mini = true,
        native_lsp = {
          enabled = true,
          underlines = {
            errors = { "undercurl" },
            hints = { "undercurl" },
            warnings = { "undercurl" },
            information = { "undercurl" },
          },
        },
        navic = { enabled = true, custom_bg = "lualine" },
        neotest = true,
        neotree = true,
        noice = true,
        notify = true,
        semantic_tokens = true,
        telescope = true,
        treesitter = true,
        treesitter_context = true,
        which_key = true,
      },
    },
    --[[
    specs = {
      {
        "akinsho/bufferline.nvim",
        optional = true,
        opts = function(_, opts)
          if (vim.g.colors_name or ""):find("catppuccin") then
            opts.highlights = require("catppuccin.groups.integrations.bufferline").get()
          end
        end,
      },
    },
    ]]--
    --[[
    -- Load the colorscheme
    config = function(_, opts)
      require'catppuccin'.setup(opts)
      vim.cmd("colorscheme catppuccin")
    end,
  },
  ]]--

  { -- https://github.com/rose-pine/neovim
    "rose-pine/neovim",
    name = "rose-pine",
    opts = {

      variant = "auto",      -- auto, main, moon, or dawn
      dark_variant = "main", -- main, moon, or dawn
      dim_inactive_windows = true, -- Default: false
      extend_background_behind_borders = true,

      enable = {
        terminal = true,
        legacy_highlights = true, -- Improve compatibility for previous versions of Neovim
        migrations = true,        -- Handle deprecated options automatically
      },

      styles = {
        bold = true,
        italic = false,
        transparency = false,
      },

      groups = {
        border = "muted",
        --border = "rose",
        link = "iris",
        panel = "surface",

        error = "love",
        hint = "iris",
        info = "foam",
        note = "pine",
        todo = "rose",
        warn = "gold",

        git_add = "foam",
        git_change = "rose",
        git_delete = "love",
        git_dirty = "rose",
        git_ignore = "muted",
        git_merge = "iris",
        git_rename = "pine",
        git_stage = "iris",
        git_text = "rose",
        git_untracked = "subtle",

        h1 = "iris",
        h2 = "foam",
        h3 = "rose",
        h4 = "gold",
        h5 = "pine",
        h6 = "foam",
      },

      palette = {
        -- Override the builtin palette per variant
        -- moon = {
        --     base = '#18191a',
        --     overlay = '#363738',
        -- },
      },


      highlight_groups = {
        -- Comment = { fg = "foam" },
        -- VertSplit = { fg = "muted", bg = "muted" },
      },

      before_highlight = function(group, highlight, palette)
        -- Disable all undercurls
        -- if highlight.undercurl then
        --     highlight.undercurl = false
        -- end
        --
        -- Change palette colour
        -- if highlight.fg == palette.pine then
        --     highlight.fg = palette.foam
        -- end
      end,

    },

    config = function(_, opts)
      require('rose-pine').setup(opts)
      --vim.cmd("colorscheme rose-pine")
      --vim.cmd("colorscheme rose-pine-main")
     --  vim.cmd("colorscheme rose-pine-moon")
      --vim.cmd("colorscheme rose-pine-dawn")
    end,
  },


  --[[
{ -- You can easily change to a different colorscheme.
    -- Change the name of the colorscheme plugin below, and then
    -- change the command in the config to whatever the name of that colorscheme is.
    --
    -- If you want to see what colorschemes are already installed, you can use `:Telescope colorscheme`.
    'folke/tokyonight.nvim',
    priority = 1000, -- Make sure to load this before all the other start plugins.
    init = function()
      -- Load the colorscheme here.
      -- Like many other themes, this one has different styles, and you could load
      -- any other, such as 'tokyonight-storm', 'tokyonight-moon', or 'tokyonight-day'.
      vim.cmd.colorscheme 'tokyonight-night'

      -- You can configure highlights by doing something like:
      vim.cmd.hi 'Comment gui=none'
    end,
},
]] --


}
