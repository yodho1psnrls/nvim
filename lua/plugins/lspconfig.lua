-- https://github.com/neovim/nvim-lspconfig/blob/master/doc/server_configurations.md#ccls

return {

  --https://github.com/Issafalcon/lsp-overloads.nvim
  { "Issafalcon/lsp-overloads.nvim" },

  {
    "neovim/nvim-lspconfig",

    config = function()
      local M = {}

      -- Custom on_attach function for key mappings
      M.on_attach = function(_, bufnr)
        local map = vim.keymap.set
        local function opts(desc)
          return { buffer = bufnr, desc = "LSP " .. desc }
        end

        -- Keymaps
        map("n", "gD", vim.lsp.buf.declaration, opts "Go to declaration")
        map("n", "gd", vim.lsp.buf.definition, opts "Go to definition")
        map("n", "gi", vim.lsp.buf.implementation, opts "Go to implementation")
        map("n", "<leader>sh", vim.lsp.buf.signature_help, opts "Show signature help")
        map("n", "<leader>wa", vim.lsp.buf.add_workspace_folder, opts "Add workspace folder")
        map("n", "<leader>wr", vim.lsp.buf.remove_workspace_folder, opts "Remove workspace folder")

        map("n", "<leader>wl", function()
          print(vim.inspect(vim.lsp.buf.list_workspace_folders()))
        end, opts "List workspace folders")

        map("n", "<leader>D", vim.lsp.buf.type_definition, opts "Go to type definition")

        map("n", "<leader>ra", function()
          require "nvchad.lsp.renamer" ()
        end, opts "NvRenamer")

        map({ "n", "v" }, "<leader>ca", vim.lsp.buf.code_action, opts "Code action")
        map("n", "gr", vim.lsp.buf.references, opts "Show references")
      end

      -- Disable semantic tokens
      M.on_init = function(client, _)
        if client.supports_method "textDocument/semanticTokens" then
          client.server_capabilities.semanticTokensProvider = nil
        end
      end

      -- Capabilities setup
      M.capabilities = vim.lsp.protocol.make_client_capabilities()
      M.capabilities.textDocument.completion.completionItem = {
        documentationFormat = { "markdown", "plaintext" },
        snippetSupport = true,
        preselectSupport = true,
        insertReplaceSupport = true,
        labelDetailsSupport = true,
        deprecatedSupport = true,
        commitCharactersSupport = true,
        tagSupport = { valueSet = { 1 } },
        resolveSupport = {
          properties = {
            "documentation",
            "detail",
            "additionalTextEdits",
          },
        },
      }

      ----------------------------------------------------------------------

      -- Setup the language servers
      local lspconfig = require("lspconfig")
      local util = require("lspconfig.util")


      -- Custom handler for signature help (To make the windows non-focusable)
      --[[
      vim.lsp.handlers["textDocument/signatureHelp"] = vim.lsp.with(
        vim.lsp.handlers.signature_help, {
          -- Make the floating window non-focusable
          focusable = false,
          -- You can customize the border as well
          border = "rounded",
        }
      )
      ]] --

      -- pylsp for Python
      lspconfig.pylsp.setup({
        cmd = { 'pylsp' },
        filetypes = { 'python' },
        root_dir = function(fname)
          return lspconfig.util.find_git_ancestor(fname) or lspconfig.util.path.dirname(fname)
        end,
        settings = {
          python = {
            analysis = {
              autoSearchPaths = true,
              useLibraryCodeForTypes = true,
            },
          },
        },
      })


      -- https://github.com/neovim/nvim-lspconfig/blob/master/doc/server_configurations.md#clangd
      --lspconfig.clangd.setup{}
      lspconfig.clangd.setup {
        -- make clang to use cache indexing (as ccls)
        cmd = { "clangd", "--background-index" },
        -- It will save the cache on a default path
        -- which is ~/AppData/Local/clangd/index
        -- If you want to specify another path
        -- you need to set this environmental variable:
        -- set CLANGD_INDEX_STORAGE_PATH=C:\path\to\custom\cache (cmd)
        -- $env:CLANGD_INDEX_STORAGE_PATH = "C:\path\to\custom\cache" (powershell)

        root_dir = function(fname)
          return util.find_git_ancestor(fname) or vim.fn.getcwd()
        end,

        --root_dir = function(fname)
        --  return util.root_pattern('compile_commands.json', '.git')(fname)
        --end,


      }


      --[[
      lspconfig.ccls.setup {
        cmd = { 'ccls' },
        filetypes = { 'c', 'cpp', 'objective-c', 'objective-cpp' },
        root_dir = util.root_pattern('compile_commands.json', '.ccls', '.git'),
        init_options = {
          compilationDatabaseDirectory = ".",
        },
      }

      --[[
      lspconfig.ccls.setup {

        cmd = { 'ccls' }, -- Ensure 'ccls' is in your PATH
        filetypes = { 'c', 'cpp', 'objective-c', 'objective-cpp' },
        offset_encoding = "utf-8",
        root_dir = function(fname)
          return util.root_pattern('compile_commands.json', '.ccls', '.git')(fname)
        end,
        --root_dir = function(fname)
        --  return lspconfig.util.find_git_ancestor(fname) or lspconfig.util.path.dirname(fname)
        --end,
        single_file_support = true,

        init_options = {
          --compilationDatabaseDirectory = "build",
          --compilationDatabaseDirectory = "Debug",
          compilationDatabaseDirectory = ".",
          index = {
            threads = 4,      -- Number of indexing threads, adjust according to your CPU cores
            onChange = true,  -- Reindex on file changes
            --initialBlacklist = { "Debug/*" }, -- Avoid indexing generated files (e.g., build directories)
            multiVersion = 1, -- Enable multiple versions of a symbol
          },
          --clang = {
          --  excludeArgs = { "-frounding-math"} ;
          --};
          cache = {
            directory = ".ccls-cache", -- Cache directory (can be customized)
            retainInMemory = 2,        -- Cache levels (0: disabled, 1: per file, 2: global)
          },
          highlight = {
            lsRanges = true, -- Highlight regions for LSP semantic tokens
          },
          completion = {
            filterAndSort = true, -- Enable sorting of completion results
            placeholder = true,   -- Show parameter placeholders in completion
            detailedLabel = true,
            maxNum = 50
          },
          diagnostics = {
            --onChange = 1000,            -- Update diagnostics after delay (in ms)
            --onOpen = true,              -- Show diagnostics when a file is opened
            onOpen = 0,      -- Disable diagnostics on file open
            onSave = 1,
            enable = true,   -- Enable diagnostics (errors, warnings)
            onChange = true, -- Re-run diagnostics on file changes
          },
          codeLens = {
            localVariables = true, -- Show code lens for local variables
          },
          format = {
            enable = false, -- Disable formatting, let another tool like clang-format handle it
          },
          lint = {
            onChange = false, -- Enable linting during text change (disable because you use nvim-lint)
          },
          hover = {
            xrefs = true, -- Enable cross-references in hover information
          },
        }

      }
]] --
      -------------------------------------------------------------------------

      lspconfig.lua_ls.setup({
        cmd = { 'lua-language-server' }, -- Make sure this path is correct
        filetypes = { 'lua' },
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
      })

      -- Repeat `lspconfig.` for other language servers you want to configure
    end,
  },
}
