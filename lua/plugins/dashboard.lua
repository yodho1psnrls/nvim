-- https://www.reddit.com/r/neovim/comments/17pzdxs/how_do_you_customize_the_dashboard_in_lazyvim/

function CreateNewProject()
  -- Prompt for the destination path
  local path = vim.fn.input('Enter path for the new cpp project: ')

  if path == '' then
    print("Cancelled the new cpp project.")
    return
  end

  -- Define source and destination
  --local source_dir = vim.fn.expand('~/AppData/Local/nvim/new_cpp_project/*')
  local source_dir = vim.fn.expand('~/AppData/Local/nvim/new_cpp_project')
  local destination_dir = vim.fn.expand(path)

  -- Remove trailing slash if present
  destination_dir = destination_dir:gsub("/$", "")

  -- Execute the commands
  os.execute('cp -r ' .. source_dir .. ' ' .. destination_dir)
  --os.execute('cd ' .. destination_dir)
  vim.cmd('cd ' .. destination_dir)
  --vim.cmd('edit ' .. destination_dir .. '/src/main.cpp')
  vim.cmd('edit src/main.cpp')

  -- Notify user
  print('New project created at: ' .. destination_dir)
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

      config = {
        week_header = {
          enable = true,
        },

        shortcut = {

          {
            desc = 'Update', --'󰊳 Update'
            group = '@property',
            action = 'Lazy update',
            key = 'u'
          },

          -- Add your custom shortcut to open the Neovim config folder
          {
            desc = 'Config',                             -- Description of the shortcut
            group = '@property',                         -- Highlight group for the text
            action = 'cd ~/AppData/Local/nvim | edit .', -- Action to open the config folder
            key = 'c',                                   -- Shortcut key (in this case, 'c')
          },

          --NEW PROJECT Shortcut
          {
            --desc = 'New Project',               -- Description for the new project option
            desc = 'New',
            group = 'DashboardPureWhite',       -- Highlight group
            action = ':lua CreateNewProject()', -- Lua function to handle project creation
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

          {
            desc = ' Apps', -- Shortcut for "Apps"
            group = 'DiagnosticHint',
            action = function()
              require('telescope.builtin').commands() -- Open available commands (like apps)
            end,
            key = 'a',
          },

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


        footer = {
          --	'Your custom footer message here.',
          --	'Another line for footer if needed.'
        },

      },


    },

  }

}
