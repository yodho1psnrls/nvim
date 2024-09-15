-- https://github.com/L3MON4D3/LuaSnip

return {

  {
    "L3MON4D3/LuaSnip",
    -- follow latest release.
    version = "v2.*", -- Replace <CurrentMajor> by the latest released major (first number of latest release)
    -- install jsregexp (optional!).
    --build = "make install_jsregexp"

    config = function()
      local luasnip = require('luasnip')

      luasnip.setup({
        -- Example of custom configuration, if applicable
        -- You might need to adjust this based on your setup
        region_check_events = "InsertEnter",
        delete_check_events = "InsertLeave",
        -- Other configuration options
      })
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

