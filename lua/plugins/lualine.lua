-- https://github.com/nvim-lualine/lualine.nvim
-- https://medium.com/@shaikzahid0713/lualine-for-neovim-776b79861699
-- https://www.reddit.com/r/neovim/comments/1gbq198/how_to_display_function_name_in_current_context/

local function adaptive_separator()
  -- Get the current window width
  local width = vim.fn.winwidth(0)
  -- Calculate the length of the separator based on the width
  local separator_length = math.max(1, width - 2) -- Adjust as needed
  return string.rep("-", separator_length)
end

--[[local function current_scope_name()
  local ts_utils = require('nvim-treesitter.ts_utils')
  local node = ts_utils.get_node_at_cursor()
  if not node then return '' end

  local scope_name = ''
  while node do
    local type = node:type()
    -- Check for relevant scope node types (adjust these as needed)
    if type == 'function_definition' or type == 'class_definition' then
      local name_node = node:field('name')[1]
      if name_node then
        scope_name = vim.treesitter.get_node_text(name_node, 0) .. (scope_name ~= '' and ' -> ' .. scope_name or '')
      end
    end
    node = node:parent()
  end

  return scope_name
end]]--

-- This lags as sh*t
local function current_scope_name()
  local params = vim.lsp.util.make_position_params(0, vim.o.encoding)
  local result = vim.lsp.buf_request_sync(0, 'textDocument/documentSymbol', params, 1000)

  if result and next(result) then
    for _, symbol in pairs(result[1].result) do
      if symbol.range.start.line <= vim.fn.line('.') - 1 and symbol.range['end'].line >= vim.fn.line('.') - 1 then
        return symbol.name
      end
    end
  end

  return ''
end

--[[local function ts_context()
  local ok, ts = pcall(require, "nvim-treesitter")
  if not ok then
    return ""
  end

  -- This returns something like "Namespace › Class › Method"
  local ctx = ts.statusline({
    indicator_size = 100,
    type_patterns = { "class", "function", "method", "namespace" },
    separator = " > ",
    transform_fn = function(line)
      return line:gsub("%s+", "")
    end,
  })

  return ctx or ""
end]]--

-- local ts = require("nvim-treesitter")
local function ts_context()
  local ctx = ts.statusline({
    indicator_size = 200,
    type_patterns = { "class", "function", "method", "namespace", "struct", "interface" },
    -- separator = " > ",
    -- separator = " › ",
  })
  if not ctx or ctx == "" then return "" end
  local parts = vim.split(ctx, "-> ", { plain = true })
  ctx = parts[#parts]

  -- ctx = ctx
  --   :gsub("^local%s+", "")           -- strip leading “local”
  --   :gsub("function%s*", "")         -- strip leading “function”
  --   :gsub("method%s*", "")           -- strip leading “method”
  --   :gsub("class%s*", "")            -- strip leading “class”
  --   :gsub("namespace%s*", "")        -- strip leading “namespace”
  --   :gsub("struct%s*", "")           -- strip leading “struct”
  --   :gsub("%b()", "")                -- remove brackets with all parameters in them
  --   :gsub("%b<>", "")                -- remove templates with all parameters in them
    -- :gsub("%s*[%(%)]%s*", "")        -- remove leftover parentheses  
    -- :gsub("%s*[%[%]]%s*", "")        -- remove TS internal brackets
    -- :gsub("%s+-", "-")               -- tighten separators from left
    -- :gsub(">%s+", ">")               -- tighten separators from right

  -- ctx = ctx:match("([%w_]+)%s*%(") -- First find word before (
  -- ctx = ctx or ctx:match("([%w_]+)$") -- else get last word

  return ctx
end

--[[local ts_utils = require("nvim-treesitter.ts_utils")
local function ts_inner_context()
  local node = ts_utils.get_node_at_cursor()
  if not node then return "" end

  -- climb until we find a meaningful container node
  while node do
    local type = node:type()

    if type == "function_definition"
      or type == "function_declaration"
      or type == "method_declaration"
      or type == "class_specifier"
      or type == "namespace_definition"
    then
      -- This gives the whole node text, not only its name
      local id = vim.treesitter.get_node_text(node, 0)
      return id or ""
    end

    node = node:parent()
  end

  return ""
end]]--

-- Instead of sticky headers and context windows,
-- just jump quickly to it with [[ and return with <C-o>
-- https://stackoverflow.com/questions/33699049/display-current-function-in-vim-status-line

return {
  {
    "nvim-lualine/lualine.nvim",
    dependencies = {
      "nvim-tree/nvim-web-devicons", -- Icons support
      "archibate/lualine-time",      -- Clock extension
      -- "nvim-lua/lsp-status.nvim", -- Not a dependeny, but to lazy load it
    },

    -- lazy = true,
    -- event = {'BufReadPost', 'TermEnter'},

    config = function()
      -- Ensure the time plugin is properly loaded
      require('lualine').setup {
        options = {
          icons_enabled = true,
          theme = 'auto', -- You can set this to 'rose-pine' or any theme you prefer

          laststatus = 3,

          --[[theme = {
            normal = {
              a = { fg = '#ffffff', bg = '#1c1c1c' }, -- Active section colors
              b = { fg = '#ffffff', bg = '#1c1c1c' },
              c = { fg = '#ffffff', bg = '#1c1c1c' },
            },
            inactive = {
              a = { fg = '#000000', bg = '#ffffff' }, -- Inactive section colors (white background)
              b = { fg = '#000000', bg = '#ffffff' },
              c = { fg = '#000000', bg = '#ffffff' },
            },
          },]] --

          component_separators = { left = '', right = ''},
          -- component_separators = { left = '|', right = '|' },
          section_separators = { left = '', right = '' },
          disabled_filetypes = {
            statusline = {},
            winbar = {},
          },
          always_divide_middle = true,
          globalstatus = true, -- true(global status line), false(separate status lines per each window)
        },
        sections = {
          lualine_a = {
            {
              'mode',
              -- fmt = function(str) return str:sub(1,1) end
            },
          },
          lualine_b = {
            'branch',
            'diff',
            -- 'diagnostics',
            {
              'diagnostics',
              sources = { 'nvim_diagnostic' },
              symbols = {
                -- Copying the icons from the signcolumn diagnostics
                error = ' ',
                warn  = ' ',
                info  = ' ',
                hint  = ' ',
              },
            },
          },

          lualine_c = {
            -- 0: Show the absolute path to the file.
            -- 1: Show the relative path from the current working directory (CWD).
            -- 2: Show the relative path from the root of the project (if your project is defined).
            -- 3: Show just the filename without any path.
            -- https://github.com/nvim-lualine/lualine.nvim?tab=readme-ov-file#filename-component-options
            {'filename', path = 1},
            -- ts_inner_context,
            -- ts_context, -- THIS
            -- current_scope_name,
          },

          -- Make sure `cdate` and `ctime` are available from lualine-time
          --lualine_x = {'encoding', 'fileformat', 'filetype', 'cdate', 'ctime'},
          --lualine_x = {'cdate', 'ctime' },
          lualine_x = {'ctime'}, -- Shows hours, minutes and seconds
          --lualine_x = { { 'time', fmt = function() return os.date("%H:%M") end } }, -- Show only hours and minutes

          --[[lualine_x = {
            { 'time', fmt = function()
              os.setlocale("en_US.UTF-8")  -- Set locale to ensure AM/PM works
              return os.date("%I:%M%p"):lower() -- %I gives the 12-hour format with AM/PM --and makes am/pm lower case
            end }
          },]] --

          lualine_y = {},
          --[[lualine_y = {
            { -- https://github.com/nvim-lualine/lualine.nvim?tab=readme-ov-file#lsp-status-component-options
              'lsp_status',
              -- icon = '', -- f013
              icon = '',
              symbols = {
                spinner = { '⠋', '⠙', '⠹', '⠸', '⠼', '⠴', '⠦', '⠧', '⠇', '⠏' },
                done = '✓',
                -- separator = ' ',
                separator = '',
              },
              ignore_lsp = {}, -- List of LSP names to ignore (e.g., `null-ls`):
              show_name = true, -- Display the LSP name
            },
          },]]--
          -- lualine_y = {'progress'},
          --lualine_z = {'location'}
          lualine_z = {
            function()
              local line = vim.fn.line('.')
              local col = vim.fn.col('.')
              return line .. '|' .. col -- Use '|' as the separator
            end,
          },

        },
        inactive_sections = {
          lualine_a = {},
          lualine_b = {},
          --lualine_c = { 'filename' },
          lualine_c = { {'filename', path = 1} },

          --lualine_c = { {'"============================================"', color = { fg = '#000000', bg = '#ffffff' }}},
          --lualine_c = { { 'filename', '"' .. adaptive_separator() .. '"', color = { fg = '#ffffff'}}},

          --lualine_x = { 'location' },
          lualine_x = {
            function()
              local line = vim.fn.line('.')
              local col = vim.fn.col('.')
              return line .. '|' .. col -- Use '|' as the separator
            end,
          },

          lualine_y = {},
          lualine_z = {}

          --lualine_a = { '', color = { fg = '#000000', bg = '#ffffff' }},
          --lualine_b = { '', color = { fg = '#000000', bg = '#ffffff' }},
          --lualine_c = { 'filename', color = { fg = '#000000', bg = '#ffffff' } },
          --lualine_x = { 'location', color = { fg = '#000000', bg = '#ffffff' } },
          --lualine_y = { '', color = { fg = '#000000', bg = '#ffffff' }},
          --lualine_z = { '', color = { fg = '#000000', bg = '#ffffff' }}
        },

        --[[tabline = {
          lualine_a = {},
          lualine_b = { {function() current_scope_name() end} },
          lualine_c = {},
          lualine_x = {},
          lualine_y = {},
          lualine_z = {},
        },]]--

        tabline = {},

        winbar = {},
        inactive_winbar = {},
        extensions = {}
      }
    end,
  }
}
