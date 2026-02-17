-- F7 will Generate both Debug and Release Directories
-- F5 will Build the Debug directory and launch dap
-- F6 will Build the Release directory and launch the exe, without a debugger

-- All those functions expect that the cwd is set on the root directory of the project !

-- TODO: 
-- Make the build, run, debug and generate independent of your
--  current working directory. Make it dependant on the current
--  buffer that your in. Such that from the buffer that you are in
--  it traces the root directory of the project.
-- You can use the same logic that the LSP uses or directly retreive
--  it from the root directory that the LSP detected
-- This will be a plus for example, if you are in some folder with
--  multiple small projects, so you can easily switch between them
--  just by opening a file from a certain project, without the need
--  of always changing the directory and keeping track if you are in
--  an actual root directory of a project, you can always stay in the
--  upper one that stores all those projects.
-- Or you can be in a big project, deep inside some library,
--  and still be able to run the whole project, 
--  without going to the root directory

-- https://neovim.discourse.group/t/how-to-new-a-terminal-and-send-to-run-command-with-lua/4218
-- https://stackoverflow.com/questions/7597249/vim-script-how-to-execute-a-command-in-a-vim-function
-- https://neovim.discourse.group/t/how-to-new-a-terminal-and-send-to-run-command-with-lua/4218

-- local dap = require('dap')


-- TODO: Get the cpp path to executables from the root CMakeLists.txt file 
-- This way they could be with different names and paths and it would be
-- more generalized for projects other than mine


-------------------------------------------------------------------------

local function prepend_time(mes)
  return "[" .. os.date("%H:%M:%S") .. "] " .. mes
end

local util = require"utilities"


local function SaveUnavedFiletype(type)
  for _, bufnr in ipairs(vim.api.nvim_list_bufs()) do
    if vim.api.nvim_buf_is_loaded(bufnr) then
      local filetype = vim.api.nvim_buf_get_option(bufnr, "filetype")
      local modified = vim.api.nvim_buf_get_option(bufnr, "modified")
      local name = vim.api.nvim_buf_get_name(bufnr)

      if filetype == type and modified and name ~= "" then
        -- Save the buffer
        vim.api.nvim_buf_call(bufnr, function()
          vim.cmd("write")
        end)
      end
    end
  end
end


function GenerateCMake()
  SaveUnavedFiletype('cmake')

  local last_cwd = vim.fn.getcwd()
  local root = util.get_project_root()
  if last_cwd ~= root then vim.cmd('cd ' .. root) end

  -- NOTE: you can also use os.execute('...') instead of vim.cmd('!...')

  vim.cmd('!rm -rf "Debug"')
  vim.cmd('!rm -rf "Release"')
  --vim.cmd('!rm -rf "Test"')

  -- Trigger CMake File API, for generating jsons that give info about executables and more
  -- NOTE: If you're using multi-config generators like Visual Studio or Ninja Multi-Config,
  -- then build directories might be build/, build/Debug/ and build/Release/
  -- The .cmake/api folder must be created in the top-level build directory, not inside Debug or Release
  --[[local query_dir = "Debug/.cmake/api/v1/query/client-nvim"
  local query_path = query_dir .. "/query.json"
  vim.fn.mkdir(query_dir, "p")
  vim.fn.writefile({
    "{",
    '  "requests": [',
    '    { "kind": "codemodel", "version": 2 }',
    "  ]",
    "}"
  }, query_path)]]--

  vim.cmd('!cmake -S . -B "Debug" -DCMAKE_BUILD_TYPE=Debug')
  vim.cmd('!cmake -S . -B "Release" -DCMAKE_BUILD_TYPE=Release')

  -- Create a symlink for compile_commands.json, so language server protocols
  --  like clangd or ccls work properly
  -- Creates a hardlink, so you dont need elevated permissions (no need to restart the terminal)
  --https://www.howtogeek.com/16226/complete-guide-to-symbolic-links-symlinks-on-windows-or-linux/
  vim.cmd('!rm compile_commands.json')
  --vim.cmd('!mklink "compile_commands.json" "Debug\\compile_commands.json"')
  vim.cmd('!mklink /H "compile_commands.json" "Debug\\compile_commands.json"')

  if last_cwd ~= root then vim.cmd('cd ' .. last_cwd) end
end

local function has_qf_diagnostics()
  local qf = vim.fn.getqflist()
  for _, item in ipairs(qf) do
    -- Mostly warning and error items are marked as valid,
    -- because they map to a line and col and bufnr that exists
    if item.valid == 1 then
      return true
    end
  end
  return false
end

-- This is very important to check, because if you have an important warning
-- and compile again, the next qflist will populate with no work to do,
-- which will leave the warning shown in the previous qflist entry,
-- which will not trigger has_qf_diagnostics, and you may not notice the warning
local function build_has_work(lines)
  for _, line in ipairs(lines) do
    -- skip empty lines
    if line:match("%S") then
      -- check if it is a generic “nothing to do” phrase
      if line:match("[Nn]othing to be done") or
         line:match("no work to do") or
         line:match("0 succeeded, 0 failed") then
        return false
      else
        -- any other non-empty line likely indicates work
        return true
      end
    end
  end
  -- all lines empty
  return false
end

-- TODO: WHY I CANT MAKE THIS TO OPEN MESSAGES AFTER ERROR
function BuildCmakeConfig(config_name, on_success_callback)
-- function BuildCmakeConfig(config_name, on_exit_callback)

  -- Another way to execute it on external shell is using systemlist
  -- This is an alternative for the vim.fn.jobstart below
  -- local output = vim.fn.system("ninja -C build") -- single string with all the lines separated with \n
  -- local output = vim.fn.systemlist("ninja -C build") -- lines of strings without \n at end
  -- local clean = vim.tbl_map(function(line) return line:gsub("\r$", "") end, output)
  -- vim.fn.setqflist({}, "r", {lines = clean})

  local last_cwd = vim.fn.getcwd()
  local root = util.get_project_root()
  if last_cwd ~= root then vim.cmd('cd ' .. root) end

  vim.fn.jobstart(
    'cmake --build "' .. config_name .. '" --config ' .. config_name, {
    stdout_buffered = true,
    stderr_buffered = true,
    -- the data are the lines of diagnostics printed out by cmake
    -- neovim then parses them into quickfix list items
    on_stdout = function(_, data)
      -- local has_work = not util.does_end_with(data[1].text, "ninja: no work to do.\r")
      local has_work = build_has_work(data)
      if data and #data > 0 and has_work then
        -- print(table.concat(data, "\n"))
        vim.fn.setqflist({}, ' ', {
          title = 'CMake Build',
          -- lines = data,
          lines = util.remove_carriage_returns(data),

          -- You can maintain multiple qflists, by setting each one a number
          -- nr=1, -- (Default: -1)
          -- Use :cgetexpr getqflist(nr=2) to switch between qflists
        })
      end
    end,

    on_stderr = function(_, data)
      -- NOTE: Even on error, the data is empty, so this mes doesnt show up
      -- this suggests that on_stderr is called always, not only on error
      if data and #data > 1 then
        print("Error during build:\n" .. table.concat(data, "\n"))
        -- vim.cmd("copen")
      end
    end,

    on_exit = function(_, exit_code)
      local msg = ""
      if exit_code ~= 0 then
        msg = "Build failed with exit code: " .. exit_code
        -- vim.cmd('messages') -- WHY THIS DOESNT TRIGGER ??? (IT IS BECAUSE THE KEYMAP THAT CALLS THIS FUNCTION IS SILENT !!!)
        -- util.open_messages_in_buffer()
      else
        if on_success_callback then
          msg = "Build exited with code: " .. exit_code
          on_success_callback()
        end
      end
      -- on_exit_callback(exit_code)
      print(prepend_time(msg));

      if has_qf_diagnostics() then
        -- vim.cmd("copen | only") -- Open quickfix list
        vim.cmd("copen") -- Open quickfix list
        -- require('quicker').toggle()
      end

      if last_cwd ~= root then vim.cmd('cd ' .. last_cwd) end
      -- vim.defer_fn(function() vim.cmd('messages') end, 5000) -- same as above, but with delay
      -- vim.cmd(echo execute('messages'))

    end,
  })
end

function LaunchDapConfig(filetype, config_name)
  local dap = require('dap')
  for _, config in ipairs(dap.configurations[filetype]) do
    if config.name == config_name then
      dap.run(config)
      return
    end
  end
  print("No " .. config_name .. "configuration for " .. filetype)
end

local ep = require('executable_picker')

---------------------------------------------------------------------

function BuildAndRunCpp()
  print(prepend_time("Building cpp ..."))
  -- BuildCmakeConfig('Release', function() vim.cmd('12split | term .\\Release\\bin\\proj.exe') end)
  BuildCmakeConfig('Release', function()
    -- vim.cmd('10split | term ' .. util.get_project_root() .. '\\Release\\bin\\proj.exe')
    local exe_to_run = ep.get_release_executable()
    vim.cmd('12split | term ' .. exe_to_run)
    vim.cmd("setlocal bufhidden=wipe") -- Close buffer when you switch away from it
    vim.cmd("setlocal nobuflisted") -- Exclude the buffer from the buffer list
    vim.cmd("normal! G")
    if vim.api.nvim_get_mode().mode ~= 't' then
      vim.api.nvim_input('i')
    end
  end)
end

function BuildAndDebugCpp()
  print(prepend_time("Building cpp ..."))
  BuildCmakeConfig('Debug', function() LaunchDapConfig('cpp', 'Launch Project') end)
end

------------------------------------------------------------------------
-- There is no Building for python, but i want to keep the names consistent

function BuildAndRunPython()
  print(prepend_time("Running Python ..."))
  --local file = vim.fn.expand('%') -- Get the current file name
  --vim.cmd('!python ' .. file)
  -- vim.cmd('10split | term python %') -- 12split
  vim.cmd('10split | term python ' .. util.find_python_exec()) -- 12split
  -- vim.cmd("<cmd>!uv run " .. util.find_python_exec() .. "<CR>")

  -- Fix weird error that input is not received (Have to focus it with mouse)
  -- vim.cmd('stopinsert')
  vim.cmd('startinsert')
  -- vim.cmd('redraw')
  -- local new_win_id = vim.api.nvim_get_current_win()
  -- vim.api.nvim_set_current_win(new_win_id)

  vim.cmd("setlocal bufhidden=wipe") -- Close buffer when you switch away from it
  -- vim.cmd("setlocal nobuflisted") -- Exclude the buffer from the buffer list
  --vim.cmd('term python %')

  -- NOTE:
  -- if vim.api.nvim_get_mode().mode ~= 't' then
  --   vim.api.nvim_input('i')
  -- end
  -- vim.cmd('<C-\\>')
  -- vim.cmd('normal! <C-\\><C-n>')
  -- vim.cmd('startinsert')
  -- vim.cmd('redraw!')
  -- vim.cmd('suspend')

  -- print("Exit Code is: " .. get_exit_code())
end

function BuildAndDebugPython()
  print(prepend_time("Debugging Python ..."))
  -- require('dap').continue()
  -- LaunchDapConfig('python', 'custom')
  LaunchDapConfig('python', 'Launch File')
end



function BuildAndRunJavaScript()
  print(prepend_time("Running JavaScript ..."))
  vim.cmd('10split | terminal node %') -- 12split
  vim.cmd("setlocal bufhidden=wipe") -- Close buffer when you switch away from it

  if vim.api.nvim_get_mode().mode ~= 't' then
    vim.api.nvim_input('i')
  end
end

function BuildAndDebugJavaScript()
  print(prepend_time("Debugging JavaScript ..."))
  -- require('dap').continue()
  LaunchDapConfig('javascript', 'Launch File')
end


--=======================================================================--



local function are_there_any_breakpoints_in_same_filetype_buffers(filter_by_id)

  local curr_type = vim.bo.filetype
  if not curr_type or curr_type == "" then
    return false
  end

  local db = util.safe_require("dap.breakpoints")
  if not db then
    return false
  end

  -- https://www.reddit.com/r/neovim/comments/15cpsy0/comment/jtysfd4/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
  -- print(vim.inspect(db.get()))
  local buffers_with_breakpoints = db.get()
  for id, breakpoints in pairs(buffers_with_breakpoints) do
    -- if vim.bo[id] and vim.bo[id].filetype == curr_type and #breakpoints > 0 then
    if vim.bo[id].filetype == curr_type then
      if filter_by_id and filter_by_id(id) then
        return true
      end
    end
  end

  return false
end



-- local function search_folder_names_above_file(folder_names, file_abs_path)
--   file_abs_path = file_abs_path or vim.api.nvim_buf_get_name(0) -- Get the current buffer's file path
local function search_folder_names_above_file(folder_names, buf_id)
  buf_id = buf_id or 0
  local file_abs_path = vim.api.nvim_buf_get_name(buf_id) -- Get the current buffer's file path

  if file_abs_path == "" then
    print("No file is currently loaded in the buffer.")
    return
  end

  local curr_folder_path = vim.fn.fnamemodify(file_abs_path, ":h") -- Get the directory of the file

  while true do
    local curr_folder_name = vim.fn.fnamemodify(curr_folder_path, ":t")

    if curr_folder_name == "" then
      -- print("===========================")
      return
    end

    for _, folder_name in ipairs(folder_names) do
      if curr_folder_name:find(folder_name) then
        return folder_name
      end
    end

    -- print(curr_folder_path)

    -- Go up to the parent directory
    curr_folder_path = vim.fn.fnamemodify(curr_folder_path, ":h")
  end
end



local filetype_to_run_table = {
  cmake = {
    run = GenerateCMake,
    debug = GenerateCMake,
  },
  cpp = {
    run = BuildAndRunCpp,
    debug = BuildAndDebugCpp,
  },
  python = {
    run = BuildAndRunPython,
    debug = BuildAndDebugPython,
  },
  javascript = {
    run = BuildAndRunJavaScript,
    debug = BuildAndDebugJavaScript,
  }
}


-- local last_run_filetype = ''

-- vim.keymap.set('n', '<leader>oo', function() print(search_folder_names_above_file({"src", "test"})) end)


-- TODO: When i have a breakpoint in tests\main.cpp
-- and i Start src\main.cpp it run the src without debugging as expected,
-- And when i have a breakpoint in src\main.cpp and
-- i Start tests\main.cpp it should run without debugging 
-- (because the breakpoint is in src\main.cpp)
-- But it runs the debugger
-- This makes no sense


local function Start(type, mode)
  -- local type = vim.bo[0].filetype
  type = type or vim.bo.filetype

  -- what = what or search_folder_names_above_file({"src", "test"}) or "src"

  -- TODO: Make the default value for mode based on if there are any breakpoints at all
  -- if not mode then
  --   if are_there_any_breakpoints_in_same_filetype_buffers() then
  --     mode = 'debug'
  --   else
  --     mode = 'run'
  --   end
  -- end

  -- last_run_filetype = type
  local conf = filetype_to_run_table

  -- NOTE:
  -- vim.cmd('messages clear')

  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  -- if vim.bo.modified then vim.cmd('w') end
  SaveUnavedFiletype(type)

  if not conf[type] or not conf[type][mode] then
    print("No ".. mode .. " configuration for file type " .. type)
    return
  end

  conf[type][mode]()
end


-- NOTE:
-- The keymaps may be silent by default
-- and thats why the on_error vim.cmd("messages")
-- doesnt open the messages


-- vim.keymap.set('n', '<F4>', function() Start("test") end )
-- vim.keymap.set('n', '<F5>', function() Start("src") end )
vim.keymap.set({'n', 'i'}, '<F4>', function() Start(nil, 'run') end, {silent = false})
vim.keymap.set({'n', 'i'}, '<F5>', function() Start(nil, 'debug') end, {silent = false}) -- Still need to provide debug key, in runtime error cases with unhandled exceptions or failed assertions
-- vim.keymap.set({'n', 'i'}, '<F6>', function() GenerateCMake() end, {silent = false})     -- Only for CPP


vim.api.nvim_create_user_command("Run", function() Start(nil, 'run') end, {})
vim.api.nvim_create_user_command("Test", function() Start(nil, 'run') end, {})
vim.api.nvim_create_user_command("Debug", function() Start(nil, 'debug') end, {})
vim.api.nvim_create_user_command("DebugSrc", function() Start(nil, 'debug') end, {})
vim.api.nvim_create_user_command("DebugTest", function() Start(nil, 'debug') end, {})

vim.api.nvim_create_user_command("Gen", function() GenerateCMake() end, {})
vim.api.nvim_create_user_command("Generate", function() GenerateCMake() end, {})



--[[
function BuildAndRun()
  vim.cmd('messages clear')
  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  -- vim.cmd('wa') -- Save all buffers
  --vim.cmd('w') -- Save only current buffers
  if vim.bo.modified then vim.cmd('w') end

  local filetype = vim.bo.filetype

  if filetype == 'cpp' then
    BuildAndRunCpp()
  elseif filetype == 'python' then
    BuildAndRunPython()
  else
    print("No Build and Run Configuration for File Type: " .. filetype)
  end
end


function BuildAndDebug()
  vim.cmd('messages clear')
  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  -- vim.cmd('wa') -- Save all buffers
  --vim.cmd('w') -- Save only current buffers
  if vim.bo.modified then vim.cmd('w') end

  local filetype = vim.bo.filetype

  if filetype == 'cpp' then
    BuildAndDebugCpp()
  elseif filetype == 'python' then
    BuildAndDebugPython()
  else
    print("No Build and Debug Configuration for File Type: " .. filetype)
  end
end


function BuildAndRunTest()
  vim.cmd('messages clear')
  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  -- vim.cmd('wa') -- Save all buffers
  --vim.cmd('w') -- Save only current buffers
  if vim.bo.modified then vim.cmd('w') end

  local filetype = vim.bo.filetype

  if filetype == 'cpp' then
    BuildAndRunCppTest()
  else
    print("No Test Configuration for File Type: " .. filetype)
  end
end


function BuildAndDebugTest()
  vim.cmd('messages clear')
  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  -- vim.cmd('wa') -- Save all buffers
  --vim.cmd('w') -- Save only current buffers
  if vim.bo.modified then vim.cmd('w') end

  local filetype = vim.bo.filetype

  if filetype == 'cpp' then
    BuildAndDebugCppTest()
  else
    print("No Debug of Test Configuration for File Type: " .. filetype)
  end
end


------------------------------------------------------------------------

-- Better to be only in normal mode, so diagnostics are shown, before building
vim.keymap.set('n', '<F5>', function() BuildAndRun() end)
vim.keymap.set('n', '<F6>', function() BuildAndRunTest() end)
vim.keymap.set('n', '<F7>', function() BuildAndDebug() end)
vim.keymap.set('n', '<F8>', function() BuildAndDebugTest() end)
vim.keymap.set('n', '<F9>', function() GenerateCMake() end)     -- Only for CPP
]]--



