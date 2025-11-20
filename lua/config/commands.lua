local util = require("utilities")


vim.api.nvim_create_user_command("Root", function()
  print(util.get_project_root())
end, {})


vim.api.nvim_create_user_command("PrintBuffers", function()
  local bufs = vim.fn.getbufinfo({ bufloaded = 1 })
  vim.notify(vim.inspect(bufs), vim.log.levels.INFO)
end, {})


vim.api.nvim_create_user_command("Config", function()
	vim.cmd('cd ~/AppData/Local/nvim | edit .')
end, {desc = 'cd to dot files'})


vim.api.nvim_create_user_command('OpenCMD', function()
  local cwd = vim.fn.getcwd():gsub("/", "\\") -- convert to Windows-style path
  vim.fn.jobstart({'cmd.exe'}, {
    cwd = cwd,
    detach = true,
  })
end, {})


-- vim.api.nvim_create_user_command('RemoveUnsavableBuffers', function()
--   for _, buf in ipairs(vim.api.nvim_list_bufs()) do
--     if vim.api.nvim_buf_get_option(buf, 'buflisted') and
--        (vim.api.nvim_buf_get_name(buf) == '' or
--         not vim.api.nvim_buf_get_option(buf, 'modifiable')) then
--       vim.api.nvim_buf_delete(buf, {force = true})
--     end
--   end
-- end, {})

-- With a check to skip unsaved/changed buffers
vim.api.nvim_create_user_command('RemoveUnsavableBuffers', function()
  for _, buf in ipairs(vim.api.nvim_list_bufs()) do
    local listed = vim.api.nvim_buf_get_option(buf, 'buflisted')
    local name = vim.api.nvim_buf_get_name(buf)
    local modifiable = vim.api.nvim_buf_get_option(buf, 'modifiable')
    local modified = vim.api.nvim_buf_get_option(buf, 'modified')
    if listed and (name == '' or not modifiable) and not modified then
      vim.api.nvim_buf_delete(buf, {force = true})
    end
  end
end, {})



-------------------------------------------------------------


function NewCppProject()
  -- Prompt for the destination path
  local path = vim.fn.input('Enter path for the new cpp project: ')

  if path == '' then
    print("Cancelled the new cpp project.")
    return
  end

  -- Define source and destination
  --local source_dir = vim.fn.expand('~/AppData/Local/nvim/new_cpp_project/*')
  local source_dir = vim.fn.expand('~/AppData/Local/nvim/new_cpp_project')
  local destination_dir = vim.fn.expand(path)

  -- Remove trailing slash if present
  destination_dir = destination_dir:gsub("/$", "")

  -- Execute the commands
  os.execute('cp -r ' .. source_dir .. ' ' .. destination_dir)
  --os.execute('cd ' .. destination_dir)
  vim.cmd('cd ' .. destination_dir)
  --vim.cmd('edit ' .. destination_dir .. '/src/main.cpp')
  vim.cmd('edit src/main.cpp')
  vim.cmd('SessionSave')

  -- Notify user
  print('New project created at: ' .. destination_dir)
end

vim.api.nvim_create_user_command('NewCppProject', NewCppProject, {})



-- Function to delete all files in the selected folder
function DeleteFilesInFolder(folder)
  local folder_path = vim.fn.expand("~") .. "/AppData/Local/nvim-data/" .. folder

  -- Use vim.fn.isdirectory to check if the folder exists
  if vim.fn.isdirectory(folder_path) == 1 then
    -- List all files and directories in the folder
    local files = vim.fn.globpath(folder_path, '*', false, true)

    -- Check if the folder contains files
    if #files == 0 then
      print("No files found in " .. folder)
      return
    end

    -- Confirm before deleting files
    local confirm = vim.fn.confirm("Are you sure you want to delete all files in " .. folder .. "?", "&Yes\n&No", 2)
    if confirm == 1 then
      -- Delete all files in the folder
      for _, file in ipairs(files) do
        vim.fn.delete(file, "rf")
      end
      print("All files in " .. folder .. " have been deleted.")
    else
      print("Deletion cancelled.")
    end
  else
    print(folder .. " does not exist.")
  end
end

-- Function to list and select folders using Telescope
function ClearCache()
  -- List of folders to choose from
  local cache_folders = { "shada", "swap", "sessions", "undo"}
  -- local cache_folders = { "shada", "swap", "sessions", "undo", "notes" }

  -- Telescope entry maker for the cache folders
  local make_entry = require('telescope.make_entry')

  -- Open Telescope with these folders for selection
  require('telescope.pickers').new({}, {
    prompt_title = "Select Cache Folder to Clear",
    finder = require('telescope.finders').new_table({
      results = cache_folders,
      entry_maker = make_entry.gen_from_string(), -- Entry maker for string-based entries
    }),
    sorter = require('telescope.sorters').get_generic_fuzzy_sorter(),
    --    attach_mappings = function(prompt_bufnr, map)
    attach_mappings = function(_, map)
      map('i', '<CR>', function()
        local selection = require('telescope.actions.state').get_selected_entry()
        if selection then
          -- Delete all files in the selected folder
          DeleteFilesInFolder(selection.value)
          -- Optional:
          -- vim.cmd('Telescope close')
          -- Update/Redraw/Refresh the dashboard
          -- vim.cmd('DashboardUpdateFooter')
          -- require("dashboard").refresh()
        end
      end)
      return true
    end,
  }):find()
end

-- Create a command to run the ClearCache function
vim.api.nvim_create_user_command('ClearCache', ClearCache, {})



-- vim.api.nvim_set_hl(0, "NotesBorderSaved", { fg = "#555555", bg = "NONE" })
-- vim.api.nvim_set_hl(0, "NotesBorderModified", { fg = "#ff8800", bg = "NONE" })
vim.api.nvim_set_hl(0, "NotesBorderModified", { fg = "#888888", bg = "NONE" })
-- vim.api.nvim_set_hl(0, "NotesBorderModified", { fg = "#FFFFA5", bg = "NONE" })
vim.api.nvim_set_hl(0, "NotesBorderSaved", { fg = "#ffffff", bg = "NONE" }) -- White

-- NOTE: This replaces the 'forest-nvim/maple.nvim' completely
function OpenNotes()
  -- local file = "notes.txt"
  -- local file = "./notes.txt"
  local file = util.get_project_root() .. "/notes.txt"

  local width = math.floor(vim.o.columns * 0.8)
  local height = math.floor(vim.o.lines * 0.8)
  local row = math.floor((vim.o.lines - height) * 0.5) - 1
  local col = math.floor((vim.o.columns - width) * 0.5) - 1

  local buf = vim.fn.bufadd(file)
  vim.fn.bufload(buf)
  -- vim.bo[buf].bufhidden = "hide" -- hide instead of unload/delete on close | Why its already hidden ?

  local win = vim.api.nvim_open_win(buf, true, {
    relative = "editor",
    width = width,
    height = height,
    row = row,
    col = col,
    style = "minimal",
    border = "rounded",
  })

  -- Disable quick-scope plugin
  vim.api.nvim_buf_set_var(buf, 'qs_local_disable', 1)

  vim.api.nvim_win_set_option(win, "winhighlight", "FloatBorder:NotesBorderSaved")

  -- vim.cmd("setlocal winhighlight=Normal:CustomMessagesHighlight")
  -- vim.api.nvim_win_set_option(win, "winhighlight", "Normal:CustomMessagesHighlight")

  vim.api.nvim_win_set_option(win, "winblend", 20) -- transparency
  vim.wo[win].wrap = true -- line wrap
  vim.wo[win].number = false  -- hide line numbers
  vim.wo[win].relativenumber = false

  local close_cmd = function()
    -- if vim.api.nvim_buf_get_option(buf, "modified") then
    --   vim.api.nvim_buf_call(buf, function() vim.cmd("write") end)
    -- end
    if vim.api.nvim_win_is_valid(win) then
      vim.api.nvim_win_close(win, true)
    end
  end

  -- Closes the window, if you close the buffer
  vim.api.nvim_create_autocmd({ "BufUnload", "BufWipeout", "BufLeave" }, {
    buffer = buf,
    callback = close_cmd
  })

  -- Prevent leaving the notes window
  -- Doesnt work, when "BufLeave" is used in the close window autocmd above
  -- vim.api.nvim_create_autocmd("WinLeave", {
  --   callback = function()
  --     vim.defer_fn(function()
  --       if vim.api.nvim_win_is_valid(win) then
  --           vim.api.nvim_set_current_win(win)
  --       end
  --     end, 1)
  --   end,
  --   buffer = buf,
  -- })

  -- vim.api.nvim_create_autocmd("BufLeave", {
  --   buffer = buf,
  --   callback = function()
  --     vim.defer_fn(function()
  --       if vim.api.nvim_buf_is_valid(buf) and vim.api.nvim_buf_is_loaded(buf) then
  --         vim.api.nvim_set_current_buf(buf)
  --       end
  --     end, 1)
  --   end,
  -- })

  local function update_border()
    if not vim.api.nvim_win_is_valid(win) then return end
    if vim.api.nvim_buf_get_option(buf, "modified") then
      vim.api.nvim_win_set_option(win, "winhighlight", "FloatBorder:NotesBorderModified")
    else
      vim.api.nvim_win_set_option(win, "winhighlight", "FloatBorder:NotesBorderSaved")
    end
  end

  vim.api.nvim_create_autocmd({ "TextChanged", "TextChangedI", "BufWritePost" }, {
    buffer = buf,
    callback = update_border,
  })

  -- Set color of the text
  -- vim.opt_local.syntax = "OFF" -- optional: disable syntax
  -- vim.opt_local.ft = "notes"   -- optional: custom filetype to avoid syntax groups
  -- vim.cmd("highlight Normal guifg=#ddddaa guibg=NONE") -- apply to default text
  -- vim.cmd("setlocal winhighlight=Normal:CustomMessagesHighlight")

  vim.keymap.set("n", "<Esc>", close_cmd, { buffer = buf, nowait = true, desc = "Close Notes" })
  vim.keymap.set("n", "<C-c>", close_cmd, { buffer = buf, nowait = true, desc = "Close Notes" })
  vim.keymap.set("n", "<leader>j", close_cmd, { buffer = buf, desc = "Close Notes"})

  -- vim.keymap.set("n", "<leader>q", close_cmd, { buffer = buf, desc = "Close Notes" })
  -- vim.keymap.set("n", "<leader>Q", close_cmd, { buffer = buf, desc = "Close Notes" })

end

vim.api.nvim_create_user_command('Notes', OpenNotes, {})

------------------------------------------------------------------

-- NOTE: This replaces the nvim-conform plugin
local function format_buffer(bufnr)
  bufnr = bufnr or vim.api.nvim_get_current_buf()
  local clients = vim.lsp.get_clients({ bufnr = bufnr })
  if #clients == 0 then return end

  -- Format using the first attached client that supports formatting
  for _, client in ipairs(clients) do
    if client:supports_method("textDocument/formatting") then
      vim.lsp.buf.format({
        bufnr = bufnr,
        async = false,  -- wait for format before saving
        filter = function(c) return c.id == client.id end,  -- only this client
      })
      return
    end
  end
end

vim.api.nvim_create_user_command('Format', format_buffer, {})
vim.keymap.set('n', "<leader>f", format_buffer,
  {desc = "Format Current Buffer", silent = true, noremap = true})

------------------------------------------------------------------

-- Completely replaces "kdheepak/lazygit.nvim",
-- https://github.com/kdheepak/lazygit.nvim?tab=readme-ov-file#telescope-plugin
-- NOTE: LazyGit can be exited with q or Ctrl+c
-- You can cancel commits or pulls with Esc
local function open_lazygit(cmd)
  cmd = cmd or "lazygit"
  local width  = math.floor(vim.o.columns * 0.9)
  local height = math.floor(vim.o.lines * 0.9)
  local row    = math.floor((vim.o.lines - height) / 2)
  local col    = math.floor((vim.o.columns - width) / 2)

  local buf = vim.api.nvim_create_buf(false, true)
  local win = vim.api.nvim_open_win(buf, true, {
    relative = "editor",
    style = "minimal",
    border = "rounded",
    row = row,
    col = col,
    width = width,
    height = height,
  })

  vim.keymap.set("t", "<Esc>", "q", { buffer = buf })
  -- vim.keymap.set("i", "<Esc>", function()
  --   vim.api.nvim_win_close(win, true)
  -- end, { buffer = buf })

  vim.fn.termopen(cmd, {
    on_exit = function()
      vim.api.nvim_win_close(win, true)
    end,
  })

  vim.cmd("startinsert")
end

vim.keymap.set("n", "<leader>lf", "<cmd>LazyGitCurrentFile<CR>", {desc="Open LazyGit [F]ile"})
vim.keymap.set("n", "<leader>lg", open_lazygit, {desc="Open Lazy[G]it"})
vim.api.nvim_create_user_command('LazyGit', open_lazygit, {})

vim.api.nvim_create_user_command('LazyGitConfig', function ()
  -- NOTE: The config file is in %localappdata%\lazygit\config.yml
  open_lazygit("lazygit --config-file")
end, {})

vim.api.nvim_create_user_command('LazyGitCurrentFile', function ()
  local file = vim.fn.expand("%:p")
  open_lazygit("lazygit --filter " .. vim.fn.shellescape(file))
end, {})

vim.api.nvim_create_user_command('LazyGitLog', function ()
  open_lazygit("lazygit log")
end, {})

vim.api.nvim_create_user_command('LazyGitLogCurrentFile', function ()
  local file = vim.fn.expand("%:p")
  open_lazygit("lazygit log " .. vim.fn.shellescape(file))
end, {})

-- See :help keymaps (Which for some reason opens UndoTree files,
-- but it will show you how to write a user commmand with arguments)
-- Or just open %localappdata%\nvim-data\lazy\readme\doc\undotree.md
vim.api.nvim_create_user_command("LazyGitFilter", function(opts)
  local args = opts.fargs
  local file = args[1]
  if vim.fn.filereadable(file) == 1 then
    vim.notify("No file found to LazyGit")
    return
  end
  open_lazygit("lazygit --filter " .. vim.fn.shellescape(file))
end, {
    nargs = 1,
    complete = function(_, line)
      -- Extract the argument typed so far (after the command name)
      local prefix = line:match("^%S+%s+(.*)$") or ""
      return vim.fn.getcompletion(prefix, "file")
    end,
  })

-- vim.api.nvim_create_user_command("LazyGitFilterCurrentFile", ... )

------------------------------------------------------------------


