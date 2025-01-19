-- A plugin that deletes buffers, without affecting your window layout

-- Delete a buffer (works in NVChad)
--map("n", "<leader>x", function()
--  require("nvchad.tabufline").close_buffer()
--end, { desc = "buffer close" })

-- Delete a buffer (works directly out of neovim)
--map('n', '<leader>x', ':bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })
-- Delete a buffer (with delbuffer plugin, which doesnt affect your window layout)

-- https://www.reddit.com/r/neovim/comments/uv7cuf/bufdeletenvim_new_feature_bdeletepre_and/

return {

  {
    "famiu/bufdelete.nvim",
    config = function()

      --vim.api.nvim_set_keymap('n', '<leader>x', ':Bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })

      --vim.api.nvim_set_keymap('n', '<leader>x',
      --  ':lua require("bufdelete").bufdelete(buf, false)<CR>',
      --{ noremap = true, silent = true, desc = "Close buffer" })

      -- Completely remap  :bd to :Bdelete
      --vim.api.nvim_create_user_command('bd', 'Bdelete', { nargs = 0 })
      --vim.api.nvim_set_keymap('n', '<leader>x', ':bd<CR>', { noremap = true, silent = true, desc = "Close buffer" })

      -- TODO: Make the terminal buffer exit to use this plugin

      -- TODO: Fix it, it doesnt quite work
      -- Test case: open nvim-tree and a cpp file, then run the cpp exe (which opens the terminal in place of the cpp file window)
      -- Then when the exe finished execution, press enter and the terminal closes, but instead of opening the cpp file back in its place
      -- The whole screen is taken up by nvim-tree

      -- AutoCMD that makes the terminals to close using this plugin, so the windows layout is preserved
    --  vim.api.nvim_create_autocmd("TermClose", {
    --    pattern = "*",
    --    callback = function()
    --      -- Close the buffer when the terminal closes
    --      vim.cmd("silent! call nvim_bufdel(bufnr('%'))")
    --    end,
    --  })
    end
  }

}
