-- https://github.com/nvim-tree/nvim-tree.lua
-- https://medium.com/@shaikzahid0713/file-explorer-for-neovim-c324d2c53657
-- https://github.com/nvim-tree/nvim-tree.lua/wiki/Recipes#workaround-when-using-rmagattiauto-session

-- TODO: Make the Root Folder Name to always apper at the Top
--        when scroll down and the first line of nvim-tree
--        is no longer visible 

return {

  { "nvim-tree/nvim-web-devicons" }, -- it is a dependancy for other plugins

  {
    "nvim-tree/nvim-tree.lua",

    -- Lazy-load when one of these commands is used
--    lazy = true,
--    cmd = { 'NvimTreeToggle', 'NvimTreeFocus', 'NvimTreeFindFile' },

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
      ]]--

    },

  },

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
]]--

}

