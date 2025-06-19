return {

--[[  
{ -- Add indentation guides even on blank lines
    'lukas-reineke/indent-blankline.nvim',
    -- Enable `lukas-reineke/indent-blankline.nvim`
    -- See `:help ibl`
    main = 'ibl',
    opts = {},
  },
]]--

--[[{
  "lukas-reineke/indent-blankline.nvim",
  config = function()
    -- Version 3 uses "ibl" as the main module
    require("ibl").setup {
      -- You can customize the character for indent lines
      -- NOTE: :help ibl.config.indent for alternatives
      indent = {
        -- char = "│",  -- Customize the character here
        char = {"╎", "╏", "┆", "┇", "┊", "┋"},
        --char = "", -- empty character will disable the indent character
      },
      -- Show the current context of indentation
      scope = {
        show_start = false,  -- Disable showing scope start line
        show_end = false,    -- Disable showing scope end line
      },
    }
  end
}]]--


  -- NOTE: In options.lua set o.expandtab = true !!!
  -- for files, that had previously used unexpanded tabs
  -- use :retab
  -- if you load a previous session, the files will use the previous
  -- expandtab setting, so you would need to remove them from buffers and open again

{
  "lukas-reineke/indent-blankline.nvim",
  lazy = true,
  main = "ibl",
  ---@module "ibl"
  ---@type ibl.config
  opts = {
    -- You can customize the character for indent lines
    -- NOTE: :help ibl.config.indent for alternatives
    indent = {
      -- char = "│",  -- Customize the character here
      char = {"╎", "╏", "┆", "┇", "┊", "┋"},
      --char = "", -- empty character will disable the indent character
    },
    -- Show the current context of indentation
    scope = {
      show_start = false,  -- Disable showing scope start line
      show_end = false,    -- Disable showing scope end line
    },
    whitespace = {
      remove_blankline_trail = true,
    },
    exclude = {
      filetypes = { "dashboard", "undotree" },
    },
  },
  config = function (_, opts)
    require("ibl").setup(opts)
  end
}


}
