-- TODO: Fix auto-session to load the session that you cwd into
-- Currently it only loads them, if you pick them from session-lens

-- TODO: Make unsaved buffers to be saved and restored with auto-session
--        or make it at least to notify you of unsaved buffers and prevent
--        you to load the new one on load/restore of a new session
--       For now, i just made it to pre_restore_cmds = {"wa"}
-- https://www.reddit.com/r/neovim/comments/14yhn1f/autosession_removes_modified_buffers/

local function get_session_shada_path()
  -- Use the session name or other unique identifying feature to name the shada
  local session_name = vim.fn.expand('%:p:h')  -- Or modify this to be a more suitable unique name
  return vim.fn.stdpath("data") .. "/sessions/shada_" .. session_name .. ".shada"
end


-- NOTE:
-- These load and save severity or nvim-tree's states are saved
-- globally, NOT per session, but we will use autosession hooks to manage them


-- Path to store the per-session severity.min setting
local diagnostic_severity_file = vim.fn.stdpath("data") .. "/severity_min_state.json"

local function save_severity_min()
  local current_config = vim.diagnostic.config()
  local severity_min = current_config.virtual_text and current_config.virtual_text.severity and current_config.virtual_text.severity.min
  if severity_min then
    local file = io.open(diagnostic_severity_file, "w")
    if file then
      file:write(vim.fn.json_encode({ min = severity_min }))
      file:close()
    end
  end
end

local function load_severity_min()
  local file = io.open(diagnostic_severity_file, "r")
  if file then
    local encoded = file:read("*a")
    file:close()
    local severity_state = vim.fn.json_decode(encoded)
    if severity_state and severity_state.min then
      local config = vim.diagnostic.config() -- Get the current config
      config.virtual_text.severity.min = severity_state.min
      vim.diagnostic.config(config) -- Apply the modified config !!! (because the config value is immutable)
    end
  end
end


-- TODO: Restore NvimTree State per Session

--[[local function save_nvim_tree_state()
  local nvim_tree_api = require("nvim-tree.api")
  local state = nvim_tree_api.tree.is_visible() -- Check if the tree is open
  vim.g.nvim_tree_last_state = state
end

local function restore_nvim_tree_state()
  local nvim_tree_api = require("nvim-tree.api")
  if vim.g.nvim_tree_last_state then
    nvim_tree_api.tree.open() -- Restore nvim-tree if it was open
  else
    nvim_tree_api.tree.close() -- Ensure it's closed otherwise
  end
end]]--

--[[local nvim_tree_state_file = vim.fn.stdpath('data') .. '/nvim_tree_state.json'

local function save_nvim_tree_state()
  local tree = require'nvim-tree.view'

  -- Check if nvim-tree is open and retrieve relevant state
  if tree.is_visible() then
    local state = {
      is_visible = true,
      cwd = vim.fn.getcwd(),  -- Get the current directory of nvim-tree
      file_path = vim.fn.expand('%:p')  -- The file path in nvim-tree
    }

    -- Close it before saving, because if the cursor is in the nvim-tree window
    -- on SessionSave, then on restore we get an error message
    -- require('nvim-tree.api').tree.toggle()

    -- Open or create the JSON file
    local file = io.open(nvim_tree_state_file, "w")
    if file then
      file:write(vim.fn.json_encode(state))
      file:close()
      print("nvim-tree state saved successfully!")
    end
  else
    print("nvim-tree is not open, no state saved.")
  end
end

local function load_nvim_tree_state()
  -- Check if the state file exists
  local file = io.open(nvim_tree_state_file, "r")
  if file then
    local encoded = file:read("*a")
    file:close()

    local tree_state = vim.fn.json_decode(encoded)

    if tree_state and tree_state.is_visible then

      -- Load nvim-tree if available
      local nvim_tree_ok, _ = pcall(require, 'nvim-tree')
      if not nvim_tree_ok then
        print("Error loading nvim-tree")
        return
      end

      -- require'nvim-tree'.toggle()
      require('nvim-tree.api').tree.toggle({ find_file = true, focus = false })

      -- vim.cmd("cd " .. tree_state.cwd)

      -- Optionally, if a file was open in the tree, try to focus on it
      if tree_state.file_path then
        vim.fn.bufload(vim.fn.bufnr(tree_state.file_path)) -- Open the buffer if possible
        vim.cmd("e " .. tree_state.file_path)  -- Open the file in the buffer
      end

    --   print("nvim-tree state loaded successfully!")
    -- else
    --   print("No valid nvim-tree state to load.")
    end
  -- else
  --   print("No nvim-tree state file found.")
  end
end]]--



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
        auto_save = true, -- This auto save doesnt quite work, i had to do an autocmd that saves the session on exit

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

        -- Save and Restore Jumplist

        --[[pre_save_cmds = {
          --"let $SHADA='~/.config/nvim/sessions/session_'.v:sessionname.'.shada'",
          'wshada',
        },

        post_restore_cmds = {
          --"call clearmatches()",  -- Clears visual matches if any exist
          --"execute 'clearjumps'", -- Clears the jumplist
          --"let $SHADA='~/.config/nvim/sessions/session_'.v:sessionname.'.shada'",
          "clearjumps",
          "rshada"               -- Reloads the session-specific ShaDa
        },]]--

        --pre_save_cmds = { "let $SHADA=expand('~/.config/nvim/sessions/session_'..v:sessionname..'.shada')" },

        pre_save_cmds = {
          -- save_nvim_tree_state,
          -- save_severity_min,

          --[[function()
            for _, bufnr in ipairs(vim.api.nvim_list_bufs()) do
              if not vim.bo[bufnr].buflisted then
              -- if vim.bo.bufhidden or not vim.bo.buflisted then
                vim.cmd("bdelete " .. bufnr)
              end
            end
          end,]]--

        },

        post_restore_cmds = {
          "rshada",
          -- load_nvim_tree_state,
          -- load_severity_min,
        },


        --[[pre_save_cmds = {
          function()
            -- Get the session-specific shada path
            local session_shada_path = get_session_shada_path()

            -- Set the session's SHADA variable
            vim.cmd("let $SHADA='" .. session_shada_path .. "'")

            -- Save jumplist (and other shada data) to the session-specific shada file
            vim.cmd("wshada")
          end,
        },

        post_restore_cmds = {
          function()
            -- Get the session-specific shada path
            local session_shada_path = get_session_shada_path()

            -- Load the session-specific shada file for jumplist restoration
            vim.cmd("let $SHADA='" .. session_shada_path .. "'")

            -- Restore jumplist from the session-specific shada file
            vim.cmd("clearjumps")
            vim.cmd("rshada")
          end,
        },]]--



        -- auto-session does not save properly some buffers that
        -- cannot be saved as terminals and others, and on load,
        -- it restores them as empty buffers, so we will do this
        -- TODO: post_restore_cmds = { function() require('utilities').close_empty_buffers() end },

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


        log_level = "error",
        -- log_level = "info",

        session_lens = {
          load_on_setup = true,
--          previewer = true,


          theme_conf = {
            border = true,
            --theme = "ivy",
          },
          mappings = {
            delete_session = { "i", "<C-D>" },
            alternate_session = { "i", "<C-S>" },
            copy_session = { "i", "<C-C>" },

            -- Reverse the order for Tabs, because it is a dropdown window type
            -- , unline the rest of the pickers
            --next_session = { "<Tab>", "<Down>" },
            --prev_session = { "<S-Tab>", "<Up>" },
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
