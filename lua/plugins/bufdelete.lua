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
    lazy = true,
    cmd = 'Bdelete',
    keys = {
      { '<leader>x', desc = "Buffer Delete" },
    },
    config = function()
      local bd = require("bufdelete")

      -- map('n', '<leader>x', '<cmd>Bdelete<CR>', { noremap = true, silent = true, desc = "Close buffer" })
      vim.keymap.set('n', '<leader>x', function ()
        -- map('n', '<C-w>', function () -- NOTE: In most apps Ctrl + W closes the tab
        -- See :help bufdelete
        if vim.bo.buftype == 'terminal' then
          bd.bufdelete(0, true) -- Force close of terminal buffers
        else
          bd.bufdelete(0, false)
        end
      end, { noremap = true, silent = true, desc = "Buffer delete" })

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
