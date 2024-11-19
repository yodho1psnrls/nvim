-- https://github.com/nvim-tree/nvim-tree.lua
-- https://medium.com/@shaikzahid0713/file-explorer-for-neovim-c324d2c53657
-- https://github.com/nvim-tree/nvim-tree.lua/wiki/Recipes#workaround-when-using-rmagattiauto-session

-- TODO: Make the Root Folder Name to always apper at the Top
--        when scroll down and the first line of nvim-tree
--        is no longer visible

return {

  --  { "nvim-tree/nvim-web-devicons" }, -- it is a dependancy for other plugins

  {
    "nvim-tree/nvim-tree.lua",

    -- Lazy-load when one of these commands is used
    lazy = true,
    --    cmd = { 'NvimTreeToggle', 'NvimTreeFocus', 'NvimTreeFindFile' },

    dependencies = {
      "nvim-tree/nvim-web-devicons",
    },

    opts = {

      sort = {
        sorter = "case_sensitive",
      },

      view = {
        width = 24,
        side = "left",
        signcolumn = "no",

        preserve_window_proportions = true, -- Option to keep nvim-tree open
        --  sync_root_with_cwd = true,
      },

      sync_root_with_cwd = true,

      actions = {
        change_dir = { global = true } -- Enable global directory change
      },

      renderer = {
        group_empty = true,

        -- Show only the root folder name
        root_folder_label = function()
          return vim.fn.fnamemodify(vim.fn.getcwd(), ":t")
        end,

        --        root_folder_label = false,  -- Disable the default one, because of out custom one

      },

      filters = {
        dotfiles = true,
        custom = { "compile_commands.json" },
      },

      -- Custom function to handle a sticky root folder
      --[[on_attach = function(bufnr)
      --  local api = require('nvim-tree.api')
        local root_name = vim.fn.fnamemodify(vim.fn.getcwd(), ":t")
        vim.api.nvim_buf_set_lines(bufnr, 0, 0, false, { "Root: " .. root_name })
        vim.api.nvim_buf_add_highlight(bufnr, -1, "NvimTreeRootFolder", 0, 0, -1)

        -- Optionally bind keymaps or custom commands here
      --  api.config.mappings.default_on_attach(bufnr)
      end,
      ]] --

    },


  config = function(_, opts)

    require'nvim-tree'.setup(opts)


    -- Make :bd and :q behave as usual when tree is visible
    -- Is this compatible with nvim-bufdelete ???
    vim.api.nvim_create_autocmd({ 'BufEnter', 'QuitPre' }, {
      nested = false,
      callback = function(e)
        local tree = require('nvim-tree.api').tree
        --local autosession = require('auto-session').sa

        -- Nothing to do if tree is not opened
        if not tree.is_visible() then
          return
        end

        -- Do nothing if we are focused over the nvim-tree window
        local current_win_id = vim.api.nvim_get_current_win()
        local tree_win_id = require('nvim-tree.view').get_winnr()
        if current_win_id == tree_win_id then
          return
        end

        -- How many focusable windows do we have? (excluding e.g. incline status window)
        local winCount = 0
        for _, winId in ipairs(vim.api.nvim_list_wins()) do
          if vim.api.nvim_win_get_config(winId).focusable then
            winCount = winCount + 1
          end
        end

        -- Check if there are any unsaved changes
        -- local has_unsaved_changes = false
        -- for _, buf in ipairs(vim.api.nvim_list_bufs()) do
        -- if vim.api.nvim_buf_is_modified(buf) then
        -- has_unsaved_changes = true
        -- break
        -- end
        -- end

        -- We want to quit and only one window besides tree is left
        if e.event == 'QuitPre' and winCount == 2 then
          --      if (has_unsaved_changes) then
          --        return
          --      else

          --vim.api.nvim_cmd({ cmd = 'qall' }, {})

          -- This should fix quitting on unsaved buffers and save on session when the tree is open
          tree.toggle({ find_file = true, focus = true }) -- close nvim-tree: will go to the last buffer used before closing
          --vim.cmd('NvimTreeClose')
          vim.cmd('SessionSave')
          vim.api.nvim_cmd({ cmd = 'q' }, {})

          --      end
        end

        -- :bd was probably issued an only tree window is left
        -- Behave as if tree was closed (see `:h :bd`)
        if e.event == 'BufEnter' and winCount == 1 then
          -- Required to avoid "Vim:E444: Cannot close last window"
          vim.defer_fn(function()
            -- close nvim-tree: will go to the last buffer used before closing
            tree.toggle({ find_file = true, focus = true })
            -- re-open nivm-tree
            tree.toggle({ find_file = true, focus = false })
          end, 10)
        end
      end
    })


    -- Make Nvim-Tree always focus the file that you are currently in
    vim.api.nvim_create_autocmd({ 'BufEnter', 'WinEnter' }, {
      callback = function()
        local tree_api = require('nvim-tree.api')
        local tree = tree_api.tree
        local current_buf = vim.api.nvim_get_current_buf()
        local current_file = vim.api.nvim_buf_get_name(current_buf)

        -- Check if nvim-tree is visible and if the current buffer is valid (not empty and not modified)
        --    if tree.is_visible() and current_file ~= "" and not vim.api.nvim_buf_get_option(current_buf, 'mod') then
        -- Focus on the current file in nvim-tree
        --      tree.find_file(current_file)
        --    end

        -- Check if nvim-tree is visible and if the current buffer has a valid file path
        if tree.is_visible() and current_file ~= "" then
          -- Check if the file exists using vim.fn.filereadable
          if vim.fn.filereadable(current_file) == 1 then
            -- Focus on the current file in nvim-tree
            tree.find_file(current_file)
          end
        end
      end,
    })


    -- Make Nvim-Tree to always focus the file that you are currently in
    -- But if nvim-tree is not loaded(if its lazy loaded), it will not trigger
    --[[
    vim.api.nvim_create_autocmd({ 'BufEnter', 'WinEnter' }, {
      callback = function()
        local status_ok, tree_api = pcall(require, 'nvim-tree.api')
        if not status_ok then return end  -- Exit if nvim-tree is not loaded

        local tree = tree_api.tree
        local current_buf = vim.api.nvim_get_current_buf()
        local current_file = vim.api.nvim_buf_get_name(current_buf)

        -- Only focus if nvim-tree is visible
        if tree.is_visible() and current_file ~= "" and not vim.api.nvim_buf_get_option(current_buf, 'mod') then
          tree.find_file(current_file)  -- Focus on the current file
        end
      end,
    })
    ]] --


-----------------------------------------------------------------------------

-- Auto command to open nvim-tree when Neovim starts
--vim.api.nvim_create_autocmd('VimEnter', {
--  callback = function()
-- Open nvim-tree using the Vim command
--    vim.cmd('NvimTreeOpen')
--  end,
--  once = true,  -- Ensure it runs only once on startup
--})


--vim.api.nvim_create_autocmd('BufWinEnter', {
--  pattern = 'NvimTree_*',
--  callback = function()
--    vim.cmd('cd D:/')  -- Change D:/ to your desired default directory
--    vim.cmd('NvimTreeRefresh')
--  end,
--})


  -- OPPEN NVIMTREE ON STARTUP
  --[[
    vim.api.nvim_create_autocmd('VimEnter', {
      callback = function()
        -- Change to the desired directory
        vim.cmd('cd D:/')  -- Adjust this path to your desired directory

        -- Open NvimTree
        vim.cmd('NvimTreeOpen')
      end,
      once = true,  -- Ensure it runs only once on startup
    })
    --]]

-----------------------------------------------------------------------------


    -- https://github.com/nvim-tree/nvim-tree.lua/wiki/Recipes#workaround-when-using-rmagattiauto-session
    -- Workaround when using rmagatti/auto-session
    --[[
    vim.api.nvim_create_autocmd({ 'BufEnter' }, {
      pattern = 'NvimTree*',
      callback = function()
        local api = require('nvim-tree.api')
        local view = require('nvim-tree.view')

        if not view.is_visible() then
          api.tree.open()
        end
      end,
    })
    ]] --


  end,


  --[[
  -- Make Nvim-Tree to always focus the file that you are currently in
  -- But if nvim-tree is not loaded(if its lazy loaded), it will not trigger
  config = function()
    local status_ok, tree_api = pcall(require, 'nvim-tree.api')
    if not status_ok then
      return -- Exit if nvim-tree is not loaded
    end

    -- Create the autocmd only if nvim-tree is loaded
    vim.api.nvim_create_autocmd({ 'BufEnter', 'WinEnter' }, {
      callback = function()
        local tree = tree_api.tree
        local current_buf = vim.api.nvim_get_current_buf()
        local current_file = vim.api.nvim_buf_get_name(current_buf)

        -- Debug output
--        print("Current buffer:", current_buf)
--        print("Current file:", current_file)
--        print("Is nvim-tree visible?", tree.is_visible())

        -- Only focus if nvim-tree is visible and the buffer is valid
        if tree.is_visible() and current_file ~= "" and not vim.api.nvim_buf_get_option(current_buf, 'mod') then
          tree.find_file(current_file) -- Focus on the current file
        end
      end,
    })
  end,
]] --

  }, -- end of { "nvim-tree" 

} -- end of return
