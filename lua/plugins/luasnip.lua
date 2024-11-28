-- https://github.com/L3MON4D3/LuaSnip
-- https://github.com/hrsh7th/nvim-cmp/wiki/Example-mappings#luasnip
-- https://github.com/saadparwaiz1/cmp_luasnip

-- This is the thing that will allow you multiline snippets
--  (that will be displayed and completed using nvim-cmp)
-- https://www.reddit.com/r/neovim/comments/120nfhn/multiline_completion_with_nvimcmp/
-- https://github.com/rafamadriz/friendly-snippets

-- NOTE:
-- So as it turns out, this plugin indeed works, but it is only
--  a snippet engine, so you should manually define snippets
--  Thats why there are predefined snippet libraries that you can use 
--  (friendly-snippets is one such library)

-- LuaSnip alone will work by expanding the snippets, triggered by some
--  sequence of words, but if you use nvim-cmp, you can trigger them by
--  the suggestions from nvim-cmp, but for that to work, you need
--  nvim-cmp-luasnip as a dependency

return {

  {
    "L3MON4D3/LuaSnip",
    -- follow latest release.
    version = "v2.*", -- Replace <CurrentMajor> by the latest released major (first number of latest release)
    -- install jsregexp (optional!).
    build = "ninja install_jsregexp",

    lazy = true,

    -- Install the snippet library
    dependencies = { "rafamadriz/friendly-snippets" },

    config = function()
      local ls = require('luasnip')

      ls.setup({
        -- Example of custom configuration, if applicable
        -- You might need to adjust this based on your setup
        region_check_events = "InsertEnter",
        delete_check_events = "InsertLeave",
        -- Other configuration options
      })

      require("luasnip.loaders.from_vscode").lazy_load()

      -- Example Custom Snippet
      --[[ls.add_snippets("cpp", {
        ls.snippet("main", {
          ls.text_node({
            "#include <iostream>",
            "",
            "int main() {",
            "    std::cout << \"Hello, World!\" << std::endl;",
            "    return 0;",
            "}",
          }),
        }),
      })]]--

    end,



    --[[
	  config = function()

    local ls = require("luasnip")

    vim.keymap.set({"i"}, "<C-K>", function() ls.expand() end, {silent = true})
    vim.keymap.set({"i", "s"}, "<C-L>", function() ls.jump( 1) end, {silent = true})
    vim.keymap.set({"i", "s"}, "<C-J>", function() ls.jump(-1) end, {silent = true})

    vim.keymap.set({"i", "s"}, "<C-E>", function()
	    if ls.choice_active() then
		    ls.change_choice(1)
	    end
    end, {silent = true})

	end,
  ]] --

  }

}

