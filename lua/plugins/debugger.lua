-- https://www.youtube.com/watch?v=MunSY_EXEiI

-- https://github.com/mfussenegger/nvim-dap/wiki/Debug-Adapter-installation#ccrust-via-codelldb
-- https://github.com/vadimcn/codelldb/releases
-- https://github.com/julianolf/nvim-dap-lldb
-- consider using nvim-nio if your debugger makes neovim to freeze
--  it basically makes some external tasks to run in parralel,
--  so neovim doesnt freeze

--https://www.reddit.com/r/neovim/comments/1dfx9vk/how_do_i_setup_nvimdap_irl_it_seems_so_cozy_in/
--https://github.com/rcarriga/nvim-dap-ui/issues/387

--https://github.com/mfussenegger/nvim-dap/issues/198

-- NOTE: Debug Adapter Protocol

-- What is the difference between 
--  dap.configurations and dap.adapters ???

-- Plugins such as nvim-dap-python or nvim-dap-lldb are plugins
--  that only configure your nvim-dap

-- TODO: Configure nvim-dap-ui to show the Call-Stack and the Stack
--  those are the most important features i use from a debugger
-- Also try to automate the debugger path and the build path,
--  so you can debug or just build and run the project just by one keybind
-- And also add some icons for the breakpoints to look prettier

local util = require("utilities")

return {

  {
    "mfussenegger/nvim-dap",

    lazy = true,

    keys = { -- The key mappings that will trigger the loading
      { '<leader>b', desc = "Set [B]reakpoint" },
      { "<leader>lb", desc = "List [B]reakpoints" },
      -- { '<F5>', desc = "Run Debugger" },
    },

    -- It would also be lazy loaded on '<F6>'
    -- See lua/config/build.lua

    -- None of those are dependancies for nvim-dap
    -- Actually, nvim-dap is dependancy for all of them
    dependencies = {
      -- "rcarriga/nvim-dap-ui",
      -- "theHamsta/nvim-dap-virtual-text",
      "lucaSartore/nvim-dap-exception-breakpoints",
      -- "mfussenegger/nvim-dap-python", -- plugin that configures nvim-dap with pylsp

      {
        "nvim-telescope/telescope-dap.nvim",
        lazy = true,
        -- keys = { "<leader>lb", desc = "List [B]reakpoints" },
        dependencies = {
          -- "mfussenegger/nvim-dap",
          "nvim-telescope/telescope.nvim",
        },
        config = function()
          require('telescope').load_extension('dap')
          vim.keymap.set('n', '<leader>lb', function()
            require('telescope').extensions.dap.list_breakpoints{}
          end, { desc = "List [B]reakpoints" })
        end,
      },
    },

    config = function()
      local dap = require('dap')

      -- NOTE: See :help signs and go to the bottom to see all sign funcs from vim.fn.
      vim.api.nvim_set_hl(0, "DapBreakpoint", { link = "Error" })
      vim.fn.sign_define("DapBreakpoint", {
        -- text = "",               -- your icon here
        -- text = ""                -- small black circle
        -- text = "●",               -- standard black circle
        -- text = "⬤",               -- large black circle
        text = "🔴",               -- red circle emoji
        texthl = "DapBreakpoint", -- highlight group
        linehl = "",              -- optional: line highlight
        numhl = "",               -- optional: number column highlight
      })
      vim.fn.sign_define("DapBreakpointCondition", { text = "", texthl = "DapBreakpoint" })
      -- vim.fn.sign_define("DapStopped", { text = "", texthl = "DapStoppedLine" })

      local function BreakLastLine()
        local original_pos = vim.api.nvim_win_get_cursor(0)

        vim.cmd("normal! G")                         -- Move to the last line
        dap.set_breakpoint()
        vim.api.nvim_win_set_cursor(0, original_pos) -- Move cursor to previous position

        print("Breakpoint set at the last line.")
      end

      --vim.keymap.set('n', '<F5>', function() dap.continue() end)
      vim.keymap.set('n', '<F2>', function() dap.step_over() end)
      vim.keymap.set('n', '<F1>', function() dap.step_into() end)
      vim.keymap.set('n', '<F3>', function() dap.step_out() end)

      -- NOTE: Most debugggers do NOT support that
      -- vim.keymap.set('n', '<F4>', function() dap.step_back() end)

      vim.keymap.set('n', '<leader>dq', function() dap.terminate() end, { desc = 'Quit Debugger' })
      vim.keymap.set('n', '<leader>rb', function() dap.clear_breakpoints() end,
        { desc = 'Remove All Breakpoints' })

      -- https://github.com/mfussenegger/nvim-dap/blob/master/doc/dap.txt (lines 875 and 878)
      vim.keymap.set('n', '<leader>-', function() dap.up() end, { desc = "Go up in current stacktrace" })
      vim.keymap.set('n', '<leader>+', function() dap.down() end, { desc = "Go down in current stacktrace" })


      -- https://github.com/mfussenegger/nvim-dap/discussions/576
      --[[
      -- Ask user to stop on which kinds of exceptions
      dap.set_exception_breakpoints()
      -- don't stop on exceptions
      dap.set_exception_breakpoints({})
      -- stop only on certain exceptions (debugpy offers "raised", "uncaught")
      dap.set_exception_breakpoints({ "uncaughted" })
      dap.set_exception_breakpoints({ "raised", "uncaught" })
      -- use default settings of debug adapter
      dap.set_exception_breakpoints("default")
  ]] --

      --    You can also set the default value via a `defaults.fallback` table:
      --dap.defaults.fallback.exception_breakpoints = { 'raised' }
      --dap.defaults.fallback.exception_breakpoints = { "Notice", "Warning", "Error", "Exception" }

      vim.keymap.set('n', '<leader>E', function() BreakLastLine() end,
        { desc = 'Set breakpoint at last line', noremap = true, silent = true })
      vim.keymap.set('n', '<leader>df', function() dap.focus_frame() end,
        { desc = 'Move cursor to current dap line', noremap = true, silent = true })


      vim.keymap.set('n', '<Leader>b', function() dap.toggle_breakpoint() end, { desc = 'Toggle [B]reakpoint' })
      --vim.keymap.set('n', '<Leader>B', function() dap.set_breakpoint() end)
      vim.keymap.set('n', '<Leader>B',
        function() dap.set_breakpoint(vim.fn.input 'Breakpoint condition: ') end,
        { desc = 'Set conditional breakpoint' })
      vim.keymap.set('n', '<Leader>lp',
        function() dap.set_breakpoint(nil, nil, vim.fn.input('Log point message: ')) end, { desc = 'Log point message' })
      vim.keymap.set('n', '<Leader>dr', function() dap.repl.open() end, { desc = 'Open dap repl' })
      vim.keymap.set('n', '<Leader>dl', function() dap.run_last() end, { desc = 'Run last dap session' })

      -- NOTE: Those may come in handy, but currenly i am not sure what to use them for
      --[[
      -- https://github.com/mfussenegger/nvim-dap/issues/161
      vim.keymap.set({ 'n', 'v' }, '<Leader>dh', function()
        require('dap.ui.widgets').hover()
      end, { desc = 'Hover DapUI widgets' })
      vim.keymap.set({ 'n', 'v' }, '<Leader>dp', function()
        require('dap.ui.widgets').preview()
      end, { desc = 'Preview DapUI Widgets' })
      vim.keymap.set('n', '<Leader>dw', function()
        local widgets = require('dap.ui.widgets')
        widgets.centered_float(widgets.frames)
      end, { desc = 'Center widget frames' })
      vim.keymap.set('n', '<Leader>ds', function()
        local widgets = require('dap.ui.widgets')
        widgets.centered_float(widgets.scopes)
      end, { desc = 'Center widget scopes' })
      ]]--

      -------------------------------------------------------

      -- Automatically breaks that the end of the program, so you can still use the debugger
      --dap.listeners.after['event_initialized']['set_final_breakpoint'] = function()
      -- Assuming you know how to determine the last line of your program
      --  local current_frame = require 'dap'.session().current_frame
      --  local last_line = current_frame.source.end_line -- logic to get the last line
      --  dap.set_breakpoint(nil, last_line)              -- Set a breakpoint at the end
      --end
      --[[
      dap.listeners.before.attach.dapui_config = function()
        break_at_end()
      end
      dap.listeners.before.launch.dapui_config = function()
        break_at_end()
      end
      ]] --


      ------------------------------------------------------------

      -- This tells nvim-dap to automatically handle threads in the background,
      -- avoiding the need for manual selection.
      -- dap.defaults.fallback.force_external_terminal = true
      -- dap.defaults.fallback.focus_terminal = true -- If the integrated terminal should get focus when its created

      ------------------------------------------------------------

      dap.adapters.python = function(cb, config)
        -- Path to your python interpreter with debugpy installed
        local python = config.pythonPath or 'python'

        cb({
          type = 'executable',
          command = python,
          args = { '-m', 'debugpy.adapter' },
        })
      end

      dap.configurations.python = {
        {
          type = 'python',
          request = 'launch',
          name = 'Launch File',
          program = '${file}',        -- The script you want to debug
          console = "integratedTerminal",
          pythonPath = function()
            -- try to detect virtualenv automatically
            local cwd = vim.fn.getcwd()
            if vim.fn.executable(cwd .. '/venv/bin/python') == 1 then
              return cwd .. '/venv/bin/python'
            elseif vim.fn.executable(cwd .. '/.venv/bin/python') == 1 then
              return cwd .. '/.venv/bin/python'
            else
              return 'python'
            end
          end,
        },
        {
          type = 'python',
          request = 'attach',
          name = 'Attach to running debugpy',
          connect = function()
            return {
              host = '127.0.0.1',
              port = 5678,
            }
          end,
          console = "integratedTerminal",
        },
      }

      ------------------------------------------------------------

      -- NOTE: NodeJS debugger
      -- Steps to install it:
      -- git clone https://github.com/microsoft/vscode-node-debug2.git
      -- cd vscode-node-debug2
      -- npm install
      -- npm install source-map@0.7.4 -- Fix to an issue, that i had
      -- npm run build

      dap.adapters.node2 = {
        type = "executable",
        command = "node",
        args = {"C:/MyPrograms/vscode-node-debug2/out/src/nodeDebug.js"},
      }

      dap.configurations.javascript = {
        {
          type = "node2",
          request = "launch",
          name = "Launch File",
          program = "${file}",
          cwd = vim.fn.getcwd(),
          sourceMaps = true,
          protocol = "inspector",
        },
      }

      dap.configurations.javascriptreact = dap.configurations.javascript

      ------------------------------------------------------------

      -- https://github.com/mfussenegger/nvim-dap/wiki/C-C---Rust-(via--codelldb)
      --local dap = require('dap')
      dap.adapters.codelldb = {
        type = 'server',
        port = "${port}",
        executable = {
          --command = '/path/to/codelldb/adapter/codelldb',
          command = 'codelldb',
          args = { "--port", "${port}" },

          -- If you're on Windows and facing detachment issues, uncomment the next line
          detached = false,
        }
      }

      dap.configurations.cpp = {
        {
          --name = "Launch file",
          name = "Launch Project",
          type = "codelldb",
          request = "launch",

          --program = function()
          --  return vim.fn.input('Path to executable: ', vim.fn.getcwd() .. '/', 'file')
          --end,

          --program = vim.fn.getcwd() .. '\\Debug\\bin\\proj.exe',
          -- cwd = vim.fn.getcwd(),
          -- cwd = '${workspaceFolder}',

          -- program = util.get_project_root() .. '\\Debug\\bin\\proj.exe',
          program = '\\Debug\\bin\\proj.exe',
          cwd = util.get_project_root(),

          stopOnEntry = false,   -- Pause the debugger at the begin point of the program (like a breakpoint at the start)

          args = {},             -- Arguments to pass to the executable program
          runInTerminal = false, -- Runs the program and hopefoly shows a life preview of the program execution during debbuging

          --terminal = 'integrated',

          setupCommands = {
            {
              -- https://www.reddit.com/r/neovim/comments/13m46c0/custom_pretty_printerdebug_helpers_for_c_debugging/
              -- Visualizes data structures like map, vector, etc better, than just raw pointers
              -- Makes them more human readable and easier to debug
              text = "-enable-pretty-printing", -- Pretty printing for C++
              description = "Enable pretty printing",
              ignoreFailures = false
            },

            --[[
            {
              text = "break set -E c++", -- Break on C++ exceptions
              description = "Break on unhandled C++ exceptions",
              ignoreFailures = false
            },
            {
              text = "break set -n __assert_fail", -- Break on failed assertions in Linux (glibc)
              description = "Break on failed assertions",
              ignoreFailures = false
            },
            {
              text = "break set -n _assert", -- Break on failed assertions (other platforms)
              description = "Break on failed assertions",
              ignoreFailures = false
            },
            {
              text = "break set -n abort", -- Break on abort signals
              description = "Break on abort (e.g., failed assertions)",
              ignoreFailures = false
            }
            ]] --
            --[[
            {
              -- Break on C++ exceptions (adjust depending on your system)
              text = "break set -E c++",
              description = "Break on unhandled C++ exceptions",
              ignoreFailures = false
            },
            {
              -- Break on abort() (for failed assertions)
              text = "breakpoint set --name abort",
              description = "Break on failed assertions (abort function)",
              ignoreFailures = false
            },
            {
              -- Handle SIGABRT signal (abort signal)
              text = "process handle SIGABRT --stop true --pass true --notify true",
              description = "Stop on SIGABRT (for failed assertions)",
              ignoreFailures = false
            }
]]                                                                                     --

            { text = "process handle SIGABRT --stop true --pass true --notify true" }, -- For failed assertions
            { text = "break set -E c++" },                                             -- For unhandled C++ exceptions
            { text = "-enable-pretty-printing" }                                       -- Pretty printing for more readable call stacks

          },

        },

        {
          name = "Launch Test",
          type = "codelldb",
          request = "launch",

          program = '\\Debug\\bin\\proj_test.exe',
          cwd = util.get_project_root(),

          stopOnEntry = false,   -- Pause the debugger at the begin point of the program (like a breakpoint at the start)

          args = {},             -- Arguments to pass to the executable program
          runInTerminal = false, -- Runs the program and hopefoly shows a life preview of the program execution during debbuging

          --terminal = 'integrated',

          setupCommands = {
            {
              text = "-enable-pretty-printing", -- Pretty printing for C++
              description = "Enable pretty printing",
              ignoreFailures = false
            },

            { text = "process handle SIGABRT --stop true --pass true --notify true" }, -- For failed assertions
            { text = "break set -E c++" },                                             -- For unhandled C++ exceptions
            { text = "-enable-pretty-printing" }                                       -- Pretty printing for more readable call stacks
          },

        },

      }
      dap.configurations.c = dap.configurations.cpp


      --[[dap.adapters.python = {
        type = 'executable',
        command = 'D:/Program Files/MSYS2/mingw64/bin/python.exe', -- Replace with the path to your Python interpreter
        args = { '-m', 'debugpy.adapter' },
      }

      dap.configurations.python = {
        {
          type = 'python',
          request = 'launch',
          name = 'Launch file',
          program = '${file}', -- The current file to debug
          pythonPath = function()
            return 'D:/Program Files/MSYS2/mingw64/bin/python.exe' -- Same interpreter path as above
          end,
        },
      }]]--

      --===========================================================--

      local nvim_tree = util.safe_require('nvim-tree.api')
      local was_tree_opened = false

      dap.listeners.before.attach.dapui_config = function()
        require("dapui").open()
      end
      dap.listeners.before.launch.dapui_config = function()
        require("dapui").open()
      end

      dap.listeners.before.event_terminated.dapui_config = function()
        require("dapui").close()
      end
      dap.listeners.before.event_exited.dapui_config = function()
        require("dapui").close()
        if nvim_tree and was_tree_opened then
          -- nvim_tree.tree.open()
          nvim_tree.tree.toggle({ find_file = true, focus = false })
        end
      end


      -----------------------------------------------------

      dap.listeners.after.event_initialized.dapui_config = function()
        if nvim_tree then     -- Check if nvim-tree is loaded
          -- was_tree_opened = require'nvim-tree.view'.is_visible()
          was_tree_opened = nvim_tree.tree.is_visible()
          nvim_tree.tree.close()
        end
      end


      ------------------------------------------------------------
    end,
  },

  -- NOTE: If you want an alternative for this in the future, consider
  -- Carcuis/dap-breakpoints.nvim or Weissle/persistent-breakpoints.nvim
  -- BUG: THIS PLUGIN I CAUSING A MASSIVE LAG SPIKE WHEN OPENING OR CLOSING FILES
  --[[
  {
    -- A plugin that auto-saves the breakpoints and some additional features
    -- https://www.reddit.com/r/neovim/comments/177zwub/daphelpernvim_small_helper_plugin_for_nvimdap_and/
    'daic0r/dap-helper.nvim',
    dependencies = { "rcarriga/nvim-dap-ui", "mfussenegger/nvim-dap" },
    config = function()
      require("dap-helper").setup()
    end,
  },
  ]]--

  -- dependancy for nvim-dap-ui
--  { "nvim-neotest/nvim-nio", },


  -- https://github.com/mfussenegger/nvim-dap/discussions/576
  {
    "lucaSartore/nvim-dap-exception-breakpoints",
--    dependencies = { "mfussenegger/nvim-dap" },
    lazy = true,
    config = function()
      --[[
      local set_exception_breakpoints = require("nvim-dap-exception-breakpoints")

      vim.api.nvim_set_keymap(
        "n",
        "<leader>dc",
        "",
        { desc = "[D]ebug [C]ondition breakpoints", callback = set_exception_breakpoints }
      )
      ]] --

      --require('dap').defaults.fallback.exception_breakpoints = {'raised'}
      --require('dap').defaults.cpp.assertions = { 'raised' } -- I dont think this is valid
    end
  },


  -- TODO: Fix this to use the configuration below,
  -- because currently it uses the default one
  {
    "rcarriga/nvim-dap-ui",
    lazy = true,

    dependencies = {
      --"mfussenegger/nvim-dap",
      "nvim-neotest/nvim-nio",
      "theHamsta/nvim-dap-virtual-text",
    },

    opts = {
      --layout = "flex",  -- You can change the layout: "vertical", "horizontal", or "flex"

      layout = { {
        -- These can be turned off if you don't want to show specific windows
        elements = { {
          { id = "scopes", size = 0.75 },
          { id = "stack", size = 0.25 },
        } },
        position = "left",
        size = 40
      }, {
        elements = { {
          id = "repl",
          size = 0.25
        }, {
          id = "console",
          size = 0.25
        } },
        position = "bottom",
        size = 10
      } },
    },

    -- init = function(_, opts)
    --   require("dapui").setup(opts)
    -- end,

    config = function(_, opts)
      require("dapui").setup(opts)

      --[[dapui.setup {
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
      }]] --

    end,
  },

  {
    "theHamsta/nvim-dap-virtual-text",
    lazy = true,
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

  --[[{
    "mfussenegger/nvim-dap-python",
    lazy = true,
    config = function()
      require("dap-python").setup("python")
      table.insert(require('dap').configurations.python, {
        type = 'python',
        request = 'launch',
        name = 'custom',

        -- `program` is what you'd use in `python <program>` in a shell
        -- If you need to run the equivalent of `python -m <module>`, replace
        -- `program = '${file}` entry with `module = "modulename"
        -- program = '${file}',
        program = util.find_python_exec(),

        console = "integratedTerminal",

        -- Other options:
        -- See https://github.com/microsoft/debugpy/wiki/Debug-configuration-settings
      })
    end,
  },]]--

  ---------------------------------------------------------------------------
  -- https://github.com/vadimcn/codelldb/releases
  -- https://github.com/julianolf/nvim-dap-lldb

  --[[
  {
    "julianolf/nvim-dap-lldb",
    dependencies = { "mfussenegger/nvim-dap" },
  }
  ]] --


}
