-- https://github.com/neovim/nvim-lspconfig/blob/master/doc/server_configurations.md#ccls

-- TODO: Fix the Code Action Feature
--		and some others that dont work properly or have their keymaps overriten like Go To Implementation
--    Most importantly: Generate definition out of declaration

-- TODO: Disable the Unused Include Directories Warning 

-- NOTE:
-- ClangD loads std library from MSVC, but Clang loads it from MSYS2 MinGW's libstdc++
-- https://github.com/clangd/clangd/issues/2224
-- https://clangd.llvm.org/guides/system-headers#query-driver

return {

  {
    "neovim/nvim-lspconfig",

    -- These are not dependencies, but we want them to be lazy loaded with lspconfig
    dependencies = {
      "https://git.sr.ht/~whynothugo/lsp_lines.nvim",
      -- "maan2003/lsp_lines.nvim",
    },

    config = function()

      -- Available Options: off, error, warn, info, debug, trace
      vim.lsp.set_log_level("ERROR") -- OFF | ERROR | WARN | INFO | DEBUG
      -- vim.lsp.set_log_level("trace") -- Verbose (For debugging code-lldb)

      -- TODO: Make the virtual_lines to show only ERRORs
      -- and the virtual_text, signs and rest make them to show
      -- to other stuff

      vim.diagnostic.config({

        virtual_text = false,
        --virtual_text = {
        --  severity = { min = vim.diagnostic.severity.INFO}, -- Warnings Enabled
        --},

        virtual_lines = true,
        --[[virtual_lines = {
          only_current_line = false,    -- true
          highlight_whole_line = false, -- false
          --mode = "n",
          -- severity = { min = vim.diagnostic.severity.ERROR }

          -- format = function (diagnostic)
          --   -- return string.format("[%s] %s", vim.diagnostic.severity[diagnostic.severity], diagnostic.message)
          --   return "Slqlqlq " .. diagnostic.message
          -- end,
        },]]--

        signs = true, -- true
        underline = true,
        update_in_insert = true,
        severity_sort = true,

				-- float = {
				-- 	border = 'rounded',
				-- 	-- source = true,
				-- }

        -- severity = {
          -- min = vim.diagnostic.severity.WARN, -- HINT | INFO | WARN | ERROR
          -- min = nil,
        -- },

        -- format = function (_) return vim.diagnostic.message end,

        --[[format = function(diagnostic)
          if type(diagnostic) ~= "table" or type(diagnostic.message) ~= "string" then
            return "" 
          end
          return diagnostic.message:gsub("^%w+: ", "") -- Remove diagnostic.code
        end,]]--
      })


      -- vim.fn.sign_define("DiagnosticSignWarn", { text = "⚠️", texthl = "DiagnosticWarn" })  -- Warning icon
      -- vim.fn.sign_define("DiagnosticSignError", { text = "❌", texthl = "DiagnosticError" })  -- Error icon
      -- vim.fn.sign_define("DiagnosticSignHint", { text = "💡", texthl = "DiagnosticHint" })   -- Hint icon
      -- vim.fn.sign_define("DiagnosticSignInfo", { text = "ℹ️", texthl = "DiagnosticInfo" })  -- Info icon
      vim.fn.sign_define("DiagnosticSignWarn", { text = "W", texthl = "DiagnosticWarn" })  -- Warning icon
      vim.fn.sign_define("DiagnosticSignError", { text = "E", texthl = "DiagnosticError" })  -- Error icon
      vim.fn.sign_define("DiagnosticSignHint", { text = "H", texthl = "DiagnosticHint" })   -- Hint icon
      vim.fn.sign_define("DiagnosticSignInfo", { text = "I", texthl = "DiagnosticInfo" })  -- Info icon


      local function ToggleWarnings()
        local diag_conf = vim.diagnostic.config() -- Get the current config
        if diag_conf == nil then return end

        -- if diag_conf.virtual_text.severity.min == nil then
        if diag_conf.virtual_text.severity.min == vim.diagnostic.severity.WARN then
          -- diag_conf.virtual_text.severity.min = vim.diagnostic.severity.ERROR
          diag_conf.virtual_text.severity.min = vim.diagnostic.severity.ERROR
          -- vim.fn.sign_define("DiagnosticSignWarn", { text = "", texthl = "DiagnosticWarn" })  -- Warning icon
          print("LSP Warnings Disabled")
        -- elseif diag_conf.virtual_text.severity.min == vim.diagnostic.severity.ERROR then
        elseif diag_conf.virtual_text.severity.min == vim.diagnostic.severity.ERROR then
          diag_conf.virtual_text.severity.min = vim.diagnostic.severity.WARN
          -- vim.fn.sign_define("DiagnosticSignWarn", { text = "W", texthl = "DiagnosticWarn" })  -- Warning icon
          print("LSP Warnings Enabled")
        end

        vim.diagnostic.config(diag_conf) -- Apply the modified config !!! (because the config value is immutable)
      end

      vim.keymap.set('n', '<leader>wt', function() ToggleWarnings() end,
        { noremap = true, silent = true, desc = 'Warnings Toggle' })


      local function ToggleSigns()
        -- LSP Diagnostic Signs Only
        --[[local diag_conf = vim.diagnostic.config()
        if not diag_conf then return end
        diag_conf.signs = not diag_conf.signs
        vim.diagnostic.config(diag_conf)
        print("Signs Toggled")]]--
        if vim.o.signcolumn == "auto" then
          vim.o.signcolumn = "no"
          print ("Sign column hidden")
        elseif vim.o.signcolumn == "no" then
          vim.o.signcolumn = "auto"
          print ("Sign column visible")
        end
      end

      vim.keymap.set('n', '<leader>sg', function() ToggleSigns() end,
        { noremap = true, silent = true, desc = 'Signs Toggle' })


      -- BUILT IN NEOVIM DIAGNOSTICS WINDOW ------------------------------------------------------------
      -- https://www.reddit.com/r/neovim/comments/11axh2p/how_to_toggle_openclose_floating_lsp_diagnostic/
      -- https://neovim.io/doc/user/diagnostic.html
      -- https://neovim.io/doc/user/quickfix.html#quickfix

      vim.keymap.set('n', '<leader>ds', vim.diagnostic.setloclist, { desc = 'Open diagno[S]tic list' })
      --vim.keymap.set('n', '<leader>dq', vim.diagnostic.setqflist, { desc = 'Open diagnostic [Q]uickfix list' })
      vim.keymap.set('n', '<leader>da', vim.diagnostic.setqflist, { desc = 'Open [A]ll diagnostic lists' })

      --vim.keymap.set('n', '<S-CR>', vim.diagnostic.open_float, { desc = 'Go to diagnostic'})
      vim.keymap.set('n', '<leader>dg', vim.lsp.util.jump_to_location, { desc = '[G]o to diagnostic'})

      -- https://www.reddit.com/r/neovim/comments/18qh3mg/how_to_show_entire_diagnostic_message/
      --vim.keymap.set('n', '<leader>df', vim.diagnostic.open_float,
      --  { desc = 'Open floating diagnostic window, while the cursor is on a line with diagnostic message'})

      -- :lua vim.diagnostic.open_float() is the command that executes when you normally press Enter
      --  in the diagnostics window, but since we remapped Enter in normal mode, we need to make it
      --  apply the logic for it to execute that
      --[[vim.api.nvim_create_autocmd("FileType", {
        pattern = "vim.diagnostic.float",
        callback = function()
          vim.keymap.set("n", "<CR>", function()
            local __opts = { float = true }
            vim.diagnostic.goto_next(__opts) -- Automatically jumps to the diagnostic window really-simeilfe
          end)
        end,
      })]]--

      -- Function to jump to the next diagnostic
      vim.keymap.set('n', '<leader>dn', vim.diagnostic.goto_next,
        { desc = 'Go to [N]ext diagnostic', noremap = true, silent = true})

      -- Function to jump to the previous diagnostic
      vim.keymap.set('n', '<leader>dp', vim.diagnostic.goto_prev,
        { desc = 'Go to [P]revious diagnostic', noremap = true, silent = true })
      ---------------------------------------------------------------------------------


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

        -- @todo: Do this without NVChad so, you can use the rename functionality
        --map("n", "<leader>ra", function()
        --  require "nvchad.lsp.renamer" ()
        --end, opts "NvRenamer")

        map("n", "<leader>ra", vim.lsp.buf.rename, opts "LSP Rename")

        -- THIS IS THE ACTUAL THING THAT GENERATES YOU FUNCTION OR METHOD DEFINITIONS BASED ON DECLARATIONS !!!
        -- THIS ALSO DOES FIXES AVAILABLE CODE ACTIONS
        map({ "n", "v" }, "<leader>ca", vim.lsp.buf.code_action, opts "Code action")
        -- Try to keymap it as it is in Visual Studio (Ctrl - . - Enter(enter when the window shows up))
        --map({ "n", "v" }, "C-.", vim.lsp.buf.code_action, opts "Code action")

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

      -- Because of ufo.nvim, we define some folding options here
      M.capabilities.textDocument.foldingRange = {
        dynamicRegistration = false,
        lineFoldingOnly = true
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
        on_attach = M.on_attach,
        on_init = M.on_init,
        capabilities = M.capabilities,

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
          -- Installed pylsp_inlay_hints extension
          -- Also installed pylsp-rope
          -- pylsp = {
          --   plugins = {
          --     pylsp_inlay_hints = {
          --       enabled = true,
          --     },
          --   },
          -- },
        },
      })

      -- NOTE: inlay-hints support for clangd
      -- require("lspconfig").clangd.setup({
      --   cmd = { "clangd", "--inlay-hints", "--header-insertion=never" },
      --   on_attach = function(client, bufnr)
      --     require("lsp-inlayhints").on_attach(client, bufnr)
      --   end,
      -- })

      -- https://github.com/neovim/nvim-lspconfig/blob/master/doc/server_configurations.md#clangd
      --lspconfig.clangd.setup{}
      lspconfig.clangd.setup {
        on_attach = M.on_attach,
        on_init = M.on_init,
        capabilities = M.capabilities,

        -- The background-index tag makes clang to use cache indexing (as ccls)
        --cmd = { "clangd", "--background-index", "--query-driver=D:/Program Files/MSYS2/mingw64/bin/clang++.exe" },
        -- pacman -S mingw-w64-x86_64-clang-tools-extra (To install clang-tidy(static analysis tool, that compliments the lsp))
        -- Other tools from clang-tools-extra that you need to consider:
        --  Clang-Format, Clang-Apply-Replacements, Clang-Check, Clang-Scan-Dependencies, Clang-Syntax
        --  Extra clang-tidy checks: modernize-*, readability-*, performance-*
        --  Run this below in the bash to see full list of available checks in the modernize, readability, and performance categories
        --  clang-tidy -list-checks | grep "modernize\|readability\|performance"
        cmd = {
          "clangd",
          "--background-index", -- cache
          '--query-driver=**/clang++.exe', -- glob pattern for the compiler, so it loads its own std library implementation
          -- '--query-driver=D:\\Program Files\\MSYS2\\mingw64\\bin\\clang++.exe',

          "--clang-tidy",
          -- Enable all performance-related checks for clang-tidy
          --"--checks=performance-*",     
          -- Or you can enable performance related checks one by one
          --'--checks="*,clang-analyzer-optin.performance.GCDAntipattern,clang-analyzer-optin.performance.Padding"',

          "--completion-style=detailed", -- To support Inlay Hints

					-- Disable Unused Header Warning, But for some reason, it has no effect
					-- "--header-insertion=never",

					-- Disable all clang-tidy checks, enabling only this specific check, which warns about deprecated c headers
					-- "--clang-tidy-checks=-*,modernize-deprecated-headers",

          --"--log=verbose", -- For Debugging
          -- "--header-insertion=never", -- (iwyu: default | never) automatically inserting #include directives for missing headers when completing symbols
          -- "--completion-style=detailed", -- (detailed: default(displays function signatures) | bundled(less detailed, only displays function, variable, etc))

          --"-Wunused-include-directive", -- Dont show unused headed warnings
          --"--disable=clang-diagnostic-unused-include-directive", -- Dont show unused headed warnings
          --"--disable-tidy-checks=llvm-header-guard",

        },

--        cmd = { "clangd", "--background-index", "--log=verbose" }, -- For Debugging
--        cmd = { "clangd", "--background-index" },

        -- It will save the cache on a default path
        -- which is ~/AppData/Local/clangd/index
        -- If you want to specify another path
        -- you need to set this environmental variable:
        -- set CLANGD_INDEX_STORAGE_PATH=C:\path\to\custom\cache (cmd)
        -- $env:CLANGD_INDEX_STORAGE_PATH = "C:\path\to\custom\cache" (powershell)

        root_dir = function(fname)
          return util.find_git_ancestor(fname) or vim.fn.getcwd() -- Deprecated
          -- return vim.fs.dirname(vim.fs.find('.git', { path = fname, upward = true })[1]) or vim.fn.getcwd()
				end,

        --root_dir = function(fname)
        --  return util.root_pattern('compile_commands.json', '.git')(fname)
        --end,

        --flags = {
        --  debounce_text_changes = 150, -- 0 by default (in ms) -- How often the lsp triggers
        --},

      }


			-- JavaScript/TypeScript
			-- lspconfig.tsserver.setup {}
			lspconfig.ts_ls.setup {}

			-- CSS
			lspconfig.cssls.setup {}


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
        on_attach = M.on_attach,
        on_init = M.on_init,
        capabilities = M.capabilities,

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
