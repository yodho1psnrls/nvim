-- https://github.com/orgs/nvim-telescope/repositories
-- https://medium.com/@jogarcia/delete-buffers-on-telescope-21cc4cf61b63
-- https://www.reddit.com/r/neovim/comments/qspemc/close_buffers_with_telescope/

-- Ctrl + q will send all telescope window items in the quickfix list
-- Ctrl + d while you browse the buffer finder will delete the selected buffer !
-- Ctrl + / while in telescope window insert mode shows telescope whichkeys
-- ? while in telescope window normal mode shows telescope whichkeys

-- telescope-fzf-native needs a build system like make or ninja
--  so if you switch between different build systems, make sure
--  to update this accordingly

return {

  --{"smartpde/telescope-recent-files"},  -- add it as a dependancy instead

  {
    -- https://github.com/nvim-telescope/telescope-file-browser.nvim
    "nvim-telescope/telescope-file-browser.nvim",
    requires = { "nvim-telescope/telescope.nvim", "nvim-lua/plenary.nvim" }
  },

  { -- Fuzzy Finder (files, lsp, etc)
    'nvim-telescope/telescope.nvim',
    event = 'VimEnter',
    branch = '0.1.x',
    dependencies = {
      'nvim-lua/plenary.nvim',
      'smartpde/telescope-recent-files',

      -- Similar to recent-files, but observes your file access
      --  pattern for even better suggestions
      -- 'nvim-telescope/telescope-frecency.nvim',

      -- 'nvim-telescope/telescope-dap.nvim', --https://github.com/nvim-telescope/telescope-dap.nvim

      { -- If encountering errors, see telescope-fzf-native README for installation instructions
        'nvim-telescope/telescope-fzf-native.nvim',

        -- `build` is used to run some command when the plugin is installed/updated.
        -- This is only run then, not every time Neovim starts up.
        -- build = 'make',
        build = 'ninja',

        -- `cond` is a condition used to determine whether this plugin should be
        -- installed and loaded.
        cond = function()
          --  return vim.fn.executable 'make' == 1
          return vim.fn.executable 'ninja' == 1
        end,
      },
      { 'nvim-telescope/telescope-ui-select.nvim' },

      -- Useful for getting pretty icons, but requires a Nerd Font.
      { 'nvim-tree/nvim-web-devicons',            enabled = vim.g.have_nerd_font },
    },
    config = function()
      -- https://github.com/parmardiwakar150/neovim-config/blob/main/lua/core/plugin_config/telescope.lua#L12
      ListModifiedBuffers = function()
        local all_buffers = vim.api.nvim_list_bufs()
        local modified_buffers = {}
        for _, bufnr in ipairs(all_buffers) do
          --local is_modified = vim.api.nvim_buf_get_option(bufnr, "modified")
          local is_modified = vim.bo[bufnr].modified
          if is_modified then
            local file = vim.api.nvim_buf_get_name(bufnr)
            table.insert(modified_buffers, file)
          end
        end
        return modified_buffers
      end
      UnsavedBuffers = function(f_opts)
        f_opts = f_opts or {}
        require("telescope.pickers").new(f_opts, {
          prompt_title = "Modified Buffers",
          finder = require("telescope.finders").new_table({
            results = ListModifiedBuffers(),
          }),
          sorter = require("telescope.config").values.file_sorter(f_opts),
          previewer = require("telescope.config").values.file_previewer(f_opts),
        }):find()
      end
      vim.keymap.set('n', '<leader>fu', UnsavedBuffers, { desc = 'telescope [U]nsaved buffers' })
      vim.api.nvim_create_user_command('UnsavedBuffers', UnsavedBuffers, { nargs = 0 })

      local telescope = require("telescope")
      telescope.load_extension("recent_files")
      --      telescope.load_extension('dap') --https://github.com/nvim-telescope/telescope-dap.nvim

      -- This may overwrite the <leader><leader> keymap that opens the Buffers
      --      vim.api.nvim_set_keymap("n", "<Leader><Leader>",
      --        [[<cmd>lua telescope.extensions.recent_files.pick()<CR>]],
      --        { noremap = true, silent = true })


      --[[
      -- Optionally, you can define the delete_buffer action if it doesn't exist
      telescope.actions.delete_buffer = function(prompt_bufnr)
            local actions = require('telescope.actions')
            local action_state = require('telescope.actions.state')

            -- Get the selected buffer
            local selection = action_state.get_selected_entry()
            if selection then
              local buf = vim.api.nvim_get_current_buf()
              vim.api.nvim_buf_delete(buf, { force = true })
            end

            -- Close Telescope window
            actions.close(prompt_bufnr)
          end,
      ]] --


      -- Telescope is a fuzzy finder that comes with a lot of different things that
      -- it can fuzzy find! It's more than just a "file finder", it can search
      -- many different aspects of Neovim, your workspace, LSP, and more!
      --
      -- The easiest way to use Telescope, is to start by doing something like:
      --  :Telescope help_tags
      --
      -- After running this command, a window will open up and you're able to
      -- type in the prompt window. You'll see a list of `help_tags` options and
      -- a corresponding preview of the help.
      --
      -- Two important keymaps to use while in Telescope are:
      --  - Insert mode: <c-/>
      --  - Normal mode: ?
      --
      -- This opens a window that shows you all of the keymaps for the current
      -- Telescope picker. This is really useful to discover what Telescope can
      -- do as well as how to actually do it!

      local save_selected_buffer = function()
        --local save_selected_buffer = function(prompt_bufnr)
        local selection = require('telescope.actions.state').get_selected_entry()
        if selection then
          local bufnr = vim.fn.bufnr(selection.value)
          vim.api.nvim_buf_call(bufnr, function()
            vim.cmd('write') -- Save the selected buffer
          end)
        end
      end

      -- Telescope doesnt Refresh on deletion of a buffer
      --  with this custom function, try to fix this
      local function delete_selected_buffer()
        --local function delete_selected_buffer(prompt_bufnr)
        local selection = require('telescope.actions.state').get_selected_entry()
        --local selection = require('telescope.actions.state').get_selected_entry(prompt_bufnr)
        if selection then
          local buf = selection.bufnr
          require('bufdelete').bufdelete(buf, false)
        end
      end

      local actions = require("telescope.actions")

      local function smart_next(prompt_bufnr)
        local picker = require("telescope.actions.state").get_current_picker(prompt_bufnr)
        if picker.sorting_strategy == "ascending" then
          -- reversed list: next = previous
          actions.move_selection_previous(prompt_bufnr)
        else
          actions.move_selection_next(prompt_bufnr)
        end
      end

      local function smart_prev(prompt_bufnr)
        local picker = require("telescope.actions.state").get_current_picker(prompt_bufnr)
        if picker.sorting_strategy == "ascending" then
          -- reversed list: previous = next
          actions.move_selection_next(prompt_bufnr)
        else
          actions.move_selection_previous(prompt_bufnr)
        end
      end

      -- [[ Configure Telescope ]]
      -- See `:help telescope` and `:help telescope.setup()`

      local opts = {
        -- You can put your default mappings / updates / etc. in here
        --  All the info you're looking for is in `:help telescope.setup()`
        --
        defaults = {

          --sorting_strategy = "ascending",
          --initial_mode = "normal",  -- Pick buffers in normal mode

          mappings = {
            -- i = { ['<c-enter>'] = 'to_fuzzy_refine' },
            i = {
              ["<C-j>"] = actions.move_selection_next,
              ["<C-k>"] = actions.move_selection_previous,
              -- ["<A-j>"] = require('telescope.actions').move_selection_next,
              -- ["<A-k>"] = require('telescope.actions').move_selection_previous,

              ["<S-Tab>"] = smart_next,
              ["<Tab>"] = smart_prev,

              ["<C-d>"] = actions.delete_buffer,
              -- ["<C-d>"] = delete_selected_buffer,
              ["<C-a>"] = function() vim.cmd(':wa') end,
              ["<C-s>"] = save_selected_buffer,
            },
          },
        },

        -- pickers = {}
        extensions = {
          ['ui-select'] = {
            require('telescope.themes').get_dropdown(),
          },

          -- File Browser Extension
          file_browser = {

            theme = "ivy",
            -- disables netrw and use telescope-file-browser in its place
            hijack_netrw = true,
            mappings = {
              ["i"] = {
                -- your custom insert mode mappings
              },
              ["n"] = {
                -- your custom normal mode mappings
              },
            },

          },

        },

        pickers = {
          -- Recently Opened Buffers List
          buffers = {
            -- Set sorting strategy for buffers to prioritize recent ones (MRU)
            sort_lastused = true, -- similar to how :ls t shows sorted buffers
            ignore_current_buffer = false, -- Optionally ignore the current buffer (you can toggle this)
            --theme = "dropdown"  -- Adjust for a more compact look

            --initial_mode = "normal",  -- Pick buffers in normal mode
            -- default_selection = "current_buffer",  -- This ensures that the current buffer is selected by default.
          },

        },
      }
      opts.defaults.mappings.n = opts.defaults.mappings.i
      telescope.setup(opts)

      -- Enable Telescope extensions if they are installed
      pcall(telescope.load_extension, 'fzf')
      pcall(telescope.load_extension, 'ui-select')


      --[[
      pcall(telescope.load_extension, 'telescope-file-browser')
      vim.keymap.set("n", "<space>fb", ":Telescope file_browser<CR>")
      -- open file_browser with the path of the current buffer
      vim.keymap.set("n", "<space>fb", ":Telescope file_browser path=%:p:h select_buffer=true<CR>")
      -- Alternatively, using lua API
      vim.keymap.set("n", "<space>fb", function()
        telescope.extensions.file_browser.file_browser()
      end)
      ]] --

      -- See `:help telescope.builtin`
      local builtin = require 'telescope.builtin'
      vim.keymap.set('n', '<leader>sh', builtin.help_tags, { desc = '[S]earch [H]elp' })
      vim.keymap.set('n', '<leader>sk', builtin.keymaps, { desc = '[S]earch [K]eymaps' })
      vim.keymap.set('n', '<leader>sf', builtin.find_files, { desc = '[S]earch [F]iles' })
      -- vim.keymap.set('n', '<leader>ss', builtin.builtin, { desc = '[S]earch [S]elect Telescope' })
      vim.keymap.set('n', '<leader>sw', builtin.grep_string, { desc = '[S]earch current [W]ord' })
      --      vim.keymap.set('n', '<leader>sg', builtin.live_grep, { desc = '[S]earch by [G]rep' })
      vim.keymap.set('n', '<leader>sd', builtin.diagnostics, { desc = '[S]earch [D]iagnostics' })
      vim.keymap.set('n', '<leader>sr', builtin.resume, { desc = '[S]earch [R]esume' })
      vim.keymap.set('n', '<leader>s.', builtin.oldfiles, { desc = '[S]earch Recent Files ("." for repeat)' })
      vim.keymap.set('n', '<leader><leader>', builtin.buffers, { desc = '[ ] Find existing buffers' })

      -- Symbols Outline Telescope Picker
      vim.keymap.set("n", "<leader>so", require("telescope.builtin").lsp_document_symbols,
        { desc = "LSP Symbols", noremap = true, silent = true })

      -- Slightly advanced example of overriding default behavior and theme
      vim.keymap.set('n', '<leader>/', function()
        -- You can pass additional configuration to Telescope to change the theme, layout, etc.
        builtin.current_buffer_fuzzy_find(require('telescope.themes').get_dropdown {
          winblend = 10,
          previewer = false,
        })
      end, { desc = '[/] Fuzzily search in current buffer' })

      -- It's also possible to pass additional configuration options.
      --  See `:help telescope.builtin.live_grep()` for information about particular keys
      vim.keymap.set('n', '<leader>s/', function()
        builtin.live_grep {
          grep_open_files = true,
          prompt_title = 'Live Grep in Open Files',
        }
      end, { desc = '[S]earch [/] in Open Files' })

      -- Shortcut for searching your Neovim configuration files
      vim.keymap.set('n', '<leader>sn', function()
        builtin.find_files { cwd = vim.fn.stdpath 'config' }
      end, { desc = '[S]earch [N]eovim files' })


      -- telescope
      vim.keymap.set("n", "<leader>fg", "<cmd>Telescope live_grep<CR>", { desc = "telescope [G]rep" })
      vim.keymap.set("n", "<leader>fb", "<cmd>Telescope buffers<CR>", { desc = "telescope [B]uffers" })
      vim.keymap.set("n", "<leader>fh", "<cmd>Telescope help_tags<CR>", { desc = "telescope [H]elp page" })
      --vim.keymap.set("n", "<leader>ma", "<cmd>Telescope marks<CR>", { desc = "telescope find marks" })
      vim.keymap.set("n", "<leader>fm", "<cmd>Telescope marks<CR>", { desc = "telescope [M]arks" })
      vim.keymap.set("n", "<leader>fo", "<cmd>Telescope oldfiles<CR>", { desc = "telescope [O]ldfiles" })

      -- vim.keymap.set("n", "<leader>fz", "<cmd>Telescope current_buffer_fuzzy_find<CR>",
      --   { desc = "telescope fu[Z]zy find" })
      vim.keymap.set("n", "<leader>fi", "<cmd>Telescope current_buffer_fuzzy_find<CR>",
        { desc = "telescope [I]n file" })

      vim.keymap.set("n", "<leader>gc", "<cmd>Telescope git_commits<CR>", { desc = "telescope git commits" })
      vim.keymap.set("n", "<leader>gs", "<cmd>Telescope git_status<CR>", { desc = "telescope git status" })
      vim.keymap.set("n", "<leader>gg", "<cmd>Gitsigns<CR>", { desc = "telescope git signs" })
      vim.keymap.set("n", "<leader>gf", require('telescope.builtin').git_files, { desc = "telescope git files" })
      vim.keymap.set("n", "<leader>gd", "<cmd>Gitsigns diffthis<CR>", { desc = "gitsigns diff current buffer" })

      -- vim.keymap.set("n", "<leader>pt", "<cmd>Telescope terms<CR>", { desc = "telescope pick hidden term" })

      -- This is NvChad Specific
      --vim.keymap.set("n", "<leader>th", "<cmd>Telescope themes<CR>", { desc = "telescope nvchad themes" })

      vim.keymap.set("n", "<leader>ff", "<cmd>Telescope find_files<cr>", { desc = "telescope [F]iles" })
      vim.keymap.set(
        "n",
        "<leader>fa",
        "<cmd>Telescope find_files follow=true no_ignore=true hidden=true<CR>",
        { desc = "telescope [A]ll files" }
      )
      --[[vim.keymap.set("n", "<leader>fm", function()
        require("conform").format { lsp_fallback = true }
      end, { desc = "General Format file" })]] --

      -- This doesnt save any buffers, nor tell you for unsaved buffers between switching sessions !
      vim.keymap.set("n", "<leader>fs", '<cmd>Telescope session-lens search_session<CR>',
        { desc = "telescope [S]essions" })

      vim.keymap.set("n", "<leader>fj", '<cmd>Telescope jumplist<CR>',
        { desc = "telescope [J]umplist" })

      vim.keymap.set("n", "<leader>cs", '<cmd>Telescope colorscheme<CR>',
        { desc = "color[S]chemes" })
      -- vim.keymap.set("n", "<leader>fc", '<cmd>Telescope colorscheme<CR>',
      --   { desc = "telescope [C]olorschemes" })
      -- vim.keymap.set("n", "<leader>ft", '<cmd>Telescope colorscheme<CR>',
      --   { desc = "telescope [T]hemes" })


      -- https://www.reddit.com/r/neovim/comments/ypaq3e/lsp_find_reference_results_in_telescope/
      -- https://github.com/Slotos/telescope-lsp-handlers.nvim
      vim.keymap.set('n', '<leader>fr', function() require('telescope.builtin').lsp_references() end,
        { noremap = true, silent = true, desc = "LSP find [R]eferences" })

      -- https://github.com/parmardiwakar150/neovim-config/blob/main/lua/core/plugin_config/telescope.lua#L37
      vim.keymap.set('n', '<leader>ch', function() require('telescope.builtin').command_history() end,
        { noremap = true, silent = true, desc = "Command [H]istory" })

			vim.keymap.set('n', '<leader>ft', '<cmd>TodoTelescope<CR>',
        { noremap = true, silent = true, desc = "telescope [T]oDo" })

    end,
  }


}


------------------------ Old Configurations from KeyMaps ------------------------------

-- This saves all buffers, when you open the session_lens window
--vim.keymap.set("n", "<leader>fs", function() vim.cmd('wa') require'telescope'.session_lens.search_session() end, { desc = "telescope session lens" })
--vim.keymap.set("n", "<leader>fs", function() vim.cmd('wa') vim.cmd('Telescope session-lens search_session') end, { desc = "telescope session lens" })

-- This saves all buffers only, if you actually sellect to switch to another session !!!
--  and not just saving all buffers only if you just had opened the window
--  (you may had buffers that you dont want to save)
--[[
local actions = require('telescope.actions')
--local session_lens = require('session-lens')
vim.keymap.set("n", "<leader>fs", function()
  require('telescope.builtin').session_lens({
    on_action = function(prompt_bufnr)
      vim.cmd('wa')  -- Save all buffers
      actions.select_default(prompt_bufnr)  -- Then select the session
    end,
  })
end, { desc = "telescope session lens with save buffers" })
]] --
--[[
vim.keymap.set("n", "<leader>fs", function()
  -- Check for unsaved buffers
  local unsaved_buffers = false
  local unsaved_bufnr = nil

  -- Iterate over all buffers to check for unsaved ones
  for _, bufnr in ipairs(vim.api.nvim_list_bufs()) do
    if vim.api.nvim_buf_get_option(bufnr, 'modified') and vim.api.nvim_buf_is_loaded(bufnr) then
      unsaved_buffers = true
      unsaved_bufnr = bufnr
      break
    end
  end

  -- If there are unsaved buffers, warn and highlight the unsaved buffer
  if unsaved_buffers then
    -- Highlight the unsaved buffer
    vim.api.nvim_set_current_buf(unsaved_bufnr)

    -- Show a warning message similar to :qa
    vim.notify("Warning: You have unsaved changes! Please save them before continuing.", vim.log.levels.WARN)

    -- Optionally, return early to prevent opening session lens
    return
  end

  -- If no unsaved buffers, save all buffers and open session lens
  vim.cmd("wa")
  vim.cmd("Telescope session-lens search_session")
end, { desc = "Telescope session lens with unsaved buffer check" })
]] --
