-- F7 will Generate both Debug and Release Directories
-- F5 will Build the Debug directory and launch dap
-- F6 will Build the Release directory and launch the exe, without a debugger

-- All those functions expect that the cwd is set on the root directory of the project !

-- TODO: Fix the creation of the symlink of the new project

-- https://neovim.discourse.group/t/how-to-new-a-terminal-and-send-to-run-command-with-lua/4218
-- https://stackoverflow.com/questions/7597249/vim-script-how-to-execute-a-command-in-a-vim-function
-- https://neovim.discourse.group/t/how-to-new-a-terminal-and-send-to-run-command-with-lua/4218

local dap = require('dap')

-------------------------------------------------------------------------

function GenerateCMake()
  --vim.cmd('!rmdir /s /q "Debug"')
  --vim.cmd('!rmdir /s /q "Release"')
  vim.cmd('!rm -rf "Debug"')
  vim.cmd('!rm -rf "Release"')

  vim.cmd('!cmake -S . -B "Debug" -DCMAKE_BUILD_TYPE=Debug')
  vim.cmd('!cmake -S . -B "Release" -DCMAKE_BUILD_TYPE=Release')

  -- Create a symlink for clangd to work properly
  --https://www.howtogeek.com/16226/complete-guide-to-symbolic-links-symlinks-on-windows-or-linux/
  --vim.cmd('!mklink "compile_commands.json" "Debug\\compile_commands.json"')

  -- Creates a hardlink, so you dont need elevated permissions (no need to restart the terminal)
  vim.cmd('!rm compile_commands.json')
  vim.cmd('!mklink /H "compile_commands.json" "Debug\\compile_commands.json"')
end

function BuildAndRunCpp()
  --vim.cmd('!cmake --build Release --config Release')
  --vim.cmd('term .\\Release\\bin\\proj.exe')

  -- Run the build command and wait for it to finish
  --  local result = vim.fn.system('cmake --build "Release" --config Release')
  -- Check if the build was successful
  --  if vim.v.shell_error == 0 then
  -- If successful, run the executable in a terminal
  --    vim.cmd('term .\\Release\\bin\\proj.exe')
  --  else
  -- Print the error message if the build failed
  --    print("Build failed:\n" .. result)
  --  end

  -- Non-Blocking version of the above
  vim.fn.jobstart('cmake --build Release --config Release', {
    on_exit = function(_, exit_code)
      if exit_code == 0 then
        -- If successful, run the executable in a terminal
        vim.cmd('term .\\Release\\bin\\proj.exe')
      else
        -- Print an error message if the build failed
        print("Build failed with exit code: " .. exit_code)
      end
    end
  })
end

-- Some Trash very Buggy functions that ChatGayPT generated
-- But i will keep them as an example of how to use jobstart and the nvim floating windows creation
-- Using a Floating Window for Build Output
--[[function BuildAndRunCpp()
  local output = {}                                  -- Table to collect output lines
  local bufnr = vim.api.nvim_create_buf(false, true) -- Create a new scratch buffer
  local winid = vim.api.nvim_open_win(bufnr, true,
    { relative = 'editor', width = 80, height = 20, row = 5, col = 5, border = 'single' })

  -- Start the build job
  vim.fn.jobstart('cmake --build Release --config Release', {
    stdout_buffered = true,
    on_stdout = function(_, line)
      table.insert(output, line)                              -- Collect output lines
      vim.api.nvim_buf_set_lines(bufnr, 0, -1, false, output) -- Update the buffer with new output
      vim.api.nvim_win_set_cursor(winid, { #output, 0 })      -- Scroll to the last line
    end,
    on_exit = function(_, exit_code)
      if exit_code == 0 then
        -- If successful, run the executable in a terminal
        vim.cmd('term .\\Release\\bin\\proj.exe')
      else
        -- Print an error message if the build failed
        table.insert(output, "Build failed with exit code: " .. exit_code)
        vim.api.nvim_buf_set_lines(bufnr, 0, -1, false, output) -- Update the buffer one last time
      end
      -- Optionally, close the floating window after the build completes
      vim.api.nvim_win_close(winid, true)
    end,
  })
end
]] --

-- Using a Split Terminal for Live Output
--[[function BuildAndRunCpp()
  -- Create a new terminal window for the build output
  vim.cmd('new')                                         -- Open a new split window
  vim.cmd('term cmake --build Release --config Release') -- Run the build command in the terminal
  vim.cmd('resize 20')                                   -- Resize the terminal window for better visibility

  -- Set up an autocmd to handle the terminal close event
  local build_group = vim.api.nvim_create_augroup("BuildGroup", { clear = true }) -- Create a new autocmd group

  vim.api.nvim_create_autocmd("TermClose", {
    group = build_group,                  -- Assign the autocmd to the group
    pattern = "*",                        -- Match any terminal
    callback = function()
      local exit_code = vim.v.shell_error -- Get the exit code of the last command
      if exit_code == 0 then
        -- If the build was successful, run the executable in a new terminal
        vim.cmd('term .\\Release\\bin\\proj.exe')
      else
        -- Print an error message if the build failed
        print("Build failed with exit code: " .. exit_code)
      end

      -- Optionally, remove the autocmd after it runs to prevent spamming
      vim.api.nvim_del_augroup_by_id(build_group) -- Clean up the autocmd group
    end,
  })
end
]] --

function BuildAndDebugCpp()
  --vim.cmd('!cmake --build "Debug" --config Debug')
  --dap.continue()

  -- Run the build command and wait for it to finish
  --[[local result = vim.fn.system('cmake --build "Debug" --config Debug')
  -- Check if the build was successful
  if vim.v.shell_error == 0 then
    -- If successful, start the debugger
    dap.continue()
  else
    -- Print the error message if the build failed
    print("Build failed:\n" .. result)
  end]] --

  -- Non-Blocking version of the above
  vim.fn.jobstart('cmake --build "Debug" --config Debug', {
    on_exit = function(_, exit_code)
      if exit_code == 0 then
        -- If successful, debug the executable
        --vim.cmd('term .\\Debug\\bin\\proj.exe')
        dap.continue()

        -- Automatically print backtrace on breakpoints
        --dap.listeners.after.event_stopped["print_backtrace"] = function()
        --  dap.repl.run('bt') -- Print the call stack in the REPL
        --end

        --vim.cmd('term .\\Debug\\bin\\proj.exe')
      else
        -- Print an error message if the build failed
        print("Build failed with exit code: " .. exit_code)
      end
    end
  })
end

------------------------------------------------------------------------
-- There is no Building for python, but i want to keep the names consistent

function BuildAndRunPython()
  --local file = vim.fn.expand('%') -- Get the current file name
  --vim.cmd('!python ' .. file)
  vim.cmd('term python %')
end

function BuildAndDebugPython()
  dap.continue()
end

------------------------------------------------------------------------


function BuildAndRun()
  vim.cmd('wa') -- Save all buffers

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
  vim.cmd('wa') -- Save all buffers

  local filetype = vim.bo.filetype

  if filetype == 'cpp' then
    BuildAndDebugCpp()
  elseif filetype == 'python' then
    BuildAndDebugPython()
  else
    print("No Build and Debug Configuration for File Type: " .. filetype)
  end
end

------------------------------------------------------------------------

vim.keymap.set('n', '<F7>', function() GenerateCMake() end) -- Only for CPP
vim.keymap.set('n', '<F5>', function() BuildAndRun() end)
vim.keymap.set('n', '<F6>', function() BuildAndDebug() end)
