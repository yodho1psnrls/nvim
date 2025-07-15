-- Adds git related signs to the gutter, as well as utilities for managing changes
-- NOTE: gitsigns is already included in init.lua but contains only the base
-- config. This will add also the recommended keymaps.

-- https://www.reddit.com/r/neovim/comments/146n4yy/what_is_the_smartest_condition_you_have_used_to/?rdt=57944
-- https://www.reddit.com/r/neovim/comments/1frxtv4/looking_for_conventional_commit_plugin/

return {

  {
    'lewis6991/gitsigns.nvim',

    -- The actual lazy load logic is in lua/config/autocmds.lua
    lazy = true,
    -- event = { "BufReadPre", "BufNewFile" },  -- load before buffer opens

    -- Lazy load it, when you change directories and it happens
    --  that there is a .git folder inside your current working directory
    --[[
    --event = 'BufRead'
    event = 'DirChanged',
    cond = function()
      -- Check if the current directory contains a .git directory
      return vim.fn.isdirectory(vim.fn.expand('.git')) == 1
    end,
    ]]--

    opts = {
      -- NOTE: Defaults
      --[[signs = {
        add          = { text = '┃' },
        change       = { text = '┃' },
        delete       = { text = '_' },
        topdelete    = { text = '‾' },
        changedelete = { text = '~' },
        untracked    = { text = '┆' },
      },
      signs_staged = {
        add          = { text = '┃' },
        change       = { text = '┃' },
        delete       = { text = '_' },
        topdelete    = { text = '‾' },
        changedelete = { text = '~' },
        untracked    = { text = '┆' },
      },
      signs_staged_enable = true,
      signcolumn = true,  -- Toggle with `:Gitsigns toggle_signs`
      numhl      = false, -- Toggle with `:Gitsigns toggle_numhl`
      linehl     = false, -- Toggle with `:Gitsigns toggle_linehl`
      word_diff  = false, -- Toggle with `:Gitsigns toggle_word_diff`
      watch_gitdir = {
        follow_files = true
      },
      auto_attach = true,
      attach_to_untracked = false,
      current_line_blame = false, -- Toggle with `:Gitsigns toggle_current_line_blame`
      current_line_blame_opts = {
        virt_text = true,
        virt_text_pos = 'eol', -- 'eol' | 'overlay' | 'right_align'
        delay = 1000,
        ignore_whitespace = false,
        virt_text_priority = 100,
        use_focus = true,
      },
      current_line_blame_formatter = '<author>, <author_time:%R> - <summary>',
      sign_priority = 6,
      update_debounce = 100,
      status_formatter = nil, -- Use default
      max_file_length = 40000, -- Disable if file is longer than this (in lines)
      preview_config = {
        -- Options passed to nvim_open_win
        style = 'minimal',
        relative = 'cursor',
        row = 0,
        col = 1
      },]]--

      on_attach = function(bufnr)
        local gitsigns = require 'gitsigns'

        local function map(mode, l, r, opts)
          opts = opts or {}
          opts.buffer = bufnr
          vim.keymap.set(mode, l, r, opts)
        end

        -- Navigation
        map('n', ']c', function()
          if vim.wo.diff then
            vim.cmd.normal { ']c', bang = true }
          else
            gitsigns.nav_hunk 'next'
          end
        end, { desc = 'Jump to next git [c]hange' })

        map('n', '[c', function()
          if vim.wo.diff then
            vim.cmd.normal { '[c', bang = true }
          else
            gitsigns.nav_hunk 'prev'
          end
        end, { desc = 'Jump to previous git [c]hange' })

        -- Actions
        -- visual mode
        map('v', '<leader>hs', function()
          gitsigns.stage_hunk { vim.fn.line '.', vim.fn.line 'v' }
        end, { desc = 'stage git hunk' })
        map('v', '<leader>hr', function()
          gitsigns.reset_hunk { vim.fn.line '.', vim.fn.line 'v' }
        end, { desc = 'reset git hunk' })
        -- normal mode
        map('n', '<leader>hs', gitsigns.stage_hunk, { desc = 'git [s]tage hunk' })
        map('n', '<leader>hr', gitsigns.reset_hunk, { desc = 'git [r]eset hunk' })
        map('n', '<leader>hS', gitsigns.stage_buffer, { desc = 'git [S]tage buffer' })
        map('n', '<leader>hu', gitsigns.undo_stage_hunk, { desc = 'git [u]ndo stage hunk' })
        map('n', '<leader>hR', gitsigns.reset_buffer, { desc = 'git [R]eset buffer' })
        map('n', '<leader>hp', gitsigns.preview_hunk, { desc = 'git [p]review hunk' })
        map('n', '<leader>hb', gitsigns.blame_line, { desc = 'git [b]lame line' })
        map('n', '<leader>hd', gitsigns.diffthis, { desc = 'git [d]iff against index' })
        map('n', '<leader>hD', function()
          gitsigns.diffthis '@'
        end, { desc = 'git [D]iff against last commit' })
        -- Toggles
        map('n', '<leader>tb', gitsigns.toggle_current_line_blame, { desc = '[T]oggle git show [b]lame line' })
        map('n', '<leader>tD', gitsigns.toggle_deleted, { desc = '[T]oggle git show [D]eleted' })
      end,
    },

    config = function(_, opts)
     require('gitsigns').setup(opts)
    end,

  },


  --[[{
    -- https://neovimcraft.com/plugin/isak102/telescope-git-file-history.nvim/
    -- https://github.com/isak102/telescope-git-file-history.nvim
    "isak102/telescope-git-file-history.nvim",
    dependencies = {
      "nvim-lua/plenary.nvim",
      "tpope/vim-fugitive",
      "nvim-telescope/telescope.nvim"
    },
    config = function ()
      require("telescope").load_extension("git_file_history")
    end
  },
  ]]--

  --[[
  {
    'ThePrimeagen/git-worktree.nvim',
    config = function ()
      require("git-worktree").setup({
        change_directory_command = 'cd',  -- default: "cd",
        update_on_change = true,          -- default: true,
        update_on_change_command = 'e .', -- default: "e .",
        clearjumps_on_change = true,      -- default: true,
        autopush = false,                 -- default: false,
      })
      
      -- Creates a worktree.  Requires the path, branch name, and the upstream
      -- Example:
      :lua require("git-worktree").create_worktree("feat-69", "master", "origin")

      -- switches to an existing worktree.  Requires the path name
      -- Example:
      :lua require("git-worktree").switch_worktree("feat-69")

      -- deletes to an existing worktree.  Requires the path name
      -- Example:
      :lua require("git-worktree").delete_worktree("feat-69")

    end,
  },
  ]]--

  --{
    -- https://github.com/parmardiwakar150/neovim-config/blob/main/lua/core/plugin_config/diffview.lua
    -- https://github.com/sindrets/diffview.nvim
  --},

}

