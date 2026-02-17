local M = {}

-- Executable choser

local util = require'utilities'

local pickers = require("telescope.pickers")
local finders = require("telescope.finders")
local conf = require("telescope.config").values
local actions = require("telescope.actions")
local action_state = require("telescope.actions.state")

local uv = vim.loop
local json = vim.fn.json_decode

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

-- Recursively scan folder for .exe files (Windows) or executables (Unix)
--[[function M.scan_executables(folder, exclude_folders)
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
end]]--

--[[local function read_json(path)
  local content = table.concat(vim.fn.readfile(path), "\n")
  return vim.fn.json_decode(content)
end

local function glob_one(pattern)
  local matches = vim.fn.glob(pattern, true, true)
  if #matches == 0 then return nil end
  return matches[1]
end

function M.get_executables(build_dir)
  local reply_dir = build_dir .. "/.cmake/api/v1/reply"

  -- 1️⃣ Read index file
  local index_file = glob_one(reply_dir .. "/index-*.json")
  if not index_file then
    vim.notify("No CMake File API index found", vim.log.levels.ERROR)
    return {}
  end

  local index = read_json(index_file)

  -- 2️⃣ Find codemodel object
  local codemodel_file
  for _, obj in ipairs(index.objects or {}) do
    if obj.kind == "codemodel" then
      codemodel_file = reply_dir .. "/" .. obj.jsonFile
      break
    end
  end

  if not codemodel_file then
    vim.notify("No codemodel found in File API reply", vim.log.levels.ERROR)
    return {}
  end

  local codemodel = read_json(codemodel_file)

  local executables = {}

  -- 3️⃣ Iterate configurations
  for _, config in ipairs(codemodel.configurations or {}) do
    local config_name = config.name

    for _, target in ipairs(config.targets or {}) do
      local target_file = reply_dir .. "/" .. target.jsonFile
      local target_data = read_json(target_file)

      if target_data.type == "EXECUTABLE" then
        local artifact_path = nil
        if target_data.artifacts and target_data.artifacts[1] then
          artifact_path = target_data.artifacts[1].path
        end

        table.insert(executables, {
          name = target_data.name,
          path = artifact_path,
          config = config_name,
        })
      end
    end
  end

  return executables
end]]--

-- Get logical target names from `cmake --build --target help`
function M.get_targets(build_dir, config)
  local cmd = { "cmake", "--build", build_dir, "--target", "help" }
  if config then
    table.insert(cmd, 4, "--config")
    table.insert(cmd, 5, config)
  end

  local output = vim.fn.systemlist(cmd)
  local targets = {}

  for _, line in ipairs(output) do
    line = line:gsub("\r", "")  -- remove trailing \r
    for exe in line:gmatch("([%w%._%-]+%.exe)") do
      table.insert(targets, exe)
    end
  end

  -- print(vim.inspect(output))
  return targets
end

-- Find all paths to a file name inside build_dir
function M.find_paths(build_dir, file_name)
  local paths = {}

  -- Add .exe automatically on Windows if missing
  if vim.loop.os_uname().sysname == "Windows_NT" then
    if not file_name:match("%.exe$") then
      file_name = file_name .. ".exe"
    end
  end

  local pattern = build_dir .. "/**/" .. file_name
  local matches = vim.fn.glob(pattern, true, true)

  for _, path in ipairs(matches) do
    table.insert(paths, path)
  end

  return paths
end

function M.get_executables(build_dir, config)
  build_dir = build_dir or 'Release'
  local targets = M.get_targets(build_dir, config)
  local executables = {}

  for _, target in ipairs(targets) do
    table.insert(executables, target)  -- just the name
  end

  return executables
end


local chosen_executable = nil

function M.PickExeInFolder(folder_path)
  folder_path = folder_path or util.get_project_root()
  folder_path = folder_path .. "\\Release"

  local exe_files = M.get_executables(folder_path)

  if #exe_files == 0 then
    vim.notify("No .exe files found in: " .. folder_path, vim.log.levels.WARN)
    return
  end

  local preselect_index = 1
  for i, name in ipairs(exe_files) do
    if name == chosen_executable then
      preselect_index = i
      break
    end
  end

  pickers.new({}, {
    prompt_title = "Select EXE",
    finder = finders.new_table { results = exe_files },
    sorter = conf.generic_sorter({}),
    default_selection_index = preselect_index,
    layout_strategy = "vertical",
    layout_config = {
      width = 0.4,      -- 40% of screen width
      height = 0.4,     -- 40% of screen height
    },
    -- winblend = 10,  -- 0 = opaque, 100 = fully transparent
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

vim.keymap.set({'n', 'i'}, "<F6>", M.PickExeInFolder, {desc='Choose Exe'})

function M.get_release_executable()
  chosen_executable = chosen_executable or M.get_executables()[1]
  -- return string.gsub(chosen_executable, 'Debug', 'Release')
  -- return M.find_paths(util.get_project_root() .. '\\Release\\', chosen_executable)[1]
  local found_exes = M.find_paths(util.get_project_root(), chosen_executable)
  for _, path in ipairs(found_exes) do
    if path:find('Release', 1, true) then  -- plain search (no patterns)
      return path
    end
  end
end

function M.get_debug_executable()
  chosen_executable = chosen_executable or M.get_executables()[1]
  -- return string.gsub(chosen_executable, 'Release', 'Debug')
  -- return M.find_paths(util.get_project_root() .. '\\Debug\\', chosen_executable)[1]
  local found_exes = M.find_paths(util.get_project_root(), chosen_executable)
  for _, path in ipairs(found_exes) do
    if path:find('Debug', 1, true) then  -- plain search (no patterns)
      return path
    end
  end
end

return M
