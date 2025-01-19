
-- Make buffers sorted, basen on which one was previously visited

--[[vim.api.nvim_create_autocmd("BufEnter", {
  callback = function()
    local bufnr = vim.fn.bufnr()
    -- Ensure the buffer is valid and listed
    if vim.api.nvim_buf_is_valid(bufnr) and vim.bo[bufnr].buflisted then
      vim.cmd("silent! buffer " .. bufnr)
    end
  end,
})]]--


--[[
local buffer_history = {}

-- Load the buffer history from the session when Neovim starts
local function load_buffer_history()
  local session = vim.fn['auto_session#get_session']
  if session and session().options then
    local history = session().options.buffer_history
    if history then
      buffer_history = history
    end
  end
end

-- Save the buffer history when exiting
local function save_buffer_history()
  local session = vim.fn['auto_session#get_session']
  if session then
    -- Save buffer_history to session options
    session().options.buffer_history = buffer_history
  end
end

-- Hook into AutoSession to load the buffer history when the session is loaded
vim.api.nvim_create_autocmd("AutoSessionLoadPost", {
  callback = load_buffer_history
})

-- Hook into AutoSession to save the buffer history when the session is saved
vim.api.nvim_create_autocmd("AutoSessionSavePre", {
  callback = save_buffer_history
})


-- Autocommand to track buffers when switching/entering them
vim.api.nvim_create_autocmd("BufEnter", {
  callback = function()
    local bufnr = vim.fn.bufnr()

    -- Avoid adding unlisted buffers like scratch buffers
    if vim.api.nvim_buf_is_valid(bufnr) and vim.bo[bufnr].buflisted then
      -- If the buffer is already in history, remove it to avoid duplication
      for i, history_bufnr in ipairs(buffer_history) do
        if history_bufnr == bufnr then
          table.remove(buffer_history, i)
          break
        end
      end

      -- Insert this buffer as most recently accessed
      table.insert(buffer_history, 1, bufnr)
    end
  end,
})

-- Autocommand to remove closed buffers from the history
vim.api.nvim_create_autocmd({"BufWinLeave", "BufDelete"}, {
  callback = function()
    local bufnr = vim.fn.bufnr()

    -- Remove the closed buffer from history if it exists
    for i, history_bufnr in ipairs(buffer_history) do
      if history_bufnr == bufnr then
        table.remove(buffer_history, i)
        break
      end
    end
  end,
})


-- Function to cycle to the most recent buffer
vim.keymap.set("n", "<leader>bn", function()
  local bufnr = buffer_history[1]
  if bufnr then
    vim.cmd("buffer " .. bufnr)
  end
end)

-- Function to cycle to the next buffer in history
vim.keymap.set("n", "<leader>bp", function()
  -- Move to next recently accessed buffer
  table.insert(buffer_history, 1, table.remove(buffer_history))
  local bufnr = buffer_history[1]
  if bufnr then
    vim.cmd("buffer " .. bufnr)
  end
end)
]]--




local map = vim.keymap.set
local telescope = require('telescope.builtin')

-- Function to get the list of buffers sorted by most recently used (MRU) order
local function get_sorted_buffers()
  local result = {}
  local bufs = vim.fn.getbufinfo({ buflisted = 1 })

  -- We want to sort the buffers by lastused
  table.sort(bufs, function(a, b)
    return a.lastused > b.lastused
    -- return a.changedtick > b.changedtick
  end)

  -- Collect the sorted buffer numbers
  for _, buf in ipairs(bufs) do
    table.insert(result, buf.bufnr)
  end

  return result
end

-- Go to the next buffer in the Telescope-sorted list of buffers
map('n', '<S-Tab>', function()
  local sorted_buffers = get_sorted_buffers()
  local current_bufnr = vim.api.nvim_get_current_buf()

  -- Find the current buffer index in the sorted buffer list
  local current_index = -1
  for i, bufnr in ipairs(sorted_buffers) do
    if bufnr == current_bufnr then
      current_index = i
      break
    end
  end

  -- Go to the next buffer (loop around if necessary)
  local next_index = current_index + 1
  if next_index > #sorted_buffers then
    next_index = 1
  end
  vim.cmd('buffer ' .. sorted_buffers[next_index])
end, { noremap = true, silent = true, desc = "Go to next buffer" })

-- Go to the previous buffer in the Telescope-sorted list of buffers
map('n', '<Tab>', function()
  local sorted_buffers = get_sorted_buffers()
  local current_bufnr = vim.api.nvim_get_current_buf()

  -- Find the current buffer index in the sorted buffer list
  local current_index = -1
  for i, bufnr in ipairs(sorted_buffers) do
    if bufnr == current_bufnr then
      current_index = i
      break
    end
  end

  -- Go to the previous buffer (loop around if necessary)
  local prev_index = current_index - 1
  if prev_index < 1 then
    prev_index = #sorted_buffers
  end
  vim.cmd('buffer ' .. sorted_buffers[prev_index])
end, { noremap = true, silent = true, desc = "Go to previous buffer" })



--[[
-- Move the newly created buffer after the current buffer in the buffer list
local function shift_buffers_on_new()
  local current_buf = vim.fn.bufnr('%')  -- Get the current buffer number
  local buf_list = vim.fn.getbufinfo({buflisted = 1})  -- List all listed buffers

  -- Add the current buffer to the list to keep track of its order
  local buf_numbers = {}
  for _, buf in ipairs(buf_list) do
    table.insert(buf_numbers, buf.bufnr)
  end

  -- Identify the position where the new buffer should appear (after current buffer)
  local current_buf_idx = vim.fn.index(buf_numbers, current_buf)

  -- If it's not the last buffer in the list, the next buffer would have its ID bumped
  if current_buf_idx + 1 <= #buf_numbers then
    -- Loop to set buffers in next positions +1 index
    for i = current_buf_idx + 1, #buf_numbers do
      vim.api.nvim_buf_set_name(buf_numbers[i], vim.api.nvim_buf_get_name(buf_numbers[i]))
    end
  end
end

-- Setup autocmd to trigger when a new file is opened or read
vim.api.nvim_create_augroup('BufferShift', { clear = true })
vim.api.nvim_create_autocmd({'BufNewFile', 'BufRead'}, {
  group = 'BufferShift',
  callback = shift_buffers_on_new,
})
]]--


--[[
-- Get the current active buffer
local current_buf = vim.api.nvim_get_current_buf()

-- Get all open buffers
local buffer_list = vim.api.nvim_list_bufs()

-- Sort buffer ids to maintain an ordered list
local sorted_buffers = {}
for _, buf in ipairs(buffer_list) do
    table.insert(sorted_buffers, buf)
end
table.sort(sorted_buffers)

-- Find index of current buffer
local current_index = nil
for i, buf in ipairs(sorted_buffers) do
    if buf == current_buf then
        current_index = i
        break
    end
end

-- Open new buffer (can be changed to your preference e.g., opening empty or from file)
vim.cmd('enew') -- Creates a new empty buffer

-- Get the newly opened buffer ID
local new_buf = vim.api.nvim_get_current_buf()

-- Reorder the buffers logically, simulate the new buffer inserting in the correct position
table.insert(sorted_buffers, current_index + 1, new_buf)

-- Ensure all subsequent buffer indices are incremented by 1
for i = current_index + 2, #sorted_buffers do
    -- Move the next buffer's virtual position by shifting their indices.
    -- We handle it logically but do not modify Vim's internal buffer list.
end

-- Optionally, you can switch focus or set the buffer to be active
vim.api.nvim_set_current_buf(new_buf)
]]--



--[[
local map = vim.keymap.set

-- Function to get the list of buffers sorted by most recently used (MRU) order
local function get_sorted_buffers()
  local result = {}
  local bufs = vim.fn.getbufinfo({ buflisted = 1 })
  
  -- Sort the buffers by lastused (most recently used first)
  table.sort(bufs, function(a, b)
    return a.lastused > b.lastused
  end)

  -- Collect the sorted buffer numbers
  for _, buf in ipairs(bufs) do
    table.insert(result, buf.bufnr)
  end

  return result
end

-- Go to the next buffer in the Telescope-sorted list of buffers
map('n', '<Tab>', function()
  local sorted_buffers = get_sorted_buffers()
  local current_bufnr = vim.api.nvim_get_current_buf()

  -- Find the current buffer index in the sorted buffer list
  local current_index = nil
  for i, bufnr in ipairs(sorted_buffers) do
    if bufnr == current_bufnr then
      current_index = i
      break
    end
  end

  -- Move to the next buffer, cycle back to the start if we're at the end
  if current_index then
    local next_index = current_index + 1
    if next_index > #sorted_buffers then
      next_index = 1  -- Loop back to the first buffer
    end
    vim.cmd('buffer ' .. sorted_buffers[next_index])
  end
end, { noremap = true, silent = true, desc = "Go to next buffer" })

-- Go to the previous buffer in the Telescope-sorted list of buffers
map('n', '<S-Tab>', function()
  local sorted_buffers = get_sorted_buffers()
  local current_bufnr = vim.api.nvim_get_current_buf()

  -- Find the current buffer index in the sorted buffer list
  local current_index = nil
  for i, bufnr in ipairs(sorted_buffers) do
    if bufnr == current_bufnr then
      current_index = i
      break
    end
  end

  -- Move to the previous buffer, cycle back to the last if we're at the beginning
  if current_index then
    local prev_index = current_index - 1
    if prev_index < 1 then
      prev_index = #sorted_buffers  -- Loop back to the last buffer
    end
    vim.cmd('buffer ' .. sorted_buffers[prev_index])
  end
end, { noremap = true, silent = true, desc = "Go to previous buffer" })
]]--


