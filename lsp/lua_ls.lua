return {
  cmd = { 'lua-language-server' }, -- Make sure this path is correct
  filetypes = { 'lua' },
  rootmarkers = {},

  -- Specific settings to send to the server. The schema for this is
  -- defined by the server. For example the schema for lua-language-server
  -- can be found here https://raw.githubusercontent.com/LuaLS/vscode-lua/master/setting/schema.json
  settings = {
    Lua = {
      runtime = {
        version = 'LuaJIT', -- Set Lua version, adjust this if needed
      },
      diagnostics = {
        globals = { 'vim' }, -- Recognize the 'vim' global
      },
      workspace = {
        library = vim.api.nvim_get_runtime_file('', true), -- Make LSP aware of Neovim runtime files
      },
      telemetry = {
        enable = false, -- Disable telemetry if you don't want to send data
      },
    },
  },
}

