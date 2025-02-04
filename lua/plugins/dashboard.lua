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

-- Function to delete all files in the selected folder
function DeleteFilesInFolder(folder)
  local folder_path = vim.fn.expand("~") .. "/AppData/Local/nvim-data/" .. folder

  -- Use vim.fn.isdirectory to check if the folder exists
  if vim.fn.isdirectory(folder_path) == 1 then
    -- List all files and directories in the folder
    local files = vim.fn.globpath(folder_path, '*', false, true)

    -- Check if the folder contains files
    if #files == 0 then
      print("No files found in " .. folder)
      return
    end

    -- Confirm before deleting files
    local confirm = vim.fn.confirm("Are you sure you want to delete all files in " .. folder .. "?", "&Yes\n&No", 2)
    if confirm == 1 then
      -- Delete all files in the folder
      for _, file in ipairs(files) do
        vim.fn.delete(file, "rf")
      end
      print("All files in " .. folder .. " have been deleted.")
    else
      print("Deletion cancelled.")
    end
  else
    print(folder .. " does not exist.")
  end
end

-- Function to list and select folders using Telescope
function ClearCache()
  -- List of folders to choose from
  local cache_folders = { "shada", "swap", "sessions", "undo" }

  -- Telescope entry maker for the cache folders
  local make_entry = require('telescope.make_entry')

  -- Open Telescope with these folders for selection
  require('telescope.pickers').new({}, {
    prompt_title = "Select Cache Folder to Clear",
    finder = require('telescope.finders').new_table({
      results = cache_folders,
      entry_maker = make_entry.gen_from_string(), -- Entry maker for string-based entries
    }),
    sorter = require('telescope.sorters').get_generic_fuzzy_sorter(),
    --    attach_mappings = function(prompt_bufnr, map)
    attach_mappings = function(_, map)
      map('i', '<CR>', function()
        local selection = require('telescope.actions.state').get_selected_entry()
        if selection then
          -- Delete all files in the selected folder
          DeleteFilesInFolder(selection.value)
          -- Optional:
          -- vim.cmd('Telescope close')
          -- Update/Redraw/Refresh the dashboard
          -- vim.cmd('DashboardUpdateFooter')
          -- require("dashboard").refresh()
        end
      end)
      return true
    end,
  }):find()
end

-- Create a command to run the ClearCache function
vim.api.nvim_create_user_command('ClearCache', ClearCache, {})



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
            action = 'cd ~/AppData/Local/nvim | edit .', -- Action to open the config folder
            key = 'c', -- Shortcut key (in this case, 'c')
          },

          --NEW PROJECT Shortcut
          {
            --desc = 'New Project',               -- Description for the new project option
            -- desc = '➕ New',
            desc = "New",
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


        footer = {
          --	'Your custom footer message here.',
          --	'Another line for footer if needed.'
        },

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
