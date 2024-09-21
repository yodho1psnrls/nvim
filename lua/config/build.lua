-- F7 will Generate both Debug and Release Directories
-- F5 will Build the Debug directory and launch dap
-- F6 will Build the Release directory and launch the exe, without a debugger

-- All those functions expect that the cwd is set on the root directory of the project !

function GenerateCMake()
  os.execute('rmdir /s /q "Debug"')
  os.execute('rmdir /s /q "Release"')
  --os.execute('rm -rf "Debug"')
  --os.execute('rm -rf "Release"')

  os.execute('cmake -S . -B "Debug" -DCMAKE_BUILD_TYPE=Debug')
  os.execute('cmake -S . -B "Release" -DCMAKE_BUILD_TYPE=Release')
  --os.execute('cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug')
  --os.execute('cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release')

  -- Create a symlink for clangd to work properly
  --https://www.howtogeek.com/16226/complete-guide-to-symbolic-links-symlinks-on-windows-or-linux/
  os.execute('mklink "Debug\\compile_commands.json" "compile_commands.json"')
end

function BuildAndRunRelease()
  os.execute('cmake --build "Release" --config Release')
  --os.execute('cmake --build build/Release --config Release')
  local is_correct = os.execute('".\\Release\\bin\\proj.exe"')

  if not is_correct then
    print("Failed to run Release executable")
  end
end

function BuildAndRunDebug()
  os.execute('cmake --build "Debug" --config Debug')
  --os.execute('cmake --build build/Debug --config Debug')
  require('dap').continue()
end

vim.keymap.set('n', '<F7>', function() GenerateCMake() end)
vim.keymap.set('n', '<F6>', function() BuildAndRunRelease() end)
vim.keymap.set('n', '<F5>', function() BuildAndRunDebug() end)
