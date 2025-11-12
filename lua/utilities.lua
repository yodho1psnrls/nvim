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

-- BUG: Doesnt Work !
function M.is_qf_open()
  for _, win in ipairs(vim.fn.getwininfo()) do
    if win.quickfix == 1 then
      return true
    end
  end
  return false
end

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


function M.get_project_root()
  -- Get all LSP clients for the current buffer
  -- https://stackoverflow.com/questions/76518562/looking-for-current-buffer-lsp-clients
  -- local bufnr = vim.api.nvim_get_current_buf()
  -- local clients = vim.lsp.buf_get_clients(bufnr) -- Deprecated
  local clients = vim.lsp.get_clients()   -- But this will get all clients
  -- vim.lsp.buf_get_clients()

  -- Check if there is an LSP client active for the current buffer
  if next(clients) ~= nil then
    -- For the first LSP client found, get the root directory
    for _, client in pairs(clients) do
      return client.config.root_dir
    end
  -- else
  --   print("No LSP client is attached to the buffer")
  end

  --return nil
  return vim.fn.getcwd()
end


function M.safe_require(module_name)
  if not package.loaded[module_name] then
    return nil
  end
  return require(module_name)
end


function M.is_loaded(plugin_name)
  local lazy = require("lazy")
  for _, plugin in pairs(lazy.plugins()) do
    if plugin.name == plugin_name then
      return plugin._.loaded == true
    end
  end
  return false
end


-- https://www.rapidtables.com/web/color/RGB_Color.html
vim.api.nvim_set_hl(0, "CustomMessagesHighlight", {
  fg = "#CDB29B",
  -- bg = "#000000",
  -- bold = true,
})


function M.center_cursor()
  local win_height = vim.api.nvim_win_get_height(0)
  local buf_lines = vim.api.nvim_buf_line_count(0)
  local row = math.min(buf_lines, math.ceil(win_height / 2))
  vim.api.nvim_win_set_cursor(0, { row, 0 })
end


local function get_window_state()
  local state = {}
  local windows = vim.api.nvim_tabpage_list_wins(0) -- List all windows in the current tab
  for _, win in ipairs(windows) do
    local buf = vim.api.nvim_win_get_buf(win) -- Get buffer in the window
    local config = vim.api.nvim_win_get_config(win) -- Get window config
    table.insert(state, {
      buf = buf,
      config = config,
    })
  end
  return state
end

function M.close_messages_buffer()
  vim.cmd('nohlsearch')
  -- vim.cmd('match none')
  vim.cmd('call clearmatches()')
  vim.cmd('Bdelete')
end

function M.open_messages_in_buffer()
  -- vim.cmd("enew")
  vim.cmd("e " .. "Messages") -- 💬 ✉️  📩 🔔 🛈 🗨️

  -- if #vim.api.nvim_list_wins() == 1 then
  --   vim.o.showtabline = 0
  -- else

  -- vim.cmd("normal! ggdG") -- Clear the new buffer to avoid any content
  -- vim.cmd("0put =execute('messages')") -- Insert the output of `:messages` into the buffer

  -- These do not copy the messages into main clipboard
  -- vim.cmd("0put =execute('messages') | let @-=''")
  local messages = vim.fn.execute('messages')
  local lines = vim.split(messages, '\n')
  vim.api.nvim_buf_set_lines(0, 0, 0, false, lines)

  vim.cmd("normal! gg\"_dd") -- delete the empty first line
  -- vim.cmd("normal! gg")
  M.center_cursor()

  vim.cmd("setlocal buftype=nofile")
  -- vim.cmd("setlocal buftype=nowrite")
  vim.cmd("setlocal bufhidden=wipe") -- Automatically delete the buffer when closed
  vim.cmd("setlocal nobuflisted") -- Exclude the buffer from the buffer list
  vim.cmd("setlocal noswapfile") -- Do not create a swap file
  vim.cmd("setlocal nomodifiable")
  -- vim.cmd("setlocal nu") -- With line numbers (nu is on, nonu is off and nu! is toggle)
  vim.cmd("setlocal nonu") -- No line numbers (nu is on, nonu is off and nu! is toggle)
  vim.cmd("setlocal nornu") -- No relative line numbers
  -- vim.cmd("setlocal readonly")

  vim.cmd("setlocal wrap") -- wrap | nowrap
  -- vim.cmd("setlocal whichwrap=")
  -- vim.cmd("setlocal wrapscan")
  vim.cmd("setlocal linebreak") -- same as the two above, but more appealing

  local half_height = math.floor(vim.api.nvim_win_get_height(0) / 2)
  vim.cmd("setlocal scrolloff=" .. half_height)

  -- vim.keymap.set('n', '<Space>' , "<cmd>Bdelete<CR>", { buffer = 0, noremap = true, silent = true })
  vim.keymap.set('n', '<CR>' , M.close_messages_buffer, { buffer = 0, noremap = true, silent = true })
  vim.keymap.set('n', '<Esc>' , M.close_messages_buffer, { buffer = 0, noremap = true, silent = true })
  vim.keymap.set('n', '<Tab>' , M.close_messages_buffer, { buffer = 0, noremap = true, silent = true })
  vim.keymap.set('n', '<S-Tab>' , M.close_messages_buffer, { buffer = 0, noremap = true, silent = true })

  vim.keymap.set('n', '<leader>x' , M.close_messages_buffer, { buffer = 0, noremap = true, silent = true })

  vim.cmd("setlocal winhighlight=Normal:CustomMessagesHighlight")
  -- vim.cmd("setlocal winhighlight=Normal:Label")
  -- vim.cmd("setlocal winhighlight=Normal:WarningMsg")

  -- vim.cmd("match Search /error:/") -- highlight errors

  -- " Highlight 'warning:' using the same highlight as WARN: in todo-comments
  vim.cmd("call matchadd('TodoFgWARN', 'warning:')")
  vim.cmd("call matchadd('TodoBgWARN', 'warning:')")

  -- " Highlight 'note:' using the same highlight as NOTE: in todo-comments
  vim.cmd("call matchadd('TodoFgNOTE', 'note:')")
  vim.cmd("call matchadd('TodoBgNOTE', 'note:')")

  -- " Highlight 'error:' in red (uses 'ErrorMsg' group)
  -- vim.cmd("call matchadd('ErrorMsg', 'error:')")
  -- " Highlight 'error:' using the same highlight as BUG: in todo-comments
  vim.cmd("call matchadd('TodoFgFIX', 'error:')")
  vim.cmd("call matchadd('TodoBgFIX', 'error:')")

  -- vim.cmd('normal /error:')
  vim.cmd('normal! G')

end



function M.move_current_fold_contents_after()
  -- Get the current cursor position
  local cursor_pos = vim.api.nvim_win_get_cursor(0)
  local current_line = cursor_pos[1]

  -- Get fold boundaries
  local fold_start = vim.fn.foldclosed(current_line)
  local fold_end = vim.fn.foldclosedend(current_line)

  if fold_start == -1 or fold_end == -1 then
    vim.notify("Cursor is not inside a closed fold", vim.log.levels.WARN)
    return
    -- vim.api.nvim_input("zc")
  end

  -- Extract fold content
  local fold_content = vim.api.nvim_buf_get_lines(0, fold_start - 1, fold_end, false)

  -- Find the next fold's start line
  local next_fold_start = vim.fn.foldclosed(fold_end + 1)
  if next_fold_start == -1 then
    next_fold_start = fold_end + 1 -- No next fold, append after the current one
  end

  -- Insert fold content after the current fold
  vim.api.nvim_buf_set_lines(0, next_fold_start, next_fold_start, false, fold_content)

  vim.notify("Moved current fold contents after the fold")

  -- if fold_start == -1 or fold_end == -1 then
  --   vim.api.nvim_input('l')
  -- end
end



function M.outline_cpp_inline_methods()
  local lines = vim.api.nvim_buf_get_lines(0, 0, -1, false)
  local class_name = nil
  local method_lines = {}
  local class_end_index = nil
  local outlined_methods = {}

  -- Parse lines
  for i, line in ipairs(lines) do
    -- Capture class name
    -- local match_class = line:match("^class%s+(%w+)%s*{")
    local match_class = line:match("^(class|struct)%s+(%w+)%s*{")
    if match_class then
      class_name = match_class
    end

    -- Capture class end
    if line:match("^};") and class_name and not class_end_index then
      class_end_index = i
    end

    -- Capture inline methods
    local match_method, body = line:match("^%s*(%w+%s+%w+%b()%s*)%{(.*)%}$")
    if class_name and match_method and body then
      table.insert(method_lines, { match_method = match_method, body = body })
      lines[i] = line:gsub("{.*}", ";") -- Convert inline to declaration
    end
  end

  -- Add outlined methods
  if class_end_index then
    table.insert(outlined_methods, "") -- Empty line for separation
    for _, method in ipairs(method_lines) do
      table.insert(outlined_methods, "inline " .. class_name .. "::" .. method.match_method .. " { " .. method.body .. " }")
    end
    vim.list_extend(lines, outlined_methods, class_end_index + 2) -- Insert methods after the class
  end

  -- Update buffer with modified lines
  vim.api.nvim_buf_set_lines(0, 0, -1, false, lines)
end


function M.file_exists(file_path)
  local stat = vim.loop.fs_stat(file_path)
  return stat and stat.type == 'file'
end

function M.find_python_exec()
  local root_main = M.get_project_root() .. '/' .. "__main__.py"
  local cwd_main = vim.fn.getcwd() .. '/' .. "__main__.py"
  if M.file_exists(root_main) then
    -- return vim.fn.fnamemodify(main_path, ':p')
    return root_main
  elseif M.file_exists(cwd_main) then
    return cwd_main
  else
    return '%'
  end
end


-- Create a user command to call the function
-- vim.api.nvim_create_user_command('CloseEmptyBuffers', function() M.close_empty_buffers end, { desc = 'Close all empty buffers' })

-- Removes the carriage return character "\r" or shown as "^M"
function M.remove_carriage_returns(lines)
  -- local clean = {}
  -- for _, line in ipairs(lines) do
  --   table.insert(clean, line:gsub("\r$", ""))
  -- end
  local clean = vim.tbl_map(function(line)
    return line:gsub("\r$", "")
  end, lines)
  return clean
  -- return lines:gsub("\r$", "")
end


return M
