-- https://www.reddit.com/r/neovim/comments/17pzdxs/how_do_you_customize_the_dashboard_in_lazyvim/

-- TODO:
-- (*) Make the footer (recent files) to update on clear cache shada
-- (*) Make the clock on the dashboard to update every second
--     , so it displays the current time, not the time of opening

-- Function to close all windows except the current one
local function close_all_splits()
  local current_win = vim.api.nvim_get_current_win()
  local windows = vim.api.nvim_list_wins()

  if #windows > 1 then
    for _, win in ipairs(windows) do
      if win ~= current_win then
        vim.api.nvim_win_close(win, true)
      end
    end
  end
end

local function open_dashboard()
  close_all_splits()
  vim.cmd('Dashboard') -- Adjust this if your dashboard plugin uses a different command
end


-- Some color cmds for the shortcuts (website: https://www.rapidtables.com/web/color/RGB_Color.html)
vim.cmd [[highlight DashboardLastSession guifg=#31DEC1 ctermfg=Green]]
--vim.cmd [[highlight DashboardSessions guifg=#FCB767 ctermfg=215]]  -- 215 is closest to #FCB767
vim.cmd [[highlight DashboardSessions guifg=#FFC786]]

vim.cmd [[highlight DashboardPureWhite guifg=#FFFFFF]]


return {

  {
    "nvimdev/dashboard-nvim",
    opts = {

      theme = 'hyper',

      disable_move = true, -- Make this true, when using hyper theme
      shortcut_type = 'letter', -- letter | number
      change_to_vcs_root = true,  -- default is false, for open file in hyper mru, it will change to the root of vcs

      config = {

        week_header = {
          enable = true,
        },

        shortcut = {

          {
            -- desc = '⏫ Update', --'󰊳 Update' 🔄 🔃 🔗 ⬆️ ⏫ 🔼 📈
            desc = 'Update',
            group = '@property',
            action = 'Lazy update',
            key = 'u'
          },

          -- Add your custom shortcut to open the Neovim config folder
          {
            -- desc = '⚙️  Config', -- Description of the shortcut 🔧
            desc = 'Config',
            group = '@property', -- Highlight group for the text
            -- group = 'DiagnosticHint',
            action = 'Config', -- Action to open the config folder
            key = 'c', -- Shortcut key (in this case, 'c')
          },

          --NEW PROJECT Shortcut
          {
            --desc = 'New Project',               -- Description for the new project option
            -- desc = '➕ New',
            desc = "New",
            group = 'DashboardPureWhite',       -- Highlight group
            -- action = ':lua CreateNewProject()', -- Lua function to handle project creation
            action = 'NewCppProject',
            key = 'n',                          -- Shortcut key for the action
          },

    --[[
    {
        --desc = 'Last Session',
        desc = 'Restore',
    --group = 'Label',
        --group = 'Success',  -- Group for green color / But it makes it Grey
    group = 'DashboardLastSession',
    
    -- Restore the last session using auto-session
        action = 'SessionRestore',
    --key = 'l',
    --key = '<Esc>',  -- Use the Esc key
        key = 'r',
    },
    ]] --

        --[[
        {
          icon = ' ',
          icon_hl = '@variable',
          desc = 'Files',
          group = 'Label',
          action = 'Telescope find_files',
          key = 'f',
        },
        ]] --

          {
            -- desc = '💾 Sessions',
            desc = 'Sessions',
            --group = 'Label',
            group = 'DashboardSessions',
            -- Open Telescope session-lens to search sessions
            action = 'Telescope session-lens search_session',
            key = 's',
          },

          -- Blank line to separate the shortcuts and put the next one on a new line
          -- Add a blank space for separation without causing issues
          -- { desc = '', action = '' },  -- empty shortcut to force a line break

          --[[
          {
            desc = ' Apps', -- Shortcut for "Apps"
            group = 'DiagnosticHint',
            action = function()
              require('telescope.builtin').commands() -- Open available commands (like apps)
            end,
            key = 'a',
          },
          ]] --
          {
            -- desc = '🧹 Free',
            desc = 'Free',
            group = 'DiagnosticHint',
            action = function()
              ClearCache()

              if vim.g.dashboard then vim.g.dashboard.refresh() end
              -- vim.cmd('Dashboard')
              --vim.cmd('redraw')
            end,
            key = 'r'
          }

          --[[
    {
        desc = ' dotfiles',  -- Shortcut for dotfiles
        group = 'Number',
        action = function()
          require('telescope.builtin').find_files({
            prompt_title = "< Dotfiles >",
            cwd = "~/.config/nvim",  -- Path to your dotfiles
          })
        end,
        key = 'd',
        },
    ]] --




        },

        packages = { enable = true }, -- show how many plugins neovim loaded


        -- limit how many projects list, action when you press key or enter it will run this action.
        -- action can be a functino type, e.g.
        -- action = func(path) vim.cmd('Telescope find_files cwd=' .. path) end

        --project = { enable = true, limit = 8, icon = 'New Project:', label = '', action = 'Telescope find_files cwd=' },
        --mru = { limit = 10, icon = 'Recent Files:', label = '', cwd_only = false },

        project = {
          enable = true, -- Enable the projects list
          limit = 5, -- Limit the number of projects displayed
          icon = ' ', -- Icon to display for the projects section
          label = 'Projects', -- Label for the projects section
          action = 'Telescope find_files cwd=', -- Action when selecting a project
        },

        -- Replacing the project section with a sessions section
        --[[
    project = {
      enable = true,
      limit = 5,
      icon = ' ',
      label = 'Last Sessions',
      action = function(_, index)
        local sessions = get_sessions()
        if sessions and sessions[index] then
          print("Loading session: " .. sessions[index])  -- Debugging line
          -- Using the correct session restore command
          vim.cmd('SessionRestore ' .. vim.fn.fnameescape(sessions[index]))
        else
          print("No session found at index: " .. index)
        end
      end,
      },
    ]] --

        mru = {
          enable = true, -- Enable the most recent files list
          limit = 10, -- Limit the number of recent files displayed
          icon = ' ', -- Icon to display for recent files section
          label = 'Recent Files', -- Label for the recent files section
          action = 'Telescope find_files cwd=', -- Action when selecting a recent file
        },

        --[[footer = {
          --  'Your custom footer message here.',
          --  'Another line for footer if needed.'
        },]]--

      },


    },

    config = function(_, opts)
      require('dashboard').setup(opts) -- Pass the opts properly
      vim.keymap.set('n', '<leader>db', open_dashboard,
        { noremap = true, silent = true, desc = "Open Dashboard" })
    end

    --[[config = function ()
    require('dashboard').setup(opts)
      --require('dashboard').setup(opts)
      vim.keymap.set('n', '<leader>db', open_dashboard, { noremap = true, silent = true })
      --map('n', '<leader>db', ':Dashboard<CR>', opts)
    end]]--

  }

}
