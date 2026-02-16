local M = {}

-- Executable choser

local util = require'utilities'

local pickers = require("telescope.pickers")
local finders = require("telescope.finders")
local conf = require("telescope.config").values
local actions = require("telescope.actions")
local action_state = require("telescope.actions.state")
local uv = vim.loop

-- Recursively scan folder for .exe files (Windows) or executables (Unix)
function M.scan_executables(folder, exclude_folders)
  folder = folder or vim.fn.getcwd()
  -- folder = folder or util.get_project_root()
  exclude_folders = exclude_folders or {'CMakeFiles', '.git', '.cache'}
  local results = {}

  local function is_excluded(dir_name)
    for _, excl in ipairs(exclude_folders) do
      if dir_name == excl then
        return true
      end
    end
    return false
  end

  local function scan(dir, root)
    root = root or dir
    local handle = uv.fs_scandir(dir)
    if not handle then return end

    while true do
      local name, _ = uv.fs_scandir_next(handle)
      if not name then break end
      local sep = uv.os_uname().version:match("Windows") and "\\" or "/"
      local full_path = dir .. sep .. name
      local stat = uv.fs_stat(full_path)

      if stat then
        if stat.type == "file" then
          -- Windows: only .exe
          if uv.os_uname().version:match("Windows") then
            if name:match("%.exe$") then
              local relative_path = full_path:sub(#root + 2)
              table.insert(results, relative_path)
            end
          else
            -- Unix: executable bit
            local bit = require("bit")
            if bit.band(stat.mode, 73) ~= 0 then
              local relative_path = full_path:sub(#root + 2)
              table.insert(results, relative_path)
            end
          end
        elseif stat.type == "directory" then
          if not is_excluded(name) then
            scan(full_path, root)
          end
        end
      end
    end
  end

  scan(folder)
  return results
end

--[[local function scan_executables(_)
  local targets = vim.fn.systemlist("cmake --build Release --target help")
  -- filter out 'all', 'clean', etc.
  local executables = {}
  for _, t in ipairs(targets) do
    if t ~= "all" and t ~= "clean" then
      table.insert(executables, t)
    end
  end
  return executables
end]]--

local chosen_executable = nil

function M.PickExeInFolder(folder_path)
  folder_path = folder_path or vim.fn.getcwd()
  -- folder_path = folder_path or util.get_project_root()
  local exe_files = M.scan_executables(folder_path)

  if #exe_files == 0 then
    vim.notify("No .exe files found in: " .. folder_path, vim.log.levels.WARN)
    return
  end

  pickers.new({}, {
    prompt_title = "Select EXE",
    finder = finders.new_table { results = exe_files },
    sorter = conf.generic_sorter({}),
    attach_mappings = function(prompt_bufnr, map)
      actions.select_default:replace(function()
        local selection = action_state.get_selected_entry()
        chosen_executable = selection[1]
        vim.notify("Selected executable: " .. selection[1])
        actions.close(prompt_bufnr)
      end)
      return true
    end
  }):find()
end

-- local modes = {
--   Release = 'Release',
--   Debug = 'Debug'
-- }

vim.keymap.set({'n', 'i'}, "<F7>", M.PickExeInFolder, {desc='Choose Exe'})

function M.get_release_executable()
  chosen_executable = chosen_executable or M.scan_executables()[1]
  return string.gsub(chosen_executable, 'Debug', 'Release')
end

function M.get_debug_executable()
  chosen_executable = chosen_executable or M.scan_executables()[1]
  return string.gsub(chosen_executable, 'Release', 'Debug')
end

return M
