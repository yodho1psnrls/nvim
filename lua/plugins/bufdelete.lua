-- A plugin that deletes buffers, without affecting your window layout

-- Delete a buffer (works in NVChad)
--map("n", "<leader>x", function()
--  require("nvchad.tabufline").close_buffer()
--end, { desc = "buffer close" })

-- Delete a buffer (works directly out of neovim)
--map('n', '<leader>x', ':bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })
-- Delete a buffer (with delbuffer plugin, which doesnt affect your window layout)

return {

  {
    "famiu/bufdelete.nvim",
    config = function()
      vim.api.nvim_set_keymap('n', '<leader>x', ':Bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })
    end
  }

}
