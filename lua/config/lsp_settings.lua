-- https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocument_completion
-- see :help lsp-api, lsp-protocol
-- see help lsp-attach
-- see help lsp-quickstart
-- https://gpanders.com/blog/whats-new-in-neovim-0-11/
-- https://lugh.ch/switching-to-neovim-native-lsp.html
-- https://its.mw/posts/lsp-configuration-neovim-11/
-- https://goral.net.pl/post/lsp-configuration-in-neovim-011/
-- https://www.reddit.com/r/neovim/comments/1l4d2dg/how_to_use_the_new_approach_to_lsp_configs_in_011x/
-- https://www.reddit.com/r/neovim/comments/1d9gzud/lsp_and_semantic_tokens_yay_or_nay/

-- NOTE: vim.lsp.with is deprecated
-- vim.lsp.handlers["..."] = ... is also deprecated for client to server responses !
-- Thats why vim.lsp.handlers[textDocument/signatureHelp] = vim.lsp.with(vim.lsp.handlers.signature_help, { list of options }
-- and vim.lsp.handlers["textDocument/hover"] = function(err, result, ctx, config) or { list of options }
-- no longer work, and instead you should give those options in the function that calls them explicitly
-- Thats the reason why after updating to nvim 0.11 there were no borders on some pop-up windows

-- See the doc/deprecated.txt (opens when you enter :help vim.lsp)
-- "vim.lsp.handlers Does not support client-to-server response handlers.
-- Only supports server-to-client requests/notification handlers."

-- See:
-- https://github.com/neovim/neovim/issues/33269#issuecomment-2771417429
-- https://www.reddit.com/r/neovim/comments/1jcjg6v/how_to_override_lsp_handlers_in_011/
-- https://www.reddit.com/r/neovim/comments/1jbegzo/comment/mhtsy98/?utm_source=share&utm_medium=mweb3x&utm_name=mweb3xcss&utm_term=1&utm_content=share_button
-- https://www.reddit.com/r/neovim/comments/1jmsl3j/switch_to_011_now_not_showing_borders_on/


-- e |vim.diagnostic.open_float()

-- Enabling "paste mode" Resets 'autoindent' 'expandtab' 'revins' 'ruler'
--   'showmatch' 'smartindent' 'smarttab' 'softtabstop'
--   'textwidth' 'wrapmargin'.



-- local util = require("utilities")

local M = {}

-- M.signatureHelpProvider = { -- Most LSPs already define it that way
--   triggerCharacters = {"(", ","},  -- override or add triggers
-- }

-- M.flags = { debounce_text_changes = 150 }

-- M.handlers = {
--   -- Same usage as vim.lsp.handlers["textDocument/signatureHelp"] = ...
--   ["textDocument/signatureHelp"] = ...
-- }

M.on_attach = function(_, bufnr)
  local map = vim.keymap.set
  local function opts(desc)
    return { silent = true, buffer = bufnr, desc = "LSP " .. desc }
  end

  -- Keymaps
  map("n", "gD", vim.lsp.buf.declaration, opts "Go to declaration")
  map("n", "gd", vim.lsp.buf.definition, opts "Go to definition")
  map("n", "gi", vim.lsp.buf.implementation, opts "Go to implementation")
  map("n", "<leader>sh", vim.lsp.buf.signature_help, opts "Show signature help")
  map("n", "<leader>wa", vim.lsp.buf.add_workspace_folder, opts "Add workspace folder")
  map("n", "<leader>wr", vim.lsp.buf.remove_workspace_folder, opts "Remove workspace folder")

  map("n", "<leader>wl", function()
    print(vim.inspect(vim.lsp.buf.list_workspace_folders()))
  end, opts "List workspace folders")

  map("n", "<leader>D", vim.lsp.buf.type_definition, opts "Go to type definition")

  map("n", "<leader>ra", vim.lsp.buf.rename, opts "LSP Rename")

  -- THIS IS THE ACTUAL THING THAT GENERATES YOU FUNCTION OR METHOD DEFINITIONS BASED ON DECLARATIONS !!!
  -- THIS ALSO DOES FIXES AVAILABLE CODE ACTIONS
  map({ "n", "v" }, "<leader>ca", vim.lsp.buf.code_action, opts "Code action")
  -- Try to keymap it as it is in Visual Studio (Ctrl - . - Enter(enter when the window shows up))
  --map({ "n", "v" }, "C-.", vim.lsp.buf.code_action, opts "Code action")

  map("n", "gr", vim.lsp.buf.references, opts "Show references")
end

-- NOTE: But better keep it on, because it is just better
-- syntax highlighting that compliments treesitter well
-- M.on_init = function(client, _)
--   if client.supports_method "textDocument/semanticTokens" then
--     -- Disable LSP Semantic Tokens (because they are ON by default if lsp supports them)
--     client.server_capabilities.semanticTokensProvider = nil
--   end
-- end

-- https://www.reddit.com/r/neovim/comments/1d9gzud/lsp_and_semantic_tokens_yay_or_nay/
-- https://github.com/hendrikmi/dotfiles/blob/3c073c05712677b0839c309d756260191670eb81/nvim/lua/core/snippets.lua#L5C1-L5C46
-- vim.highlight.priorities.semantic_tokens = 95 -- set their priority to less than Treesitter's to prevent the LSP from overwriting the syntax highlighting


-- NOTE:
-- dynamicRegistration = false (Always on) Means that the capability
--  should be fully declared in the initialize response.
--  The capability must be declared upfront, and can’t change while running.
-- dynamicRegistration = true Means that the server is allowed to
--  dynamically register this capability later, instead of having
--  to declare it up-front when it connects.
--  Server may enable/disable or modify it after the client connects
--  using client/registerCapability and client/unregisterCapability
-- This exists because some servers
--  Only activate features for certain filetypes.
--  Activate features after parsing project config files.
--  Toggle capabilities depending on project state.

M.capabilities = vim.lsp.protocol.make_client_capabilities()
M.capabilities.textDocument.completion.completionItem = {
  documentationFormat = { "markdown", "plaintext" },
  snippetSupport = true,
  preselectSupport = true,
  insertReplaceSupport = true,
  labelDetailsSupport = true,
  deprecatedSupport = true,
  commitCharactersSupport = true,
  tagSupport = { valueSet = { 1 } },
  resolveSupport = {
    properties = {
      "documentation",
      "detail",
      "additionalTextEdits",
    },
  },
}

-- Because of ufo.nvim, we define some folding options here
M.capabilities.textDocument.foldingRange = {
  dynamicRegistration = false,
  lineFoldingOnly = true
}

M.capabilities.textDocument.semanticTokens.multilineTokenSupport = true

-- Usually this is defaulted, if the lsp supports it
--[[M.capabilities.textDocument.signatureHelp = {
  dynamicRegistration = false,
  signatureInformation = {
    documentationFormat = { "markdown", "plaintext" },
    parameterInformation = { labelOffsetSupport = true },
  },
}]]--

-- https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#markupContent
-- M.capabilities.textDocument.completion.completionItem.documentationFormat = { "markdown", "plaintext" }
-- M.capabilities.textDocument.hover.contentFormat = { "markdown", "plaintext" }
-- M.capabilities.textDocument.signatureHelp.signatureInformation.documentationFormat = { "markdown", "plaintext" }

-- Since we register and trigger it manually, we dont want the lsp to override it
-- M.capabilities.textDocument.signatureHelp.dynamicRegistration = false

vim.lsp.config('*', {
  root_markers = { '.git' },
  -- capabilities = {
  --   textDocument = {
  --     semanticTokens = {
  --       multilineTokenSupport = true,
  --     }
  --   }
  -- },
  on_attach = M.on_attach,
  on_init = M.on_init,
  capabilities = M.capabilities,
  -- handlers = M.handlers,
})


------------------------------------------------------------------

-- NOTE: Semantic Tokens from the LSP (Highlights that override treesitter)
-- https://www.reddit.com/r/neovim/comments/1d9gzud/lsp_and_semantic_tokens_yay_or_nay/
-- https://github.com/hendrikmi/dotfiles/blob/3c073c05712677b0839c309d756260191670eb81/nvim/lua/core/snippets.lua#L5C1-L5C46
-- Prevent LSP from overwriting treesitter color settings
-- https://github.com/NvChad/NvChad/issues/1907
-- vim.hl.priorities.semantic_tokens = 95 -- Or any number lower than 100, treesitter's priority level

vim.lsp.log.set_level("OFF") -- OFF | ERROR | WARN | INFO | DEBUG

local my_virtual_lines_config = {
  -- severity = { min = vim.diagnostic.severity.INFO}, -- Warnings Enabled
  -- highlight_whole_line = false, -- false
  current_line = true,
  format = function (diagnostic)
    -- return string.format("[%s] %s", vim.diagnostic.severity[diagnostic.severity], diagnostic.message)
    -- return "Slqlqlq " .. diagnostic.message -- To test
    return diagnostic.message
  end,
}

-- NOTE: There is now native virtual_lines, without the need for nvim virtual lines plugin
-- https://www.reddit.com/r/neovim/comments/1if024i/theres_now_a_builtin_virtual_lines_diagnostic/
vim.diagnostic.config({

  virtual_text = false,
  -- virtual_lines = true,
  -- See help vim.diagnostic.Opts.VirtualLines
  -- virtual_lines = my_virtual_lines_config,
  virtual_lines = false,

  --[[virtual_text = {
   -- severity = { min = vim.diagnostic.severity.INFO}, -- Warnings Enabled
    current_line = false,
    prefix = '●',
    -- Add a custom format function to show error codes
    -- format = function(diagnostic)
    --   local code = diagnostic.code and string.format('[%s]', diagnostic.code) or ''
    --   return string.format('%s %s', code, diagnostic.message)
    -- end,
  },]]--

  underline = true,
  update_in_insert = true,
  severity_sort = true,
  -- signs = true, -- true
  signs = {
    text = {
      [vim.diagnostic.severity.ERROR] = "",
      [vim.diagnostic.severity.WARN] = "",
      [vim.diagnostic.severity.INFO] = "",
      [vim.diagnostic.severity.HINT] = "",
    },
    numhl = { -- The numberline highlighting next to the sign icons
      -- [vim.diagnostic.severity.ERROR] = "ErrorMsg",
      -- [vim.diagnostic.severity.WARN] = "WarningMsg",
    },
  },

  -- float = {
  --  border = 'rounded',
  --  -- source = true,
  -- }

  -- severity = {
  -- min = vim.diagnostic.severity.WARN, -- HINT | INFO | WARN | ERROR
  -- min = nil,
  -- },

  -- format = function (_) return vim.diagnostic.message end,

  --[[format = function(diagnostic)
    if type(diagnostic) ~= "table" or type(diagnostic.message) ~= "string" then
      return "" 
    end
    return diagnostic.message:gsub("^%w+: ", "") -- Remove diagnostic.code
  end,]]--
})

-- local is_virtual_lines_enabled = true
-- local function toggle_virtual_lines()
--   if is_virtual_lines_enabled then
--     vim.diagnostic.config({ virtual_lines = false })
--   else
--     vim.diagnostic.config({ virtual_lines = my_virtual_lines_config })
--   end
--   is_virtual_lines_enabled = not is_virtual_lines_enabled
-- end

-- NOTE: update_in_insert = false, but only for virtual lines
vim.api.nvim_create_autocmd('InsertEnter', {
  callback = function ()
    vim.diagnostic.config({ virtual_lines = false })
  end
})
local vl_activate_id = nil
local function toggle_virtual_lines()
  if vl_activate_id == nil then
    vim.diagnostic.config({ virtual_lines = my_virtual_lines_config })
    -- NOTE: InsertLeave and InsertLeavePre dont invoke on Ctrl + c
    vl_activate_id = vim.api.nvim_create_autocmd('InsertLeave', {
      callback = function ()
        vim.diagnostic.config({ virtual_lines = my_virtual_lines_config })
      end
    })
  else
    vim.diagnostic.config({ virtual_lines = false })
    vim.api.nvim_del_autocmd(vl_activate_id)
    vl_activate_id = nil
  end
end
toggle_virtual_lines()
vim.keymap.set("n", "<leader>ll", toggle_virtual_lines, { desc = "Toggle virtual lines" })


vim.keymap.set("n", "<leader>dt", function()
  vim.diagnostic.enable(not vim.diagnostic.is_enabled())
end, {desc="Toggle all shown diagnostics", silent=true, noremap=true})

-- NOTE: Deprecated:

-- vim.fn.sign_define("DiagnosticSignWarn", { text = "⚠️", texthl = "DiagnosticWarn" })  -- Warning icon
-- vim.fn.sign_define("DiagnosticSignError", { text = "❌", texthl = "DiagnosticError" })  -- Error icon
-- vim.fn.sign_define("DiagnosticSignHint", { text = "💡", texthl = "DiagnosticHint" })   -- Hint icon
-- vim.fn.sign_define("DiagnosticSignInfo", { text = "ℹ️", texthl = "DiagnosticInfo" })  -- Info icon

-- vim.fn.sign_define("DiagnosticSignWarn", { text = "W", texthl = "DiagnosticWarn" })  -- Warning icon
-- vim.fn.sign_define("DiagnosticSignError", { text = "E", texthl = "DiagnosticError" })  -- Error icon
-- vim.fn.sign_define("DiagnosticSignHint", { text = "H", texthl = "DiagnosticHint" })   -- Hint icon
-- vim.fn.sign_define("DiagnosticSignInfo", { text = "I", texthl = "DiagnosticInfo" })  -- Info icon


local function ToggleWarnings()
  local diag_conf = vim.diagnostic.config() -- Get the current config
  if diag_conf == nil then return end

  -- if diag_conf.virtual_text.severity.min == nil then
  if diag_conf.virtual_text.severity.min == vim.diagnostic.severity.WARN then
    -- diag_conf.virtual_text.severity.min = vim.diagnostic.severity.ERROR
    diag_conf.virtual_text.severity.min = vim.diagnostic.severity.ERROR
    -- vim.fn.sign_define("DiagnosticSignWarn", { text = "", texthl = "DiagnosticWarn" })  -- Warning icon
    print("LSP Warnings Disabled")
    -- elseif diag_conf.virtual_text.severity.min == vim.diagnostic.severity.ERROR then
  elseif diag_conf.virtual_text.severity.min == vim.diagnostic.severity.ERROR then
    diag_conf.virtual_text.severity.min = vim.diagnostic.severity.WARN
    -- vim.fn.sign_define("DiagnosticSignWarn", { text = "W", texthl = "DiagnosticWarn" })  -- Warning icon
    print("LSP Warnings Enabled")
  end

  vim.diagnostic.config(diag_conf) -- Apply the modified config !!! (because the config value is immutable)
end

vim.keymap.set('n', '<leader>wt', function() ToggleWarnings() end,
  { noremap = true, silent = true, desc = 'Warnings Toggle' })


local function ToggleSigns()
  -- LSP Diagnostic Signs Only
  --[[local diag_conf = vim.diagnostic.config()
        if not diag_conf then return end
        diag_conf.signs = not diag_conf.signs
        vim.diagnostic.config(diag_conf)
        print("Signs Toggled")]]--
  if vim.o.signcolumn == "auto" then
    vim.o.signcolumn = "no"
    print ("Sign column hidden")
  elseif vim.o.signcolumn == "no" then
    vim.o.signcolumn = "auto"
    print ("Sign column visible")
  end
end

vim.keymap.set('n', '<leader>sg', function() ToggleSigns() end,
  { noremap = true, silent = true, desc = 'Signs Toggle' })


-- BUILT IN NEOVIM DIAGNOSTICS WINDOW ------------------------------------------------------------
-- https://www.reddit.com/r/neovim/comments/11axh2p/how_to_toggle_openclose_floating_lsp_diagnostic/
-- https://neovim.io/doc/user/diagnostic.html
-- https://neovim.io/doc/user/quickfix.html#quickfix

-- vim.diagnostic.setqflist({
--   diagnostics = vim.diagnostic.get(0),
--   open = true,
--   title = "Current Buffer Diagnostics"
-- })

vim.keymap.set('n', '<leader>ds', vim.diagnostic.setloclist, { desc = 'Open LSP diagno[S]tic' })
--vim.keymap.set('n', '<leader>dq', vim.diagnostic.setqflist, { desc = 'Open diagnostic [Q]uickfix list' })
vim.keymap.set('n', '<leader>da', vim.diagnostic.setqflist, { desc = 'Open [A]ll LSP diagnostic' })

--vim.keymap.set('n', '<S-CR>', vim.diagnostic.open_float, { desc = 'Go to diagnostic'})
-- NOTE: You dont need that keymap, since just pressing enter does the same
-- vim.keymap.set('n', '<leader>dg', vim.lsp.util.jump_to_location, { desc = '[G]o to diagnostic'})

-- https://www.reddit.com/r/neovim/comments/18qh3mg/how_to_show_entire_diagnostic_message/
--vim.keymap.set('n', '<leader>df', vim.diagnostic.open_float,
--  { desc = 'Open floating diagnostic window, while the cursor is on a line with diagnostic message'})

-- :lua vim.diagnostic.open_float() is the command that executes when you normally press Enter
--  in the diagnostics window, but since we remapped Enter in normal mode, we need to make it
--  apply the logic for it to execute that
--[[vim.api.nvim_create_autocmd("FileType", {
        pattern = "vim.diagnostic.float",
        callback = function()
          vim.keymap.set("n", "<CR>", function()
            local __opts = { float = true }
            vim.diagnostic.goto_next(__opts) -- Automatically jumps to the diagnostic window really-simeilfe
          end)
        end,
      })]]--


-- NOTE: Use ]q, [q, ]l, [l
--[[
-- Function to jump to the next diagnostic
vim.keymap.set('n', '<leader>dn', vim.diagnostic.goto_next,
  { desc = 'Go to [N]ext diagnostic', noremap = true, silent = true})

-- Function to jump to the previous diagnostic
vim.keymap.set('n', '<leader>dp', vim.diagnostic.goto_prev,
  { desc = 'Go to [P]revious diagnostic', noremap = true, silent = true })
]]--

---------------------------------------------------------------------------------

--[[local set_rounded_border = function(handler_name)
  vim.lsp.handlers[handler_name] = vim.lsp.with(
    vim.lsp.handlers[handler_name],
    { border = "rounded" }
  )
end
set_rounded_border("textDocument/hover")
set_rounded_border("textDocument/signatureHelp")
set_rounded_border("textDocument/codeAction")]]--

----------------------------- COMPLETION ------------------------------------
-- https://vi.stackexchange.com/questions/46700/is-the-new-built-in-neovim-autocompletion-replacing-completion-plugins-like-nvim

-- If you have completion errors using typescript-language-server and rust-analyzer
-- See https://github.com/neovim/neovim/issues/33142

-- NOTE: See :help complete-items
-- Map LSP kinds to icons
--[[local cmp_icons = {
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
}]]--

local cmp_icons = {
  "",
  "󰆧",
  "󰊕",
  "",
  "󰇽",
  "󰂡",
  "󰠱",
  "",
  "",
  "󰜢",
  "",
  "󰎠",
  "",
  "󰌋",
  "",
  "󰏘",
  "󰈙",
  "",
  "󰉋",
  "",
  "󰏿",
  "󰙅",
  "",
  "󰆕",
  "",
}

local lsp_protocol = require("vim.lsp.protocol")
local kind_names = lsp_protocol.CompletionItemKind
-- print(vim.inspect(require("vim.lsp.protocol").CompletionItemKind))

-- https://www.reddit.com/r/neovim/comments/1mglgn4/simple_native_autocompletion_with_autocomplete/
-- See :help lsp-attach
vim.api.nvim_create_autocmd('LspAttach', {
  -- group = vim.api.nvim_create_augroup('my.lsp', {}),
  callback = function(ev)
    local client = vim.lsp.get_client_by_id(ev.data.client_id)
    if client == nil then return end
    -- local kinds = vim.lsp.protocol.CompletionItemKind

    if client:supports_method('textDocument/completion') then
      -- Autotrigger setup (Trigger on every char). See :help lsp-autocompletion
      -- NOTE: Extending the triggerCharacters field, should happen on LspAttach
      -- before calling vim.lsp.completion.enable(... {autotrigger=true})
      -- BUG: autotrigger=true in vim.lsp.completion.enable respects the
      -- triggerCharacters, vim.o.autocomplete=true, does NOT !!
      local chars = {} -- ASCII printable chars
      for c = 33, 126 do chars[#chars + 1] = string.char(c) end -- 32 is space
      client.server_capabilities.completionProvider.triggerCharacters = chars

      vim.lsp.completion.enable(true, client.id, ev.buf, {
        autotrigger = true,
        -- See :help complete-items for all fields of item parameter
        convert = function(item)
          local trim_word = function(word, cap) -- Used for abbr
            -- word:gsub("^%s+", "") -- trim leading spaces (Clangd gives such spaces in front)
            word = word:gsub("%b()", ""):gsub("%b{}", "") -- remove brackets
            word = word:match("[%w_.]+.*") or word -- remove leading misc chars
            word = word:gsub("%?$", "") -- Remove trailing ? (lua properties have these)
            if cap then word = #word > cap and word:sub(1, cap-1) .. "…" or word end -- cap length
            return word

          end
          return {
            -- abbr = item.label -- What appears in the popup
            abbr = trim_word(item.label, 20),
            word = trim_word(item.label), -- What gets inserted 
              -- .. ((item.kind == kinds.Function or item.kind == kinds.Method) and "(" or ""),

            dup = 0, -- duplication behavior (1 or 0)
            -- detail = item.detail or "", -- rigth-side annotation (signature or type)
            -- menu = "[" .. client.name .. "]", -- Optional (usually shows sources)

            kind = cmp_icons[item.kind],
            -- kind = string.format("%s %s", cmp_icons[item.kind] or "", kind_names[item.kind]), -- the type icon
            -- kind = (cmp_icons[item.kind] or "") .. " " .. kind_names[item.kind], -- the type icon
            menu = "", -- (Remove it) right-side annotation (Usually the return type of the function)
            -- menu = item.detail or "" -- default menu column (Shows return type)
            -- index -- ordering hint

            -- filterText - What text filtering uses
            -- sortText	- Custom sorting
            -- insertText	- Override insertion behavior
            -- textEdit	- Replace custom range
          }
        end,
        -- float= { -- Not a field in completion.enable opts
        --   anchor="NW",
        --   relative="cursor",
        --   row=1,
        --   col=0,
        -- },
      })
    end

  end
})

-- NOTE: Trigger autocompletion after certain number of characters are inserted
--[[vim.api.nvim_create_autocmd("InsertCharPre", {
  callback = function()
    local min_chars = 3 - 1
    local col = vim.fn.col(".") - 1  -- current column (0-based)
    if col < min_chars then return end        -- require at least 3 characters

    local line = vim.api.nvim_get_current_line()
    local prefix = line:sub(1, col) -- text before cursor

    -- local prefix = "foo.bar."
    -- print(prefix:match("%W+$")) -- prints "."

    local last_char = prefix:sub(-1)
    local word = prefix:match("%w+$") or ""
    if #word >= min_chars or last_char == "." then
      vim.lsp.completion.get()
    end
  end,
})]]--

-- Try snippets expansion, by accepting a selection with :h complete_CTRL-Y
-- https://github.com/neovim/neovim/issues/29695
-- See :h complete_CTRL-Y
-- Use enter to accept a selection (expands the snippets properly)

-- NOTE: Completion Keybindings
-- Defaults are: <C-n> and <C-p> for navigation and <C-y> for acception
vim.keymap.set("i", "<Tab>", function()
  -- local is_begining = strpart( getline('.'), 0, col('.')-1 ) =~ '^\s*$' -- vimscript
  return vim.fn.pumvisible() == 1 and "<C-n>" or "<Tab>"
end, { expr = true, noremap=true, silent=true, desc="Scroll completions"})
vim.keymap.set("i", "<S-Tab>", function()
  return vim.fn.pumvisible() == 1 and "<C-p>" or "<S-Tab>"
end, { expr = true, noremap=true, silent=true, desc="Scroll completions backwards"})
vim.keymap.set("i", "<CR>", function()
  local has_selected = vim.fn.complete_info().selected ~= -1
  return (vim.fn.pumvisible() == 1 and has_selected) and "<C-y>" or "<CR>"
end, { expr = true, noremap=true, silent=true, desc="Accept selected completion"})
-- NOTE: Use <C-e> to end completion (return back to what you typed originally)
-- See more in :help popupmenu-keys


-- See :help options and then /complete

-- Command mode autocompletion options
-- https://www.reddit.com/r/vim/comments/qltep/what_is_your_wildmode_setting_and_why/
-- vim.o.wildmode = "list:longest,list:full"
-- :help nvim_open_win and see the zindex config parameter example
-- vim.o.wildoptions = vim.o.wildoptions .. ",pum"

-- See :help completeopt
-- vim.o.completeopt = "menu,menuone,noselect,fuzzy,nearest" -- THIS
vim.o.completeopt = "menuone,noselect,fuzzy,popup" -- Shows selection item info as additional popup
-- vim.o.showmatch = true -- Default is true (Show the selected word)

vim.o.complete = ".,o" -- use buffer and omnifunc
-- Omnifunc is part of neovim's native omnicompletion
-- It is a function which returns completion candidates
-- You can manually trigger it with <C-x><C-o>
vim.bo.omnifunc = "v:lua.vim.lsp.omnifunc"
-- vim.o.pumblend
-- vim.o.dictionary
-- vim.o.tags
-- vim.o.path

-- vim.o.autocomplete = true -- Set autotrigger=true in vim.lsp.completion.enable instead !
-- vim.o.autocompletedelay = 250 -- 60wpm ~ 200ms delay between chars
-- vim.o.autocompletetimeout -- completion sources loading (Dont touch)
-- vim.o.completefunc

-- NOTE:
-- https://vi.stackexchange.com/questions/36486/nvim-popup-vs-vim-popup
-- https://github.com/neovim/neovim/issues/26681#issuecomment-1864781638
-- https://github.com/neovim/neovim/pull/25541
-- This will enable you to customize the selected item info popup
-- thats enabled with vim.o.completeopt+=popup, not yet implemented
-- See :help completepopup and you will see it in Missing Features
-- Until implemented in core, use a CompleteChange autocmd with
-- floating window which shows the item.info
--vim.o.completepopup = { border = "round", }

-- https://vi.stackexchange.com/questions/46203/how-do-you-get-the-position-of-the-cursor-or-of-a-line-within-the-window
-- https://github.com/neovim/neovim/issues/29225
-- https://github.com/neovim/neovim/issues/30578
-- https://github.com/neovim/neovim/issues/29225#issuecomment-2469976099
-- https://github.com/neovim/neovim/issues/10996

-- The preview_winid gets changed when you click on the popup window (even though, its not focusable)
-- local _pc_id = vim.api.nvim_create_augroup("PopupConfigGroup", {})
local last_winid = nil -- The preview_winid changes when you click on it
vim.api.nvim_create_autocmd("CompleteChanged", { -- :help CompleteChanged
  desc = "Configure completion item info popup",
  -- group = _pc_id,
  callback = function()
    local winid = vim.fn.complete_info().preview_winid
    if winid and (winid ~= last_winid) then
      vim.api.nvim_win_set_config(
        winid,
        {
          border = "rounded",
          focusable = false,
          -- style = "minimal", -- currently only minimal is supported
          -- width = math.min(60, vim.o.columns - 2),
          -- height = 8, -- Doesnt prevent the window to be bigger for some reason
          -- anchor = "SW",
          -- title = "INFO",
          -- title_pos = "center",
        }
      )
      last_winid = winid
      -- vim.api.nvim_clear_autocmds({ group = _pc_id })
      -- _pc_id = -1
    end
  end
})

-- NOTE: Show LSP hover doc when completion selection changes
-- Its not the same as adding 'popup' in vim.o.completeopt, because
-- it also shows the signature not only the documentation/comments for the item
-- NOTE: Ways to get the window's width from its id
-- vim.api.nvim_win_get_width(winid)
-- vim.api.nvim_win_get_config(info.preview_winid).width
--[[vim.api.nvim_create_autocmd("CompleteChanged", { -- :help CompleteChanged
  desc = "Show popup completion item resolve",
  callback = function(ev)
    local info = vim.fn.complete_info()
    local pum_win = vim.fn.pum_getpos()
    local pum_width = pum_win.width and pum_win.width or 0
    local offset = pum_win.col and vim.api.nvim_win_get_cursor(0)[2] - pum_win.col + 4 or 0
    offset = pum_width-offset
    -- offset = math.max(0, offset)
    -- print("lines: " .. vim.o.lines)
    -- print("cur_row: " .. vim.fn.getpos('.')[2])
    if info.selected ~= -1 then
      vim.lsp.buf.hover({
        border = 'rounded',
        focusable = false,
        focus = false,
        silent = true,

        offset_y=0,
        offset_x = offset,
        relative="cursor",

        max_width = math.max(1, vim.o.columns - (vim.fn.getpos('.')[3] + offset + 8)),
        -- max_height = math.max(1, vim.o.lines - (vim.fn.getpos('.')[2] + 8)),
        close_events = {"CompleteChanged", "CompleteDonePre"}
      })
    end
  end
})]]--

-- https://stackoverflow.com/questions/19580157/to-hide-user-defined-completion-message-at-vim
vim.o.shortmess = vim.o.shortmess .. "c" -- Hide "User defined completion" messages at every completion selection

-- Those pum(PopUpMenu) options mainly affect the completion menu
vim.o.pumheight = 8 -- PopUpMenu Max Height
vim.o.pumborder = 'rounded' -- Requires nvim 0.12
vim.o.pumblend = 15 -- 0 fully opaque to 100 fully transparent

-- vim.opt.pumanchor = "cursor" -- Anchor PUM to cursor position (Default)
-- vim.opt.pumnoreverse = true -- Never flip above unless forced (Deprecated)

vim.api.nvim_set_hl(0, "PmenuSel", { blend = 0 }) -- different opacity for selected item

-- • |hl-PmenuBorder| |hl-PmenuShadow| |hl-PmenuShadowThrough| see 'pumborder'.
vim.api.nvim_set_hl(0, 'PmenuBorder', { fg = 'White' })

----------------------------- SIGNATURE_HELP ------------------------------------
-- https://www.reddit.com/r/neovim/comments/ocup0d/how_can_i_make_nvimlsp_signaturehelp_to_stick/
-- https://neovim.discourse.group/t/show-signature-help-on-insert-mode/2007/5
-- NOTE: :help lsp-handler

-- https://github.com/neovim/neovim/issues/14846
local function show_signature_help()
  vim.lsp.buf.signature_help({
    border = 'rounded',
    focusable = false, -- Can you focus it with a mouse
    focus = false,    -- Should it focus the window | Can you focus it with jump through windows keys
    silent = true,
    max_height = 3,
    anchor_bias = 'above',
    close_events = {'InsertLeave'}
  })
end

-- Lua 26: vim/_defaults.lua:234
vim.api.nvim_create_autocmd({"CursorHoldI"}, { callback=show_signature_help })
-- vim.keymap.set('i', '<C-s>', show_signature_help, {noremap=true, silent=true});

-- If you miss the original <S-j> behavior that cojoins lines
--  , just use it from visual mode (or even better, vipJ to join whole paragraph).
--  https://www.reddit.com/r/vim/comments/14iito9/automatically_shiftj/
-- The default mapping for signature_help is
-- <C-s> or <Plug>(nvim.lsp.ctrl-s) in insert mode.
-- vim.keymap.set('n', '<S-j>', show_signature_help
-- , {desc="Show Signature Help", noremap=true, silent=true})

-- https://www.reddit.com/r/neovim/comments/vbsryc/show_lsp_signature_help_window_above_cursor/
-- https://github.com/askfiy/nvim/commit/395fb842ea3441fd69d6cb1e96c6f78d9bc19edb
--[[local function lsp_signature_help(_, result, ctx, config)
  -- Add file type for LSP signature help
  local bufnr, winner = vim.lsp.handlers.signature_help(_, result, ctx, config)

  -- Put the signature floating window above the cursor
  vim.api.nvim_win_set_config(winner, {
    anchor = "SW",
    relative = "cursor",
    row = 0,
    col = 0
  })

  if bufnr and winner then
    vim.api.nvim_buf_set_option(bufnr, "filetype", config.filetype)
    return bufnr, winner
  end
end

-- 🔥 Register your custom handler globally
vim.lsp.handlers["textDocument/signatureHelp"] = lsp_signature_help]]--

-- https://www.reddit.com/r/neovim/comments/vbsryc/show_lsp_signature_help_window_above_cursor/
--[[vim.lsp.handlers["textDocument/signatureHelp"] =
vim.lsp.with(vim.lsp.handlers.signature_help, {
  -- border = "single",
  border = "rounded",
  silent = true,
  focusable = false, -- Enable focusing the window
  -- focus = false, -- Does it focus on show
  -- max_width = 80,
  -- max_height = 15,
  -- reuse_win = true, -- Doesnt exist such option
})]]--

-- NOTE: Debugging
--[[vim.api.nvim_create_autocmd("LspAttach", {
  callback = function(ev)
    local client = vim.lsp.get_client_by_id(ev.data.client_id)
    if client and client.server_capabilities.signatureHelpProvider then
      -- vim.lsp.inlay_hint.enable(true, { bufnr = ev.buf })
      print("Has Signature Help Provider")
      print(vim.inspect(debug.getinfo(vim.lsp.handlers["textDocument/signatureHelp"])))
    end
  end,
})]]--

-- NOTE: Now it gets triggered by treesitter, see its config file !

-- BUG: LSP Signature Help Autotrigger Logic based on InsertChar AutoCmd
-- , but it uses vim.keymap.set which interferes with other keybindings
--[[vim.api.nvim_create_autocmd('LspAttach', {
  callback = function(ev)
    -- local client = vim.lsp.get_client_by_id(ev.data.client_id)
    -- if client == nil then return end
    local bufnr = ev.buf
    -- NOTE: This replaces the nvim-lspsignature plugin completely
    -- Signature help keymap
    -- vim.keymap.set("n", "<C-k>", vim.lsp.buf.signature_help, opts "Signature help")
    -- Signature help autotrigger

    -- vim.keymap.set("i", "(", function() -- BUG: autopairs dont trigger ()
    --   vim.lsp.buf.signature_help()
    --   return "(" -- needs expr=true to evaluate the return of the function
    -- end, { buffer=bufnr, expr=true, silent=true, remap=true})
    -- vim.keymap.set("i", ",", function()
    --   vim.lsp.buf.signature_help()
    --   return "," -- needs expr=true to evaluate the return of the function
    -- end, { buffer=bufnr, expr=true, silent=true, remap=true})

    -- Hook signature help without breaking existing keymaps
    util.add_key_trigger('i', '(', show_signature)
    util.add_key_trigger('i', ',', show_signature)

  end
})]]--

----------------------------- INLAY-HINTS ------------------------------------

--[[
-- vim.lsp.inlay_hint.enable(true) -- Overriten by the LspAttach enabling

-- vim.lsp.handlers["textDocument/inlayHint"] = function(err, result, ctx, config)
--   print("Got inlay hints:", vim.inspect(result))
--   -- You could completely replace how hints are placed in the buffer here.
-- end

vim.api.nvim_create_autocmd("LspAttach", {
  callback = function(ev)
    local client = vim.lsp.get_client_by_id(ev.data.client_id)
    if client and client.server_capabilities.inlayHintProvider then
      vim.lsp.inlay_hint.enable(true, { bufnr = ev.buf })
    end
  end,
})
-- Available highlight groups:
-- LspInlayHint (main)
-- LspInlayHintParameter
-- LspInlayHintType
vim.api.nvim_set_hl(0, "LspInlayHint", {
  fg = "#6e7380",
  bg = "#1e1f25",
  italic = true,
})
-- vim.api.nvim_set_hl(0, "LspInlayHint", { fg = "#6e7380", italic = true })
-- vim.api.nvim_set_hl(0, "LspInlayHintParameter", { fg = "#c586c0", italic = true })
-- vim.api.nvim_set_hl(0, "LspInlayHintType", { fg = "#9cdcfe", italic = true })


-- See :help vim.lsp.inlay_hint and :help vim.lsp.inline_completion(copilot lsp multiline completion support)

-- vim.lsp.inlay_hint.set({ -- Deprecated, doesnt work
--   prefix = " » ", -- prefix used before the hint text
--   only_current_line = false, -- highlight for the prefix
-- })

vim.keymap.set("n", "<leader>ih", function()
  vim.lsp.inlay_hint.enable(not vim.lsp.inlay_hint.is_enabled())
end, { desc = "Toggle LSP Inlay Hints" })
-- vim.keymap.set("n", "<leader>ih", function()
--   local bufnr = vim.api.nvim_get_current_buf()
--   local enabled = vim.lsp.inlay_hint.is_enabled({ bufnr = bufnr })
--   vim.lsp.inlay_hint.enable(not enabled, { bufnr = bufnr })
-- end, { desc = "Toggle LSP Inlay Hints in Current Buffer" })
]]--

----------------------------- CODE-LENS ------------------------------------

--[[
-- Refresh codelenses (usually after opening a buffer)
vim.api.nvim_create_autocmd("BufEnter", {
  callback = function()
    vim.lsp.codelens.refresh()
  end
})

-- Map a key to run the code lens under the cursor
vim.keymap.set("n", "<leader>cl", vim.lsp.codelens.run, { noremap = true, silent = true })
]]--

--[[
-- Options for virtual text display
local ns = vim.api.nvim_create_namespace("codelens_vt")

local function render_codelenses(bufnr)
  bufnr = bufnr or vim.api.nvim_get_current_buf()
  vim.api.nvim_buf_clear_namespace(bufnr, ns, 0, -1)

  local lenses = vim.lsp.codelens.get(bufnr)
  if not lenses then return end

  for _, lens in ipairs(lenses) do
    local line = lens.range.start.line
    local text = lens.command and lens.command.title or ""
    if text ~= "" then
      vim.api.nvim_buf_set_extmark(bufnr, ns, line, 0, {
        virt_text = { { text, "Comment" } },
        virt_text_pos = "eol",
        hl_mode = "combine",
      })
    end
  end
end

-- Auto-refresh codelenses when entering buffer or saving
vim.api.nvim_create_autocmd({ "BufEnter", "BufWritePost", "CursorHold" }, {
  callback = function()
    vim.lsp.codelens.refresh()
    render_codelenses()
  end
})

-- Keymap to run the code lens under cursor
vim.keymap.set("n", "<leader>cl", function()
  vim.lsp.codelens.run()
  render_codelenses() -- refresh after running
end, { desc="Run Code Lens under cursor", noremap = true, silent = true })
]]--


