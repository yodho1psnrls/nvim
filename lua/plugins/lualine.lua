-- https://github.com/nvim-lualine/lualine.nvim
-- https://medium.com/@shaikzahid0713/lualine-for-neovim-776b79861699


local function adaptive_separator()
  -- Get the current window width
  local width = vim.fn.winwidth(0)
  -- Calculate the length of the separator based on the width
  local separator_length = math.max(1, width - 2) -- Adjust as needed
  return string.rep("-", separator_length)
end


local function current_scope_name()
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
end


--[[local function current_scope_name()
  local params = vim.lsp.util.make_position_params()
  local result = vim.lsp.buf_request_sync(0, 'textDocument/documentSymbol', params, 1000)

  if result and next(result) then
    for _, symbol in pairs(result[1].result) do
      if symbol.range.start.line <= vim.fn.line('.') - 1 and symbol.range['end'].line >= vim.fn.line('.') - 1 then
        return symbol.name
      end
    end
  end

  return ''
end]]--


return {
  {
    "nvim-lualine/lualine.nvim",
    dependencies = {
      "nvim-tree/nvim-web-devicons", -- Icons support
      "archibate/lualine-time",      -- Clock extension

      -- Not a dependeny, but to lazy load it
      "nvim-lua/lsp-status.nvim",

    },

    lazy = true,
    event = 'BufReadPost',

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

          --component_separators = { left = '', right = ''},
          component_separators = { left = '|', right = '|' },
          section_separators = { left = '', right = '' },
          disabled_filetypes = {
            statusline = {},
            winbar = {},
          },
          always_divide_middle = true,
          globalstatus = true, -- true(global status line), false(separate status lines per each window)
        },
        sections = {
          lualine_a = { 'mode' },
          lualine_b = { 'branch', 'diff', 'diagnostics' },

          -- 0: Show the absolute path to the file.
          -- 1: Show the relative path from the current working directory (CWD).
          -- 2: Show the relative path from the root of the project (if your project is defined).
          -- 3: Show just the filename without any path.
          lualine_c = { {'filename', path = 1} },

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
          -- lualine_y = {'progress'},
          --lualine_z = {'location'}

          lualine_z = {
            { 'location', fmt = function()
              local line = vim.fn.line('.')
              local col = vim.fn.col('.')
              return line .. '|' .. col -- Use '|' as the separator
            end }
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
            { 'location', fmt = function()
              local line = vim.fn.line('.')
              local col = vim.fn.col('.')
              return line .. '|' .. col -- Use '|' as the separator
            end }
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
