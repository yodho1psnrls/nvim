-- https://github.com/hrsh7th/nvim-cmp
-- https://stackoverflow.com/questions/74688630/make-nvim-cmp-not-autoselect-the-1st-option?rq=2
-- https://github.com/petertriho/cmp-git
-- https://www.reddit.com/r/neovim/comments/so4g5e/if_you_guys_arent_using_lsp_signaturenvim_what/

-- You can integrate this plugin with lsp-singature or lsp-singature-helper

-- https://www.reddit.com/r/neovim/comments/vtgrsn/nvimcmp_in_terminal_mode/
-- https://github.com/hrsh7th/nvim-cmp/wiki/List-of-sources

-- TODO: Completion, for :! commands
--  Currently, you can just enable it, but it lag-spikes heavily

return {

  -- dependencies
  -- "L3MON4D3/LuaSnip",
  -- "saadparwaiz1/cmp_luasnip",
  { "hrsh7th/cmp-nvim-lsp", lazy = true, event = 'LspAttach' },
  { "hrsh7th/cmp-nvim-lua", lazy = true, event = 'LspAttach' },
  { "hrsh7th/cmp-buffer",   lazy = true, event = 'LspAttach' },
  { "hrsh7th/cmp-path",     lazy = true, event = 'CmdlineEnter' },
  { "hrsh7th/cmp-cmdline",  lazy = true, event = 'CmdlineEnter' },

  {
    "hrsh7th/nvim-cmp",
    event = "InsertEnter",
    config = function()
      local cmp = require "cmp"

      -- Define the fields arrangement (basic default setup)
      local field_arrangement = { "abbr", "kind", "menu" }

      -- Formatting options
      local formatting_style = {
        fields = field_arrangement,

        format = function(_, item)
          -- Basic LSP icons or default text
          local icons = {
            Text = "",
            Method = "󰆧",
            Function = "󰊕",
            Constructor = "",
            Field = "󰇽",
            Variable = "󰂡",
            Class = "󰠱",
            Interface = "",
            Module = "",
            Property = "󰜢",
            Unit = "",
            Value = "󰎠",
            Enum = "",
            Keyword = "󰌋",
            Snippet = "",
            Color = "󰏘",
            File = "󰈙",
            Reference = "",
            Folder = "󰉋",
            EnumMember = "",
            Constant = "󰏿",
            Struct = "󰙅",
            Event = "",
            Operator = "󰆕",
            TypeParameter = "",
          }

          -- Set icon based on item kind
          item.kind = string.format("%s %s", icons[item.kind] or "", item.kind)

          return item
        end,
      }

      -- Border function for floating windows
      local function border(hl_name)
        return {
          { "╭", hl_name },
          { "─", hl_name },
          { "╮", hl_name },
          { "│", hl_name },
          { "╯", hl_name },
          { "─", hl_name },
          { "╰", hl_name },
          { "│", hl_name },
        }
      end

      -- Setup options for nvim-cmp
      local options = {
        preselect = cmp.PreselectMode.None, -- Disable automatic preselection

        -- Use event handlers to dynamically adjust behavior
        -- Here we disable the suggestions to not show up after only typing space, tab or enter
        --[[event = {
          insert_char_pre = function(char)
            if char == ' ' or char == '\t' or char == '\n' then
              cmp.close() -- Close the popup
            end
          end,
        },]]--

        completion = {
          completeopt = "menu,menuone,noselect,noinsert",
          --keyword_length = 3, -- The number of characters needed to trigger auto-completion
          keyword_length = 1,
        },

        window = {
          completion = {
            winhighlight = "Normal:CmpPmenu,CursorLine:CmpSel,Search:None",
            scrollbar = false,
            --border = border "CmpBorder", -- The completion window should remain focusable to allow item selection
          },
          documentation = {
            border = border "CmpDocBorder",
            winhighlight = "Normal:CmpDoc",
            focusable = false,
          },
          signature = {
            border = border "CmpSignatureBorder",
            winhighlight = "Normal:CmpSignature",
            focusable = false,
          },
        },

        snippet = {
          expand = function(args)
            require("luasnip").lsp_expand(args.body)
          end,
        },

        formatting = formatting_style,

        mapping = {
          ["<C-n>"] = cmp.mapping.select_next_item({ behavior = cmp.SelectBehavior.Select }),
          ["<C-p>"] = cmp.mapping.select_prev_item({ behavior = cmp.SelectBehavior.Select }),

          ["<C-d>"] = cmp.mapping.scroll_docs(-4),
          ["<C-f>"] = cmp.mapping.scroll_docs(4),
          ["<C-Space>"] = cmp.mapping.complete(),
          -- ["<C-e>"] = cmp.mapping.close(),
          ['<C-e>'] = cmp.mapping.abort(),
          -- ['<CR>'] = cmp.mapping.confirm({ select = true }),

          -- This is a custom select, that wont select the first
          --  suggestion on enter, (it is annoyng to press enter
          --  when you want to go to new line and suddenly,
          --  some suggestion just expands)
          ["<CR>"] = cmp.mapping(function(fallback)
            if cmp.visible() and cmp.get_selected_entry() then
              cmp.confirm({ behavior = cmp.ConfirmBehavior.Insert, select = false })
            else
              fallback()
            end
          end, { "i", "s" }),

          ["<Tab>"] = cmp.mapping(function(fallback)
            if cmp.visible() then
              cmp.select_next_item()
            elseif require("luasnip").expand_or_jumpable() then
              require("luasnip").expand_or_jump()
            else
              fallback()
            end
          end, { "i", "s" }),

          ["<S-Tab>"] = cmp.mapping(function(fallback)
            if cmp.visible() then
              cmp.select_prev_item()
            elseif require("luasnip").jumpable(-1) then
              require("luasnip").jump(-1)
            else
              fallback()
            end
          end, { "i", "s" }),
        },

        -- https://github.com/hrsh7th/nvim-cmp/blob/main/doc/cmp.txt#L591
        sources = {
          { name = "nvim_lsp" },
          {
            name = "luasnip",
            option = {
              use_show_condition = false, -- Default: true
              show_autosnippets = true,   -- Default: false
            }
          },
          {
            name = "buffer",
            keyword_length = 3, -- Only suggest buffer words with 3 or more characters
            option = {
              -- Gives suggestions from all buffers
              get_bufnrs = function()
                return vim.api.nvim_list_bufs()
              end
            }
          },

          { name = "nvim_lua" },
          { name = "path" },
          { name = 'cmdline' },

          -- You dont need those, since the lsp source is from nvim_lsp
          -- { name = "nvim_lsp_signature_help" }, -- Correct source name here
          -- {name = "lsp-signature"}
        },
      }

      -- Configure completion window borders
      options.window.completion.border = border "CmpBorder"

      -- Setup nvim-cmp with the options
      cmp.setup(options)

      ----------------------------------------------------

      -- Use buffer source for `/` and `?` (if you enabled
      --  `native_menu`, this won't work anymore).
      cmp.setup.cmdline({ '/', '?' }, {
        mapping = cmp.mapping.preset.cmdline(),
        sources = {
          { name = 'buffer' }
        }
      })


      -- Use cmdline & path source for ':' (if you enabled `native_menu`, this won't work anymore).
      cmp.setup.cmdline(':', {
        mapping = cmp.mapping.preset.cmdline(),
        sources = cmp.config.sources({
          { name = 'path' }
        }, {
          {
            name = 'cmdline',
            option = {
              ignore_cmds = { 'Man', '!' }
              --ignore_cmds = { 'Man' } -- If we dont ignore the ! commands, it lags heavily
            }
          }
        }),
        matching = { disallow_symbol_nonprefix_matching = false }
      })

      ------------------------------------------------

      --[[
    -- Set up lspconfig.
    local capabilities = require('cmp_nvim_lsp').default_capabilities()
    -- Replace <YOUR_LSP_SERVER> with each lsp server you've enabled.
    require('lspconfig')['clangd'].setup {
      capabilities = capabilities
    }
    require('lspconfig')['pylsp'].setup {
      capabilities = capabilities
    }
    --require('lspconfig')['lua-language-server'].setup {
    require('lspconfig')['lua_ls'].setup {
      capabilities = capabilities
    }
    ]] --
    end
  }
}
