-- https://www.reddit.com/r/neovim/comments/13iy0p0/why_some_people_use_buffers_for_tabs_and_not_vim/
-- https://joshldavis.com/2014/04/05/vim-tab-madness-buffers-vs-tabs/

-- TODO: https://github.com/akinsho/bufferline.nvim/blob/main/doc/bufferline.txt#L388
-- Group buffers by directory, and then group them by the custom logic
--  (which is newest written buffer, currently)

-- TODO:
-- Make the tabline scroll offset not change on multiple open tabs
-- (So much that they dont fit on the width of the screen)
-- , by default when i go to a No Name buffer(line NvimTree or Telescope)
-- , it scrolls to the end of the tab list.
-- I want it to stay the same scroll position as it was in the last
-- opened valid buffer that i was in

-- NOTE: Look at this if you want to replace this plugin with native
-- https://github.com/LionyxML/dotneovim/blob/main/init.lua#L1864
-- https://github.com/LionyxML/dotneovim/blob/012b5d2a0fc8122c26c41a5332f5452c287d8ed0/init.lua#L1864
-- Look for tabline and arglist
-- https://www.reddit.com/r/vim/comments/u3nv6/buffer_list_vs_arguments_list/

local util = require("utilities")


local function get_current_buffer_name()
  local bufname = vim.api.nvim_buf_get_name(0)  -- Get full file path
  if bufname == "" then
    return "[No Name]"  -- Handle empty buffers
  end
  return vim.fn.fnamemodify(bufname, ":t")  -- Get only the file name
end


local function get_symbols_outline_shown_buffer()
  local so = util.safe_require("symbols-outline")
  if not so then return end
  -- This gives the symbols-outline buffer, not the buffer that it shows the symbols of
  local buf = so.view.bufnr
  if buf then
    return vim.api.nvim_buf_get_name(buf)
  else
    return "[No Buffer Shown]"
  end
end


return {
  {
    'akinsho/bufferline.nvim',
    version = "*",
    dependencies = {
      'nvim-tree/nvim-web-devicons'
    },

    lazy = true,
    -- event = { "BufReadPre", "BufNewFile" }, -- Only triggers for actual file-related events
    event = { "VimEnter", },

    opts = {
      options = {

        max_name_length = 15, -- 12 -- This doesnt matter, if truncate_names = false
        buffer_padding = 0, -- 1
        tab_size = 0, -- == 3 + max_name_length
        max_prefix_length = 15, -- Max length for buffer names when the tabline overflows

        show_buffer_icons = true,
        show_buffer_close_icons = false,
        show_clone_icon = false,
        show_tab_indicators = true,

        show_duplicate_prefix = false,


        enforce_regular_tabs = false,  -- make all tabs same size

        -- Doesnt seem to do anything
        --scroll_strategy = "close", -- default, cycle, close | thiccc, none, cycle 

        always_show_bufferline = true, -- false = hidden, when only one buffer open
        auto_toggle_bufferline = true,

        --separator_style = "slant" | "padded_slant" | "slope" | "padded_slope" | "thick" | "thin" | { 'any', 'any' },
        separator_style = "slant",
        --separator_style = { '(', ')' },

        -- You can order t
        persist_buffer_sort = true,
        move_wraps_at_ends = true,

        --modified_icon = '+',  -- '● '

        -- When true, names longer than max_name_length would be shortened
        truncate_names = false,


        -- If you want underline over current buffer name
        --[[indicator = {
          --icon = '▎', -- this should be omitted if indicator style is not 'icon'
          --style = 'icon' | 'underline' | 'none',
          icon = "",
          style = "icon",
        },]]--

        -- Mouse hover reveals the close button
        --[[hover = {
          enabled = true,
          delay = 200,
          reveal = {'close'}
        },]]--

        --sort_by = 'relative_directory', -- or 'tabs', use other sorting as preferred

        -- By default it sorts them by buffer number, just the same order as :ls
        sort_by = function(buffer_a, buffer_b)
          -- Get the last written time of the buffers
          local function get_last_written(bufnr)
            local filename = vim.api.nvim_buf_get_name(bufnr)
            if vim.fn.filereadable(filename) == 1 then
              return vim.fn.getftime(filename)
            end
            --return -1 -- Default for unsaved or nonexistent files - Place in back
            return math.huge -- Place new empty buffers in front
          end

          local time_a = get_last_written(buffer_a.id)
          local time_b = get_last_written(buffer_b.id)

          return time_a > time_b -- Sort descending by last written time
          -- return time_a < time_b

          -- Example from :help bufferline-configuration
          -- local modified_a = vim.fn.getftime(buffer_a.path)
          -- local modified_b = vim.fn.getftime(buffer_b.path)
          -- return modified_a > modified_b
        end,

        -- TODO: Fix the border separator to show up on the tabline,
        --        so the whole border is seamless

        -- Use ":set filetype?" to see the filetype
        offsets = {
          {
            filetype = "NvimTree",
            --text = "File Explorer",
            text = function() return ' ' .. vim.fn.fnamemodify(vim.fn.getcwd(), ':t') end,
            -- text = function() return ' ' .. vim.fn.fnamemodify(vim.fn.getcwd(), ':t') end,
            -- text = function() return '󰇘\\' .. vim.fn.fnamemodify(vim.fn.getcwd(), ':t') end,
            -- text = function() return '\\' .. vim.fn.fnamemodify(vim.fn.getcwd(), ':t') .. '\\' end,
            highlight = "Directory",
            text_align = "left",  -- left, center, right

            padding = 0, -- bufferline offset beyond the window width
            offset_separator = true, -- a highlight for vertical split (a string or a boolean(which is default if set to true))
            --offset_separator = '|', -- This doesnt seem to work
            --separator == true,
          },
          {
            filetype = "Outline",

            text = "Symbols Outline",
            -- text = function() return "└" .. get_current_buffer_name() end,
            -- text = function() return " " .. get_current_buffer_name() end,
            -- text = function() return " " .. get_symbols_outline_shown_buffer() end,

            -- highlight = "CustomMessagesHighlight",
            -- highlight = "WarningMsg",
            highlight = "Comment", -- FocusedSymbol | Pmenu | SymbolsOutlineConnector | Comment
            -- highlight = { gui = 'underline', link = 'WarningMsg' },

            text_align = "center",  -- left, center, right

            padding = 0, -- bufferline offset beyond the window width
            offset_separator = true, -- a highlight for vertical split (a string or a boolean(which is default if set to true))
            --offset_separator = '|', -- This doesnt seem to work
            --separator == true,
          },
          {
            filetype = "undotree",
            text = "UndoTree",
            highlight = "Comment",
            text_align = "center",  -- left, center, right

            padding = 0, -- bufferline offset beyond the window width
            offset_separator = true, -- a highlight for vertical split (a string or a boolean(which is default if set to true))
            --offset_separator = '|', -- This doesnt seem to work
            --separator == true,
          },
        },

        --[[style_preset = {
          require('bufferline').style_preset.no_italic,
          --require('bufferline').style_preset.no_bold,
          --require('bufferline').style_preset.minimal,
        },]]--


        --diagnostics = "nvim_lsp",
        --[[diagnostics_indicator = function(count, level, diagnostics_dict, context)
          local icon = level:match("error") and " " or " "
          return " " .. icon .. count
        end]]--

      },
    },

    config = function (_, opts)
      vim.opt.termguicolors = true
      require("bufferline").setup(opts)

      --vim.api.nvim_set_hl(0, "BufferLineOffsetSeparator",
      --  {fg = "#ffffff", bg = "#000000"})


      -- Trigger bufferline to Refresh on save
      -- (because of we sort the buffers by last time saved)

      --vim.cmd([[ autocmd BufWritePost * lua vim.api.nvim_input(":") vim.api.nvim_input("<Esc>") ]])
      -- Additional Esc infront, so we dont type :, if we are in insert mode
      --vim.cmd([[ autocmd BufWritePost * lua vim.api.nvim_input("<Esc>") vim.api.nvim_input(":") vim.api.nvim_input("<Esc>") ]])

        -- Forces the bufferline to redraw without cycling
      --vim.cmd([[autocmd BufWritePost * lua vim.cmd("redraw")]])
      vim.cmd([[autocmd BufWritePost * lua vim.cmd("redrawtabline")]])

      --vim.cmd([[ autocmd BufWritePost * lua require'bufferline'.refresh() ]])
      --vim.cmd([[ autocmd BufWritePost * lua vim.cmd("BufferLineCycleNext | BufferLineCyclePrev") ]])
      -- vim.cmd([[ 
      --   autocmd BufWritePost * lua vim.cmd("BufferLineCycleNext")
      --   autocmd BufWritePost * lua vim.cmd("BufferLineCyclePrev")
      -- ]])



      vim.api.nvim_set_keymap(
        'n',
        '<Tab>',
        ':BufferLineCycleNext<CR>',
        -- ':BufferLineCyclePrev<CR>',
        { noremap = true, silent = true, desc = 'Next buffer in the recent lineup' }
      )

      vim.api.nvim_set_keymap(
        'n',
        '<S-Tab>',
        ':BufferLineCyclePrev<CR>',
        -- ':BufferLineCycleNext<CR>',
        { noremap = true, silent = true, desc = 'Previous buffer in the recent lineup' }
      )

    end,

  }
}



