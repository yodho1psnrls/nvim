
local util = require("utilities")
local pickers = require("telescope.pickers")
local finders = require("telescope.finders")
local conf = require("telescope.config").values
local actions = require("telescope.actions")
local action_state = require("telescope.actions.state")

--[[local function scan_cpp_json(root)
  local cmd = string.format(
    'fd --type f --extension cpp.json "%s" 2>/dev/null || find "%s" -type f -name "*.cpp.json"',
    root,
    root
  )
  local handle = io.popen(cmd)
  if handle == nil then return end
  local result = handle:read("*a")
  handle:close()

  local files = {}
  for file in result:gmatch("[^\r\n]+") do
    table.insert(files, file)
  end
  return files
end]]--

-- Find all .cpp.json files recursively from current working dir
--[[local function scan_cpp_json(dir)
  local files = {}
  local handle = vim.loop.fs_scandir(dir)
  if not handle then return files end

  while true do
    local name, typ = vim.loop.fs_scandir_next(handle)
    if not name then break end
    local path = dir .. "/" .. name
    if typ == "file" and name:match("%.cpp%.json$") then
      table.insert(files, path)
    elseif typ == "directory" then
      -- recurse
      local subfiles = scan_cpp_json(path)
      vim.list_extend(files, subfiles)
    end
  end

  return files
end

local function open_trace_in_edge(root)
  root = root or util.get_project_root()
  local files = scan_cpp_json(root)
  -- print(vim.inspect(files))
  -- Telescope picker
  pickers.new({}, {
    prompt_title = "Select Compiler Trace File (.cpp.json)",
    finder = finders.new_table {
      results = files,
    },
    sorter = conf.generic_sorter({}),
    attach_mappings = function(prompt_bufnr, _)
      actions.select_default:replace(function()
        actions.close(prompt_bufnr)
        local selection = action_state.get_selected_entry().value
        if selection then
          -- Open in Edge private mode
          vim.fn.jobstart({ "cmd", "/c", "start", "msedge", "--inprivate", selection }, { detach = true })
        end
      end)
      return true
    end,
  }):find()
end]]--


-- Recursive scanner
local function scan_cpp_json(dir, root)
  root = root or dir
  dir = dir:gsub("\\", "/")
  local files = {}
  local handle = vim.loop.fs_scandir(dir)
  if not handle then return files end

  while true do
    local name, typ = vim.loop.fs_scandir_next(handle)
    if not name then break end
    local path = dir .. "/" .. name
    if typ == "file" and name:match("%.cpp%.json$") then
      -- store { display = relative_path, full = absolute_path }
      local rel = path:sub(#root + 2) -- +2 for "/" after root
      table.insert(files, { display = rel, full = path })
    elseif typ == "directory" then
      local subfiles = scan_cpp_json(path, root)
      vim.list_extend(files, subfiles)
    end
  end
  return files
end

-- Detect OS and return Edge command
local function get_edge_command(file)
  local os_name = vim.loop.os_uname().sysname
  if os_name:match("Windows") then
    return { "cmd", "/c", "start", "msedge", "--inprivate", file }
  else
    return { "microsoft-edge", "--inprivate", file }
  end
end

local function open_trace_in_edge(root)
  root = root or vim.fn.getcwd()
  if not root or root == "" then
    vim.notify("Invalid root directory", vim.log.levels.ERROR)
    return
  end

  local files = scan_cpp_json(root, root)
  if #files == 0 then
    vim.notify("No .cpp.json files found in " .. root, vim.log.levels.WARN)
    return
  end

  -- Telescope picker
  pickers.new({}, {
    prompt_title = "Select Clang Trace File (.cpp.json)",
    finder = finders.new_table {
      results = files,
      entry_maker = function(entry)
        return {
          value = entry.full,
          display = entry.display,
          ordinal = entry.display,
        }
      end,
    },
    sorter = conf.generic_sorter({}),
    attach_mappings = function(prompt_bufnr, _)
      actions.select_default:replace(function()
        actions.close(prompt_bufnr)
        local selection = action_state.get_selected_entry().value
        if selection then
          -- Open in Edge private mode (Windows)
          -- vim.fn.jobstart({ "cmd", "/c", "start", "msedge", "--inprivate", selection }, { detach = true })
          -- vim.fn.jobstart(get_edge_command(selection), { detach = true })
          vim.fn.jobstart("speedscope " .. selection, { detach = true })
        end
      end)
      return true
    end,
  }):find()
end


vim.keymap.set("n", "<leader>tt", function()
  open_trace_in_edge()
end, { desc = "Open ftime-trace in Edge" })


