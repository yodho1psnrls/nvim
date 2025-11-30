-- https://github.com/nvim-tree/nvim-tree.lua
-- https://medium.com/@shaikzahid0713/file-explorer-for-neovim-c324d2c53657
-- https://github.com/nvim-tree/nvim-tree.lua/wiki/Recipes#workaround-when-using-rmagattiauto-session

-- TODO: Make the Root Folder Name to always apper at the Top
--        when scroll down and the first line of nvim-tree
--        is no longer visible
--       Just like https://github.com/nvim-tree/nvim-tree.lua/discussions/2822

-- d to delete a file (will be prompted for confirmation)
-- r to rename a file
-- r to move a file, by just giving it another directory
-- a to add a new file in the directory that your cursor is in

-- local util = require("utilities")

local width_ratio = 0.25

return {

  --  { "nvim-tree/nvim-web-devicons" }, -- it is a dependancy for other plugins
  -- See https://github.com/JMarkin/nvim-tree.lua-float-preview

  {
    "nvim-tree/nvim-tree.lua",

    -- Lazy-load when one of these commands is used
    lazy = true,
    --    cmd = { 'NvimTreeToggle', 'NvimTreeFocus', 'NvimTreeFindFile' },

    -- It will be lazy loaded on '<leader>n'
    -- See lua/config/keymaps.lua

    keys = {
      {"<leader>n", desc = "[N]vimtree toggle"},
    },
    cmd = {
      "NvimTreeOpen",
      "NvimTreeToggle",
      "NvimTreeFocus",
      "NvimTreeFindFile",
    },

    dependencies = {
      "nvim-tree/nvim-web-devicons",

      -- Load this as dependancy, because we lazy load it,
      -- but we need it, when we you nvim-tree, because 
      -- we use the bufferline to display the root folder
      -- name over the nvim-tree window
      "akinsho/bufferline.nvim",

    },

    opts = {

      sort = {
        sorter = "case_sensitive",
      },

      view = {
        width = function() return math.floor(width_ratio * vim.o.columns) end,
        -- width = 24, -- 24
        -- width = 32,
        side = "left",
        signcolumn = "no",

        -- Unique highlight to current viewing buffer
        -- https://github.com/nvim-tree/nvim-tree.lua/discussions/2925
        --number = true,
        --cursorline = true,

        preserve_window_proportions = true, -- Option to keep nvim-tree open
        --  sync_root_with_cwd = true,

        --[[mappings = {
          list = {
            {
              key = '+', action = 'cd',
              -- key = '-', action = 'dir_up',
            },
          },
        },]]--

      },


      -- Since by default - goes one directory up,
      --  its logical to set + to go directory down (where the cursor points)
      --[[on_attach = function(bufnr)
        local api = require("nvim-tree.api")
        local opts = function(desc)
          return { desc = "nvim-tree: " .. desc, buffer = bufnr, noremap = true, silent = true, nowait = true }
        end

        -- Default key mappings
        api.config.mappings.default_on_attach(bufnr)

        -- Custom key mapping for `-`
        vim.keymap.set("n", "-", api.tree.change_root_to_node, opts("CD"))
      end,]]--


      sync_root_with_cwd = true,

      actions = {
        change_dir = { global = true } -- Enable global directory change
      },

      renderer = {
        group_empty = true,

        -- Show only the root folder name
       --[[ root_folder_label = function()
          return vim.fn.fnamemodify(vim.fn.getcwd(), ":t")
        end, ]]--

        -- Disable the default one, because the tabline implements that
        root_folder_label = false,

        highlight_opened_files = "name", -- name, icon, both, none

      },

      -- highlight_opened_files_group = 'NvimTreeOpenedFiles',

      -- Potential fix for the error:
      -- [NvimTree] Could not start the fs_event watcher for path \c\Users\Todor\AppData\Local\nvim\.git : ENOENT
      -- git = {
      --   enable = true,
      --   ignore = true, -- hide files listed in .gitignore file
      -- },

      filters = {
        dotfiles = true, -- hide files starting with a dot
        custom = {
          "compile_commands.json",
          -- ".git",
        },
      },

      -- disable_netrw = true,

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

    vim.cmd("highlight! NvimTreeOpenedFiles guibg=#2a273f guifg=#e0def4")

    --======================================================================--

    --map("n", "<C-n>", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
    -- map("n", "<leader>n", "<cmd>NvimTreeToggle<CR>", { desc = "nvimtree toggle window" })
    vim.keymap.set("n", "<leader>n", function()
        require('nvim-tree.api').tree.toggle({ find_file = true, focus = true })
        -- if util.is_loaded("symbols-outline") then
        --   vim.cmd("SymbolsOutlineClose")
        -- end
      end,
    { desc = "[N]vimtree toggle" })
    --map("n", "<leader>e", "<cmd>NvimTreeFocus<CR>", { desc = "nvimtree focus" })


    -- Keymap to jump to the folder containing the current file under cursor
    --[[vim.keymap.set("n", "<leader>ft", function()
      local api = require("nvim-tree.api")
      local node = api.tree.get_node_under_cursor()
      if node and node.type == "file" and node.parent then
        api.tree.toggle(node.parent) -- Toggle the parent folder open
        api.tree.focus(node.parent) -- Move the cursor to the parent folder
      end
    end, { desc = "Jump to the folder containing current file", silent = true })]]--


    -- Keymap to collapse the folder containing the current file
    vim.keymap.set("n", "<leader>fd", function()
      local api = require("nvim-tree.api")
      local node = api.tree.get_node_under_cursor()
      if node and node.parent and not node.nodes then
        api.node.navigate.parent_close()
      end
    end, { desc = "Collapse folder containing current file", silent = true })

    -- Toggle-able Keymap to toggle the folder containing the current file
    --[[vim.keymap.set("n", "<leader>tf", function()
      local api = require("nvim-tree.api")
      local node = api.tree.get_node_under_cursor()
      if not node then return end

      -- Check if the cursor is over a folder and toggle its state
      if node.type == "directory" then
        if node.open then
          api.node.navigate.parent_close()
        else
          api.node.open.edit()
        end
      elseif node.parent then
        -- For a file, toggle its parent folder
        if node.parent.open then
          api.node.navigate.parent_close()
        else
          api.node.open.edit()
        end
      end
    end, { desc = "Toggle folder containing current file", silent = true })]]--

    -------------------------------------------------------------------------

      --[[vim.api.nvim_create_autocmd("VimResized", {
        desc = "Resize nvim-tree based on the width_ratio percentage",
        callback = function ()
          local view = require("nvim-tree.view")
          if view.is_visible() then
            local new_width = math.floor(vim.o.columns * width_ratio)
            require("nvim-tree").setup({
              view = { width = new_width },
            })
            view.resize(new_width)
          end
        end,
      })]]--

    --======================================================================--

    -- Highlighing groups for current buffer/file
      -- vim.cmd[[ highlight NvimTreeCursorLine guibg=#512C57 gui=bold]]
    --vim.cmd[[ highlight NvimTreeCursorLine guibg=#484069 guifg=#FFFFFF gui=bold]]
    --vim.cmd[[ highlight NvimTreeCursorLineNr guifg=red]]

    --[[
    -- Autocommand for when opening Neovim with a directory
    -- Or when you open a folder like :edit some_folder
    vim.api.nvim_create_autocmd("VimEnter", {
      callback = function()
        -- Check if the current working directory is a directory
        if vim.fn.isdirectory(vim.fn.getcwd()) == 1 then
            --open_nvim_tree()
          require'nvim-tree'.open() -- Load NvimTree if you
        end
      end,
    })
    ]]--


    -- NOTE: Make :q to ignore the tree window when
    -- you have other window opened and you use :quit on it
    -- Is this compatible with nvim-bufdelete ???
    --[[vim.api.nvim_create_autocmd({ 'BufEnter', 'QuitPre' }, {
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
    })]]--


    -- NOTE: Make Nvim-Tree always focus the file that you are currently in
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


    -- TODO: NvimTree sticky header root folder name
    -- (The top most line) , (can be implemented with Winbar)
    --[[vim.api.nvim_create_autocmd("FileType", {
      pattern = "NvimTree",
      callback = function()
        local root_name = require("nvim-tree.core").get_cwd()
        vim.opt_local.winbar = vim.fn.fnamemodify(root_name, ":t") -- Use "" or another icon as a folder symbol
      end,
    })]]--


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
