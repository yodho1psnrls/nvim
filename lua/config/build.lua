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

-------------------------------------------------------------------------

function GenerateCMake()
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
end


function BuildCmakeConfig(config_name, on_success_callback)
  vim.fn.jobstart('cmake --build "' .. config_name .. '" --config ' .. config_name, {
  stdout_buffered = true,
  stderr_buffered = true,
  on_stdout = function(_, data)
    if data and #data > 0 then
      -- Print standard output messages
      print(table.concat(data, "\n"))
    end
  end,

  on_stderr = function(_, data)
    if data and #data > 0 then
      -- Print error messages from stderr
      print("Error during build:\n" .. table.concat(data, "\n"))
    end
  end,

  on_exit = function(_, exit_code)
    if exit_code ~= 0 then
      -- If the build failed, print the exit code and show all messages
      print("Build failed with exit code: " .. exit_code)
      vim.cmd('messages')
      --vim.defer_fn(function() vim.cmd('messages') end, 100) -- same as above, but with delay
    else
      if on_success_callback then
        on_success_callback()
      end
    end
  end,
  })
end

function LaunchDapConfig(config_name)
  local dap = require('dap')
  for _, config in ipairs(dap.configurations.cpp) do
    if config.name == config_name then
      dap.run(config)
      return
    end
  end
  print("Configuration not found: " .. config_name)
end


---------------------------------------------------------------------


function BuildAndRunCpp() --                      10
  BuildCmakeConfig('Release', function() vim.cmd('12split | term .\\Release\\bin\\proj.exe') end)
end

function BuildAndDebugCpp()
  BuildCmakeConfig('Debug', function() LaunchDapConfig('Launch Project') end)
end


function BuildAndRunCppTest()
  BuildCmakeConfig('Release', function() vim.cmd('16split | term .\\Release\\bin\\proj_test.exe') end)
end

function BuildAndDebugCppTest()
  BuildCmakeConfig('Debug', function() LaunchDapConfig('Launch Test') end)
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

function BuildAndRunPython()
  --local file = vim.fn.expand('%') -- Get the current file name
  --vim.cmd('!python ' .. file)
   vim.cmd('10split | term python %')
  --vim.cmd('term python %')
end

function BuildAndDebugPython()
  require('dap').continue()
end

------------------------------------------------------------------------


function BuildAndRun()
  vim.cmd('messages clear')
  -- vim.cmd('echo string(repeat("=", winwidth(0)))')
  -- vim.cmd('wa') -- Save all buffers
  vim.cmd('w') -- Save only current buffers

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
  vim.cmd('w') -- Save only current buffers

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
  vim.cmd('w') -- Save only current buffers

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
  vim.cmd('w') -- Save only current buffers

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

