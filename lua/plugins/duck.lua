--[[
return {
  'tamton-aquib/duck.nvim',
  config = function()
    vim.keymap.set('n', '<leader>dd', function() require("duck").hatch() end, {desc = "Duck Hatch"})
    vim.keymap.set('n', '<leader>dk', function() require("duck").cook() end, {desc = "Duck Cook"})
    vim.keymap.set('n', '<leader>da', function() require("duck").cook_all() end, {desc = "Cook All Ducks"})
  end
}
]]--

-- Lazy Load Version
return {
  'tamton-aquib/duck.nvim',

  lazy = true,

  keys = { -- The key mappings that will trigger the loading
    { '<leader>dh', desc = "Duck Hatch" },
    { '<leader>dc', desc = "Duck Cook" },
    { '<leader>dd', desc = "Cook All Ducks" },
  },
  config = function()
    vim.keymap.set('n', '<leader>dh', function() require("duck").hatch() end, {desc = "Duck Hatch"})
    vim.keymap.set('n', '<leader>dc', function() require("duck").cook() end, {desc = "Duck Cook"})
    vim.keymap.set('n', '<leader>dd', function() require("duck").cook_all() end, {desc = "Cook All Ducks"})
  end,
}

