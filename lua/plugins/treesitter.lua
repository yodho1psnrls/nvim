return {

  -- TODO: You can try to lazy load it, not on LspAttach, but on
  -- opening a file that is one of the types listed in ensure_installed

  { -- Highlight, edit, and navigate code
    'nvim-treesitter/nvim-treesitter',
    lazy = true,
    event = "LspAttach",

    -- NOTE: Make the sticky headers load, when treesitter is loaded
    -- These are not dependencies, but just plugins which are lazy loaded
    -- when treesitter gets loaded
    dependencies = {
      "nvim-treesitter/nvim-treesitter-context",
      -- "lukas-reineke/indent-blankline.nvim",
    },

    build = ':TSUpdate',
    main = 'nvim-treesitter.configs', -- Sets main module to use for opts
    -- [[ Configure Treesitter ]] See `:help nvim-treesitter`
    opts = {
      --ensure_installed = { 'bash', 'c', 'diff', 'html', 'lua', 'luadoc', 'markdown', 'markdown_inline', 'query', 'vim', 'vimdoc' },
      ensure_installed = {'c', 'cpp', 'glsl', 'hlsl', 'python', 'lua', 'luadoc', 'vim', 'vimdoc', 'bash', 'html'},
      -- Autoinstall languages that are not installed
      auto_install = true,
      highlight = {
        enable = true,
        -- Some languages depend on vim's regex highlighting system (such as Ruby) for indent rules.
        --  If you are experiencing weird indenting issues, add the language to
        --  the list of additional_vim_regex_highlighting and disabled languages for indent.
        additional_vim_regex_highlighting = { 'ruby' },
      },
      indent = { enable = true, disable = { 'ruby' } },
    },
    -- There are additional nvim-treesitter modules that you can use to interact
    -- with nvim-treesitter. You should go explore a few and see what interests you:
    --
    --    - Incremental selection: Included, see `:help nvim-treesitter-incremental-selection-mod`
    --    - Show your current context: https://github.com/nvim-treesitter/nvim-treesitter-context
    --    - Treesitter + textobjects: https://github.com/nvim-treesitter/nvim-treesitter-textobjects


    -- Lazy load it on LspAttach
    --[[
    config = function(_, opts)

      local function on_attach(_, _)
        require('nvim-treesitter.configs').setup(opts)
      end

      vim.api.nvim_create_autocmd("LspAttach", {
        callback = on_attach,
      })

    end,
    ]]--

    --[[-- config = function()
    config = function(_, opts)
      require('nvim-treesitter').setup(opts)
      require('nvim-treesitter.configs').setup {
        indent = {
          enable = true
        }
      }
      vim.g.indent_blankline_use_treesitter = true
      vim.g.indent_blankline_show_current_context = true
    end]]--

  },

--[[
{
  "nvim-treesitter/nvim-treesitter",
  opts = {
  
    -- A list of parser names, or "all" (the listed parsers MUST always be installed)
    --ensure_installed = { "c", "lua", "vim", "vimdoc", "query", "markdown", "markdown_inline" },
    ensure_installed = {"c", "cpp", "glsl", "hlsl", "python", "lua", "vim" },

    -- Install parsers synchronously (only applied to `ensure_installed`)
    sync_install = false,

    -- Automatically install missing parsers when entering buffer
    -- Recommendation: set to false if you don't have `tree-sitter` CLI installed locally
    auto_install = true,

    -- List of parsers to ignore installing (or "all")
    ignore_install = { "javascript" },

    ---- If you need to change the installation directory of the parsers (see -> Advanced Setup)
    -- parser_install_dir = "/some/path/to/store/parsers", -- Remember to run vim.opt.runtimepath:append("/some/path/to/store/parsers")!

    highlight = {
      enable = true,

      -- NOTE: these are the names of the parsers and not the filetype. (for example if you want to
      -- disable highlighting for the `tex` filetype, you need to include `latex` in this list as this is
      -- the name of the parser)
      -- list of language that will be disabled
      disable = { "c", "rust" },
      -- Or use a function for more flexibility, e.g. to disable slow treesitter highlight for large files
      disable = function(lang, buf)
        local max_filesize = 100 * 1024 -- 100 KB
        local ok, stats = pcall(vim.loop.fs_stat, vim.api.nvim_buf_get_name(buf))
        if ok and stats and stats.size > max_filesize then
          return true
        end
      end,

      -- Setting this to true will run `:h syntax` and tree-sitter at the same time.
      -- Set this to `true` if you depend on 'syntax' being enabled (like for indentation).
      -- Using this option may slow down your editor, and you may see some duplicate highlights.
      -- Instead of true it can also be a list of languages
      additional_vim_regex_highlighting = false,
    },
  
  },
}
]]--

}


