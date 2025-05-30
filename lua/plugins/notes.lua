return {
	{
		'forest-nvim/maple.nvim',
		lazy = true,
		cmd = {
			'MapleNotes',
			'Notes'
		},
		keys = {
			{'<leader>j', desc = "Toggle Maple Notes"}
		},
		config = function()
			require('maple').setup({
				-- Appearance
				width = 0.6725,        -- (0.6) Width of the popup (ratio of the editor width)
				height = 0.6725,       -- (0.6) Height of the popup (ratio of the editor height)
				border = 'rounded', -- Border style ('none', 'single', 'double', 'rounded', etc.)
				title = ' maple ',
				title_pos = 'center',
				winblend = 10,       -- Window transparency (0-100)
				show_legend = false, -- Whether to show keybind legend in the UI

				-- Storage
				storage_path = vim.fn.stdpath('data') .. '/notes',

				-- Notes management
				notes_mode = "project",            -- "global" or "project"
				use_project_specific_notes = true, -- Store notes by project

				-- Keymaps (set to nil to disable)
				keymaps = {
					-- toggle = '<leader>on',      -- Key to toggle Maple
					-- toggle = '<leader><CR>',
					toggle = '<leader>j',
					-- toggle = '<leader>z',
					-- close = 'q',               -- Key to close the window
					close = '<Esc>',
					-- close = '<C-c>',
					-- switch_mode = 'm',         -- Key to switch between global and project view
					switch_mode = '<Tab>',
				}
			})

			vim.api.nvim_create_user_command("Notes", function()
				vim.cmd('MapleNotes')
			end, {desc = 'Toggle Maple Notes'})

		end
	}
}
