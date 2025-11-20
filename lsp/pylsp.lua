return {
  cmd = {'pylsp'},
  root_markers = {'__main__.py', '.git'},
  filetypes = {'python'},
  settings = {
    python = {
      analysis = {
        autoSearchPaths = true,
        useLibraryCodeForTypes = true,
      },
    },
    -- Installed pylsp_inlay_hints extension
    -- Also installed pylsp-rope
    -- pylsp = {
    --   plugins = {
    --     pylsp_inlay_hints = {
    --       enabled = true,
    --     },
    --   },
    -- },
    -- pylsp = {
    --   plugins = {
    --     flake8 = {
    --       enabled = true,
    --       config = vim.fn.expand('~\\AppData\\Roaming\\flake8')
    --     },
    --     -- pylint = { enabled = true },
    --     -- pyflakes = { enabled = false },
    --     -- pycodestyle = { enabled = false },
    --   },
    -- },
  },
}
