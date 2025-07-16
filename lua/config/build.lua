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


-- Install a Lua JSON parser like dkjson or lunajson.
local function get_cmake_executables()
  local json = require("dkjson") -- Use a JSON library like `dkjson` (install if not available)
  local executables = {}

  -- Locate `compile_commands.json` file (assuming it's in the build directory)
  local build_dir = vim.fn.input("Enter CMake build directory: ", vim.loop.cwd() .. "/", "file")
  local compile_commands_path = build_dir .. "/compile_commands.json"

  -- Check if the file exists
  if vim.fn.filereadable(compile_commands_path) == 0 then
    vim.notify("Could not find compile_commands.json in: " .. compile_commands_path, vim.log.levels.ERROR)
    return {}
  end

  -- Read the JSON file
  local file = io.open(compile_commands_path, "r")
  local content = file:read("*a")
  file:close()

  -- Parse JSON
  local parsed_commands, _, err = json.decode(content)
  if err then
    vim.notify("Error parsing compile_commands.json: " .. err, vim.log.levels.ERROR)
    return {}
  end

  -- Extract executables
  for _, command in ipairs(parsed_commands) do
    local output = command.output
    if output and output:match("%.exe$") then
      table.insert(executables, output)
    end
  end

  -- Display or return the list of executables
  vim.notify("Found executables:\n" .. table.concat(executables, "\n"), vim.log.levels.INFO)
  return executables
end



function GenerateCMake()
  local last_cwd = vim.fn.getcwd()
  local root = util.get_project_root()
  if last_cwd ~= root then vim.cmd('cd ' .. root) end

  vim.cmd('!rm -rf "Debug"')
  vim.cmd('!rm -rf "Release"')
  --vim.cmd('!rm -rf "Test"')

  vim.cmd('!cmake -S . -B "Debug" -DCMAKE_BUILD_TYPE=Debug')
  vim.cmd('!cmake -S . -B "Release" -DCMAKE_BUILD_TYPE=Release')
  --vim.cmd('!cmake -S . -B "Test" -DCMAKE_BUILD_TYPE=Test')

  -- Create a symlink for compile_commands.json, so language server protocols
  --  like clangd or ccls work properly
  -- Creates a hardlink, so you dont need elevated permissions (no need to restart the terminal)
  --https://www.howtogeek.com/16226/complete-guide-to-symbolic-links-symlinks-on-windows-or-linux/
  vim.cmd('!rm compile_commands.json')
  --vim.cmd('!mklink "compile_commands.json" "Debug\\compile_commands.json"')
  vim.cmd('!mklink /H "compile_commands.json" "Debug\\compile_commands.json"')

  if last_cwd ~= root then vim.cmd('cd ' .. last_cwd) end
end


-- TODO: WHY I CANT MAKE THIS TO OPEN MESSAGES AFTER ERROR
function BuildCmakeConfig(config_name, on_success_callback)
-- function BuildCmakeConfig(config_name, on_exit_callback)
  local last_cwd = vim.fn.getcwd()
  local root = util.get_project_root()
  if last_cwd ~= root then vim.cmd('cd ' .. root) end

  vim.fn.jobstart(
    'cmake --build "' .. config_name .. '" --config ' .. config_name, {
    stdout_buffered = true,
    stderr_buffered = true,
    on_stdout = function(_, data)
      if data and #data > 0 then
        -- print(table.concat(data, "\n"))
        vim.fn.setqflist({}, ' ', {
          title = 'CMake Build',
          lines = data,
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
        vim.cmd("copen | only") -- Open quickfix list
        -- require('quicker').toggle()
      else
        if on_success_callback then
          msg = "Build exited with code: " .. exit_code
          on_success_callback()
        end
      end
      -- on_exit_callback(exit_code)
      print(prepend_time(msg));


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


---------------------------------------------------------------------


function BuildAndRunCpp()
-- function BuildAndRunCpp(exit_code)
  -- if exit_code ~= 0 then return end
  print(prepend_time("Building and Running Cpp ..."))
  -- BuildCmakeConfig('Release', function() vim.cmd('12split | term .\\Release\\bin\\proj.exe') end)
  BuildCmakeConfig('Release', function()
    vim.cmd('10split | term ' .. util.get_project_root() .. '\\Release\\bin\\proj.exe')--12split
    vim.cmd("setlocal bufhidden=wipe") -- Close buffer when you switch away from it
    -- vim.cmd("setlocal nobuflisted") -- Exclude the buffer from the buffer list
    if vim.api.nvim_get_mode().mode ~= 't' then
      vim.api.nvim_input('i')
    end
  end)
end

function BuildAndDebugCpp()
-- function BuildAndDebugCpp(_)
  print(prepend_time("Building and Debugging Cpp ..."))
  BuildCmakeConfig('Debug', function() LaunchDapConfig('cpp', 'Launch Project') end)
end


function BuildAndRunCppTest()
-- function BuildAndRunCppTest(exit_code)
  -- if exit_code ~= 0 then return end
  print(prepend_time("Building and Running Cpp Tests ..."))
  BuildCmakeConfig('Release', function()
    vim.cmd('14split | term ' .. util.get_project_root() .. '\\Release\\bin\\proj_test.exe')
    vim.cmd("normal! G")
    -- if vim.api.nvim_get_mode().mode ~= 't' then
    --   vim.api.nvim_input('i')
    -- end
  end)
end

function BuildAndDebugCppTest()
-- function BuildAndDebugCppTest(_)
  print(prepend_time("Building and Debugging Cpp Tests ..."))
  BuildCmakeConfig('Debug', function() LaunchDapConfig('cpp', 'Launch Test') end)
end

--[[
function BuildAndRunCppTest()
  if BuildCmakeConfig('Test') then
    vim.cmd('10split | term .\\Test\\bin\\test_target.exe')
  end
end

function BuildAndDebugCppTest()
  if BuildCmakeConfig('Test') then
    --require('dap').continue()
    LaunchDapConfig('Launch Test')
  end
end
]]--


------------------------------------------------------------------------
-- There is no Building for python, but i want to keep the names consistent

--[[local function RunFile(command, on_exit)
  bufnr = bufnr or vim.api.nvim_get_current_buf()

  -- Function to capture the last line of terminal output
  vim.api.nvim_buf_attach(bufnr, false, {
    on_lines = function(_, _, _, _, _, _, _)
      local lines = vim.api.nvim_buf_get_lines(bufnr, 0, -1, false)
      local last_line = lines[#lines]

      -- Check if the last line contains the exit code
      if last_line:match("^%d+$") then
        return last_line
      end
    end,
  })
end]]--


function BuildAndRunPython()
  print(prepend_time("Running Python ..."))
  --local file = vim.fn.expand('%') -- Get the current file name
  --vim.cmd('!python ' .. file)
  vim.cmd('10split | term python %') -- 12split
  vim.cmd("setlocal bufhidden=wipe") -- Close buffer when you switch away from it
  -- vim.cmd("setlocal nobuflisted") -- Exclude the buffer from the buffer list
  --vim.cmd('term python %')

  if vim.api.nvim_get_mode().mode ~= 't' then
    vim.api.nvim_input('i')
  end

  -- print("Exit Code is: " .. get_exit_code())
end

function BuildAndDebugPython()
  print(prepend_time("Debugging Python ..."))
  -- require('dap').continue()
  LaunchDapConfig('python', 'file')
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
  cpp = {
    src = {
      run = BuildAndRunCpp,
      debug = BuildAndDebugCpp,
    },
    test = {
      run = BuildAndRunCppTest,
      debug = BuildAndDebugCppTest,
    }
  },
  python = {
    src = {
      run = BuildAndRunPython,
      debug = BuildAndDebugPython,
    },
  },
  javascript = {
    src = {
      run = BuildAndRunJavaScript,
      debug = BuildAndDebugJavaScript,
    }
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

local function Start(type, what, mode)
  -- local type = vim.bo[0].filetype
  type = type or vim.bo.filetype

  -- Why it defaults to test when i use it on a terminal ?
  what = what or search_folder_names_above_file({"src", "test"}) or "src"

  -- print(search_folder_names_above_file({what}))

  if not mode then
    -- https://vi.stackexchange.com/questions/10167/is-there-a-way-that-i-can-identify-which-window-is-a-terminal-after-calling-the
    -- if type == "" then -- It may be a terminal window
    --   mode = 'debug'
    --   type = last_run_filetype
    --
    --   -- Exit the Terminal Wondow
    --   vim.api.nvim_input("i")
    --   vim.api.nvim_input("<CR>")
    --
    -- else
    -- NOTE: Only checks for breakpoints in same type buffers and
    --        same what(src or test)
      if are_there_any_breakpoints_in_same_filetype_buffers(
        function (id)
          return search_folder_names_above_file({what}, id)
        end
      ) then
        mode = 'debug'
      else
        mode = 'run'
      end
    -- end
  end

  -- last_run_filetype = type
  local conf = filetype_to_run_table

  vim.cmd('messages clear')
  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  if vim.bo.modified then vim.cmd('w') end

  if not conf[type] or not conf[type][what] or not conf[type][what][mode] then
    print("No ".. mode .. " " .. what  .. " configuration for file type " .. type)
    return
  end

  conf[type][what][mode]()
end


-- NOTE:
-- The keymaps may be silent by default
-- and thats why the on_error vim.cmd("messages")
-- doesnt open the messages


-- vim.keymap.set('n', '<F4>', function() Start("test") end )
-- vim.keymap.set('n', '<F5>', function() Start("src") end )
vim.keymap.set({'n', 'i'}, '<F4>', function() Start(nil, nil, 'run') end, {silent = false})
vim.keymap.set({'n', 'i'}, '<F5>', function() Start(nil, nil, 'debug') end, {silent = false}) -- Still need to provide debug key, in runtime error cases with unhandled exceptions or failed assertions
vim.keymap.set({'n', 'i'}, '<F6>', function() GenerateCMake() end, {silent = false})     -- Only for CPP


vim.api.nvim_create_user_command("Run", function() Start(nil, 'src', 'run') end, {})
vim.api.nvim_create_user_command("Test", function() Start(nil, 'test', 'run') end, {})
vim.api.nvim_create_user_command("Debug", function() Start(nil, nil, 'debug') end, {})
vim.api.nvim_create_user_command("DebugSrc", function() Start(nil, 'src', 'debug') end, {})
vim.api.nvim_create_user_command("DebugTest", function() Start(nil, 'test', 'debug') end, {})

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



