local M = {}

-- https://www.reddit.com/r/neovim/comments/jqbdpp/how_to_automatically_close_empty_buffers/
--[[
vim.api.nvim_create_user_command('ExitBuffer', function()
  -- Get the list of listed buffers
  local buffers = vim.fn.getbufinfo({ buflisted = 1 })

  if #buffers > 1 then
    -- More than one listed buffer, update and delete the current buffer
    vim.cmd('update | bdelete')
  else
    -- Only one listed buffer, update and quit
    vim.cmd('update | quit')
  end
end, { desc = 'Exit the current buffer based on listed buffers' })
]]


-- Function to close all empty buffers
function M.close_empty_buffers()
  -- Get a list of all buffers (including unlisted)
  local buffers = vim.fn.getbufinfo({})

  -- Iterate over the buffers
  for _, buf in ipairs(buffers) do
    -- Check if the buffer is listed, unmodified, and empty
    if vim.fn.buflisted(buf.bufnr) == 1 and  -- Check if the buffer is listed
       vim.api.nvim_buf_get_changedtick(buf.bufnr) == 1 and  -- Check if the buffer is unmodified
       vim.api.nvim_buf_line_count(buf.bufnr) == 0 then  -- Check if the buffer has no lines (empty buffer)
      -- Delete the buffer if it's empty and unmodified
      vim.api.nvim_buf_delete(buf.bufnr, { force = true })
    end
  end
end


-- Create a user command to call the function
-- vim.api.nvim_create_user_command('CloseEmptyBuffers', function() M.close_empty_buffers end, { desc = 'Close all empty buffers' })


return M
