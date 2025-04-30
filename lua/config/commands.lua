
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
  local cache_folders = { "shada", "swap", "sessions", "undo" }

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




