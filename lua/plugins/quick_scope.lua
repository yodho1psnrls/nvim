-- https://github.com/unblevable/quick-scope

return {

  'unblevable/quick-scope',

  -- The order is important, so the themes dont overwrite the
  --  hightlight groups of that quick-scope configures !!!
  --after = 'themes', -- Load quick-scope after themes.lua file
  --dependencies = { 'themes',}, -- QuickScope will wait for themes to load

  config = function()

    --require('config.autocmds')
    --require('plugins.themes')

    -- Moved to options.lua
    -- Trigger a highlight in the appropriate direction when pressing these keys
    -- vim.g.qs_highlight_on_keys = { 'f', 'F', 't', 'T' }

    -- Trigger a highlight only when pressing f and F
    --vim.g.qs_highlight_on_keys = { 'f', 'F' }

    --vim.api.nvim_set_keymap({'n', 'x'}, "<leader>qs", "<plug>(QuickScopeToggle)",
    -- {noremap = false, silent = true, desc = "QuickScopeToggle"})

    vim.g.qs_hi_priority = 2   -- highlight priority (competing with the theme)
    vim.g.qs_enable = 1        -- 0 will start the plugin disabled (default: 1)
    vim.b.qs_local_disable = 0 -- same as qs_enable, but for a specific buffer

    -- Set QuickScope configuration
    vim.g.qs_max_chars = 1000                                 -- disable on line bigger than that (default: 1000)
    vim.g.qs_buftype_blacklist = { 'terminal', 'nofile' }     -- disable for buffers like floating windows and terminals
    vim.g.qs_filetype_blacklist = { 'dashboard', 'startify' } -- disable for dashboard filetypes
    --vim.g.qs_accepted_chars = { 'a', 'b' } -- default: a-z, A-Z, 0-9

    -- The option g:qs_lazy_highlight can be used to change
    --  the vanilla highlight mode autocmd event from CursorMoved
    --  to CursorHold. This option is provided to reduce the slowdown
    --  caused by vanilla highlight mode in large terminals. (default: 0)
    vim.g.qs_lazy_highlight = 0

    -- Delay duration after which the highlighting starts if the cursor is not moved
    --  , this option increases performance
    -- Taken into account only if g:qs_lazy_highlight and g:qs_highlight_on_keys are
    --  not enabled. If you set this to 0, the highlighting will be synchronous.
    --  It requires has('timers'). (default: 50)
    vim.g.qs_delay = 0


    -- Moved to autocmds.lua
    --[[
    -- Define QuickScope highlight colors in an augroup
    vim.api.nvim_create_augroup("qs_colors", { clear = true })
    vim.api.nvim_create_autocmd("ColorScheme", {
      group = "qs_colors",
      pattern = "*",
      callback = function()
        vim.api.nvim_set_hl(0, "QuickScopePrimary",
          --{ fg = "#afff5f", underline = true, ctermfg = 155, cterm = { underline = true } })
          --FF007F --FF77BC
          { fg = "#FF77BC", underline = true, ctermfg = 155, cterm = { underline = true } })
        vim.api.nvim_set_hl(0, "QuickScopeSecondary",
          --{ fg = "#5fffff", underline = true, ctermfg = 81, cterm = { underline = true } })
          { fg = "#FF9933", underline = true, ctermfg = 81, cterm = { underline = true } })
      end,
    })
    ]]--

  ------------------------------------------------------------------------

    vim.keymap.set('n', '<leader>sc', '<cmd>QuickScopeToggle<CR>', {desc = 'Toggle Quick Scope'})

    -- Function to check if the current buffer is writable
    local function is_buffer_writable()
      --local bufnr = vim.api.nvim_get_current_buf()
      --return not vim.bo[bufnr].readonly
      -- return not vim.bo.readonly

      local bufnr = vim.api.nvim_get_current_buf()
      local bufname = vim.api.nvim_buf_get_name(bufnr)

      -- Check if the buffer is writable and not "[No Name]"
      return not vim.bo.readonly and bufname ~= "" and bufname ~= "[No Name]"

    end

    -- Enable quick-scope if the buffer is writable, disable otherwise
    local function toggle_quick_scope()
      if is_buffer_writable() then
        -- Enable QuickScope
        vim.g.qs_enable = 1
      else
        -- Disable QuickScope
        vim.g.qs_enable = 0
      end
    end

    -- Autocommand to check whenever you switch buffers
    vim.api.nvim_create_autocmd('BufEnter', {
      pattern = '*',
      callback = toggle_quick_scope,
    })


  end

}
