-- TODO: Fix auto-session to load the session that you cwd into
-- Currently it only loads them, if you pick them from session-lens

-- TODO: Make unsaved buffers to be saved and restored with auto-session
--        or make it at least to notify you of unsaved buffers and prevent
--        you to load the new one on load/restore of a new session
--       For now, i just made it to pre_restore_cmds = {"wa"}
-- https://www.reddit.com/r/neovim/comments/14yhn1f/autosession_removes_modified_buffers/

return {

  {
    'rmagatti/auto-session',

    -- You might say, "But this wont autoload when you enter
    --  a directory with a session in it", but actually currently
    --  if you dont explicitly load a session, it still wont load
    --  it (when you just enter a folder with a session)
    -- I dont know, why this problem occurs
    -- So Lazy Loading it, wont change its current behaiviour at all
    lazy = true,
    --event = "BufReadPre", -- https://github.com/folke/persistence.nvim
    cmd = { 'Telescope session-lens search_session' },
    --action = { 'Telescope session-lens search_session' },

    dependencies = {
      'nvim-telescope/telescope.nvim', -- Only needed if you want to use session lens
    --  'nvim-tree/nvim-tree.lua',       -- In order for nvim-tree to close
    },

    config = function()
      require('auto-session').setup({
        enabled = true,
        --enabled = false,  -- DISABLED FOR NOW

        root_dir = vim.fn.stdpath "data" .. "/sessions/",
        auto_save = true,

        --auto_restore = true,  -- restores you from previos session on startup
        auto_restore = false, -- Disable automatic session restoration on startup so dashboard shows up

        auto_create = true,
        --allowed_dirs = {'D:/'},
        allowed_dirs = nil,
        suppressed_dirs = nil,

        --interferes with the Telescope session-lens session previewer/picker
        --auto_restore_last_session = true,
        auto_restore_last_session = false, -- Set to false so dashboard shows up

        use_git_branch = true, -- false
        lazy_support = true,
        --bypass_save_filetypes = nil,
        bypass_save_filetypes = { 'alpha', 'dashboard' }, -- Does not save session on dashboard
        close_unsupported_windows = true,
        args_allow_single_directory = true,
        args_allow_files_auto_save = true,
        continue_restore_on_error = true,

        -- NOTE: Fix the issue that it doesnt save the session on exit when nvim-tree is open
        -- And that when you cd to a folder it doesnt restore the last session

        -- NOTE: An Auto Session is not loaded when changing directories,
        -- its only loaded if loaded from session-lens

        -- This is what closes nvim-tree on cwd change !!
--        cwd_change_handling = true,
--        pre_cwd_changed_cmds = { "NvimTreeClose" },

        cwd_change_handling = false,
--        pre_cwd_changed_cmds = nil,
--        pre_save_cmds = { "NvimTreeClose" },


        -- Command Hooks:
        --pre_cwd_changed_cmds
        --pre_save_cmds
        --post_save_cmds
        --pre_delete_cmds
        --post_delete_cmds
        pre_restore_cmds = { "wa" },

        -- auto-session does not save properly some buffers that
        -- cannot be saved as terminals and others, and on load,
        -- it restores them as empty buffers, so we will do this
        --TODO: post_restore_cmds = { function() require('utilities').close_empty_buffers() end },

        --post_cwd_changed_cmds

        --[[
        post_restore_cmds = {
          function()
            -- Restore nvim-tree after a session is restored
            local nvim_tree_api = require('nvim-tree.api')
            nvim_tree_api.tree.open()
            nvim_tree_api.tree.change_root(vim.fn.getcwd())
            nvim_tree_api.tree.reload()
          end
        },
        ]]--

        -- NOTE:
        -- With this you can save the nvim-tree state
        -- See the example (but with quickfix list) in the repo readme
--        save_extra_cmds = {...}


        --log_level = "error",
        log_level = "info",

        session_lens = {
          load_on_setup = true,
--          previewer = true,

          theme_conf = {
            border = true,
          },
          mappings = {
            delete_session = { "i", "<C-D>" },
            alternate_session = { "i", "<C-S>" },
            copy_session = { "i", "<C-C>" },
          },
          session_control = {
            control_dir = vim.fn.stdpath "data" .. "/auto_session/",
            control_filename = "session_control.json",
          },
        },
      })
    end,


  }

}
