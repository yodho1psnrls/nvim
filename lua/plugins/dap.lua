-- https://www.youtube.com/watch?v=MunSY_EXEiI

-- https://github.com/mfussenegger/nvim-dap/wiki/Debug-Adapter-installation#ccrust-via-codelldb
-- https://github.com/vadimcn/codelldb/releases
-- https://github.com/julianolf/nvim-dap-lldb
-- consider using nvim-nio if your debugger makes neovim to freeze
--  it basically makes some external tasks to run in parralel,
--  so neovim doesnt freeze

-- NOTE: Plugins such as nvim-dap-python or nvim-dap-lldb are plugins
-- that only configure your nvim-dap

-- TODO: Configure nvim-dap-ui to show the Call-Stack and the Stack
--  those are the most important features i use from a debugger
-- Also try to automate the debugger path and the build path,
--  so you can debug or just build and run the project just by one keybind
-- And also add some icons for the breakpoints to look prettier

return {

  {
    "mfussenegger/nvim-dap",
    config = function()
      vim.keymap.set('n', '<F5>', function() require('dap').continue() end)
      vim.keymap.set('n', '<F10>', function() require('dap').step_over() end)
      vim.keymap.set('n', '<F11>', function() require('dap').step_into() end)
      vim.keymap.set('n', '<F12>', function() require('dap').step_out() end)
      vim.keymap.set('n', '<Leader>b', function() require('dap').toggle_breakpoint() end)
      vim.keymap.set('n', '<Leader>B', function() require('dap').set_breakpoint() end)
      vim.keymap.set('n', '<Leader>lp',
        function() require('dap').set_breakpoint(nil, nil, vim.fn.input('Log point message: ')) end)
      vim.keymap.set('n', '<Leader>dr', function() require('dap').repl.open() end)
      vim.keymap.set('n', '<Leader>dl', function() require('dap').run_last() end)
      vim.keymap.set({ 'n', 'v' }, '<Leader>dh', function()
        require('dap.ui.widgets').hover()
      end)
      vim.keymap.set({ 'n', 'v' }, '<Leader>dp', function()
        require('dap.ui.widgets').preview()
      end)
      vim.keymap.set('n', '<Leader>df', function()
        local widgets = require('dap.ui.widgets')
        widgets.centered_float(widgets.frames)
      end)
      vim.keymap.set('n', '<Leader>ds', function()
        local widgets = require('dap.ui.widgets')
        widgets.centered_float(widgets.scopes)
      end)



      ------------------------------------------------------------
      -- https://github.com/mfussenegger/nvim-dap/wiki/C-C---Rust-(via--codelldb)
      local dap = require('dap')
      dap.adapters.codelldb = {
        type = 'server',
        port = "${port}",
        executable = {
          --command = '/path/to/codelldb/adapter/codelldb',
          command = 'codelldb',
          args = { "--port", "${port}" },

          -- On windows you may have to uncomment this:
          detached = false,
        }
      }

      dap.configurations.cpp = {
        {
          name = "Launch file",
          type = "codelldb",
          request = "launch",
          program = function()
            return vim.fn.input('Path to executable: ', vim.fn.getcwd() .. '/', 'file')
          end,
          cwd = '${workspaceFolder}',
          stopOnEntry = false,   -- Pause the debugger at the begin point of the program (like a breakpoint at the start)

          args = {},             -- Arguments to pass to the executable program
          runInTerminal = false, -- You can customize this depending on your project

          terminal = 'integrated',
        },
      }

      dap.configurations.c = dap.configurations.cpp

      ------------------------------------------------------------
    end,
  },

  {
    -- A plugin that auto-saves the breakpoints and some additional features
    -- https://www.reddit.com/r/neovim/comments/177zwub/daphelpernvim_small_helper_plugin_for_nvimdap_and/
    'daic0r/dap-helper.nvim',
    dependencies = { "rcarriga/nvim-dap-ui", "mfussenegger/nvim-dap" },
    config = function()
      require("dap-helper").setup()
    end,
  },

  {
    "nvim-neotest/nvim-nio", -- dependancy for nvim-dap-ui
  },

  {
    -- integration with telescope, so you can browse
    -- breakpoints and other debug related stuff
    "nvim-telescope/telescope-dap.nvim",
    dependencies = {
      "mfussenegger/nvim-dap",
      "nvim-telescope/telescope.nvim",
    },
    config = function()
      require('telescope').load_extension('dap')
    end,
  },

  {
    "rcarriga/nvim-dap-ui",
    dependencies = {
      "mfussenegger/nvim-dap",
      "nvim-neotest/nvim-nio"
    },
    config = function()
      local dap, dapui = require("dap"), require("dapui")

      dapui.setup()

      --[[
      dapui.setup {
        -- Set icons to characters that are more likely to work in every terminal.
        icons = { expanded = '▾', collapsed = '▸', current_frame = '*' },

        controls = {
          enabled = true,   -- Enable controls
          element = "repl", -- Controls will target the REPL window
          icons = {
            pause = '⏸',
            play = '▶',
            step_into = '⏎',
            step_over = '⏭',
            step_out = '⏮',
            step_back = 'b',
            run_last = '▶▶',
            terminate = '⏹',
            disconnect = '⏏',
          },
        },

        -- Add required fields with either custom or default values
        mappings = {
          -- Custom key mappings for the UI
          expand = { "<CR>", "<2-LeftMouse>" },
          open = "o",
          remove = "d",
          edit = "e",
          repl = "r",
          toggle = "t",
        },

        element_mappings = {}, -- Default: empty table

        expand_lines = true,   -- Default: expand the number of lines in stack trace elements

        force_buffers = true,  -- Force creating buffers for DAP elements

        layouts = {
          {
            elements = {
              { id = "scopes",      size = 0.25 },
              { id = "breakpoints", size = 0.25 },
              { id = "stacks",      size = 0.25 },
              { id = "watches",     size = 0.25 },
            },
            size = 40,
            position = "left", -- Can be "left", "right", "top", or "bottom"
          },
          {
            elements = {
              { id = "repl",    size = 0.5 },
              { id = "console", size = 0.5 },
            },
            size = 10,
            position = "bottom", -- Can be "left", "right", "top", or "bottom"
          },
        },

        floating = {
          max_height = nil,  -- These can be a number or function, defaults to nil
          max_width = nil,   -- Floating window max width
          border = "single", -- Border style, "single", "double" or "rounded"
          mappings = {
            close = { "q", "<Esc>" },
          },
        },

        render = {
          indent = 1,            -- How deep indenting should be for hierarchical structures
          max_type_length = nil, -- Limit length of type displayed
        },
      }
      ]] --

      dap.listeners.before.attach.dapui_config = function()
        dapui.open()
      end
      dap.listeners.before.launch.dapui_config = function()
        dapui.open()
      end
      dap.listeners.before.event_terminated.dapui_config = function()
        dapui.close()
      end
      dap.listeners.before.event_exited.dapui_config = function()
        dapui.close()
      end
    end,
  },

  {
    "theHamsta/nvim-dap-virtual-text",
    config = function()
      require("nvim-dap-virtual-text").setup {
        enabled = true,                     -- enable this plugin (the default)
        enabled_commands = true,            -- create commands DapVirtualTextEnable, DapVirtualTextDisable, DapVirtualTextToggle, (DapVirtualTextForceRefresh for refreshing when debug adapter did not notify its termination)
        highlight_changed_variables = true, -- highlight changed values with NvimDapVirtualTextChanged, else always NvimDapVirtualText
        highlight_new_as_changed = false,   -- highlight new variables in the same way as changed variables (if highlight_changed_variables)
        show_stop_reason = true,            -- show stop reason when stopped for exceptions
        commented = false,                  -- prefix virtual text with comment string
        only_first_definition = true,       -- only show virtual text at first definition (if there are multiple)
        all_references = false,             -- show virtual text on all all references of the variable (not only definitions)
        clear_on_continue = false,          -- clear virtual text on "continue" (might cause flickering when stepping)
        --- A callback that determines how a variable is displayed or whether it should be omitted
        --- @param variable Variable https://microsoft.github.io/debug-adapter-protocol/specification#Types_Variable
        --- @param buf number
        --- @param stackframe dap.StackFrame https://microsoft.github.io/debug-adapter-protocol/specification#Types_StackFrame
        --- @param node userdata tree-sitter node identified as variable definition of reference (see `:h tsnode`)
        --- @param options nvim_dap_virtual_text_options Current options for nvim-dap-virtual-text
        --- @return string|nil A text how the virtual text should be displayed or nil, if this variable shouldn't be displayed
        display_callback = function(variable, buf, stackframe, node, options)
          -- by default, strip out new line characters
          if options.virt_text_pos == 'inline' then
            return ' = ' .. variable.value:gsub("%s+", " ")
          else
            return variable.name .. ' = ' .. variable.value:gsub("%s+", " ")
          end
        end,
        -- position of virtual text, see `:h nvim_buf_set_extmark()`, default tries to inline the virtual text. Use 'eol' to set to end of line
        virt_text_pos = vim.fn.has 'nvim-0.10' == 1 and 'inline' or 'eol',

        -- experimental features:
        all_frames = false,     -- show virtual text for all stack frames not only current. Only works for debugpy on my machine.
        virt_lines = false,     -- show virtual lines instead of virtual text (will flicker!)
        virt_text_win_col = nil -- position the virtual text at a fixed window column (starting from the first text column) ,
        -- e.g. 80 to position at column 80, see `:h nvim_buf_set_extmark()`
      }
    end,
  },

  ---------------------------------------------------------------------------

  {
    "mfussenegger/nvim-dap-python",
    config = function()
      require("dap-python").setup("python")
    end,
  },

  ---------------------------------------------------------------------------
  -- https://github.com/vadimcn/codelldb/releases
  -- https://github.com/julianolf/nvim-dap-lldb

  --[[
  {
    "julianolf/nvim-dap-lldb",
    dependencies = { "mfussenegger/nvim-dap" },

    -- no need for path to codelldb, because it is set
    -- on PATH as an environmental variable
    --opts = { codelldb_path = "/path/to/codelldb" },

    config = function()
      local cfg = {
        configurations = {
          -- C lang configurations
          cpp = {
            {
              name = "Launch debugger",
              type = "lldb",
              request = "launch",
              cwd = "${workspaceFolder}",
              program = function()
                -- Build with debug symbols
                local out = vim.fn.system({ "make", "debug" })
                -- Check for errors
                if vim.v.shell_error ~= 0 then
                  vim.notify(out, vim.log.levels.ERROR)
                  return nil
                end
                -- Return path to the debuggable program
                return "path/to/executable"
              end,
            },
          },

          --c = cpp
        },
      }

      require("dap-lldb").setup(cfg)
    end,
  }
  ]] --


}
