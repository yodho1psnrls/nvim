-- see :help lsp-api, lsp-protocol
-- see help lsp-attach
-- see help lsp-quickstart
-- https://gpanders.com/blog/whats-new-in-neovim-0-11/
-- https://lugh.ch/switching-to-neovim-native-lsp.html
-- https://its.mw/posts/lsp-configuration-neovim-11/
-- https://goral.net.pl/post/lsp-configuration-in-neovim-011/
-- https://www.reddit.com/r/neovim/comments/1l4d2dg/how_to_use_the_new_approach_to_lsp_configs_in_011x/
-- https://www.reddit.com/r/neovim/comments/1d9gzud/lsp_and_semantic_tokens_yay_or_nay/


local M = {}

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

-- M.on_init = function(client, _)
--   if client.supports_method "textDocument/semanticTokens" then
--     client.server_capabilities.semanticTokensProvider = nil
--   end
-- end

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
})


------------------------------------------------------------------

-- NOTE: Semantic Tokens from the LSP (Highlights that override treesitter)
-- https://www.reddit.com/r/neovim/comments/1d9gzud/lsp_and_semantic_tokens_yay_or_nay/
-- https://github.com/hendrikmi/dotfiles/blob/3c073c05712677b0839c309d756260191670eb81/nvim/lua/core/snippets.lua#L5C1-L5C46
-- Prevent LSP from overwriting treesitter color settings
-- https://github.com/NvChad/NvChad/issues/1907
-- vim.hl.priorities.semantic_tokens = 95 -- Or any number lower than 100, treesitter's priority level

vim.lsp.log.set_level("ERROR") -- OFF | ERROR | WARN | INFO | DEBUG

local my_virtual_lines_config = {
  -- severity = { min = vim.diagnostic.severity.INFO}, -- Warnings Enabled
  -- highlight_whole_line = false, -- false
  current_line = false,
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
  virtual_lines = my_virtual_lines_config,

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

local is_virtual_lines_enabled = true
local function toggle_virtual_lines()
  if is_virtual_lines_enabled then
    vim.diagnostic.config({ virtual_lines = false })
  else
    vim.diagnostic.config({ virtual_lines = my_virtual_lines_config })
  end
  is_virtual_lines_enabled = not is_virtual_lines_enabled
end
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

-- Completion and SignatureHelp
vim.api.nvim_create_autocmd('LspAttach', {
  callback = function(ev)
    local client = vim.lsp.get_client_by_id(ev.data.client_id)
    if client == nil then return end

    -- Autocompletion from the lsp
    if client:supports_method('textDocument/completion') then
      vim.lsp.completion.enable(true, client.id, ev.buf, {
        autotrigger = true,
        convert = function(item)
          return {
            -- abbr = item.label -- What appears in the popup
            --   :gsub('%b()', ''), -- functionName(int x, int y)" → "functionName"
            word = item.label -- What gets inserted
              :gsub("^%s+", ""), -- trim leading spaces (Clangd gives such spaces in front)

            dup = 0, -- duplication behavior (1 or 0)
            -- detail = item.detail or "", -- rigth-side annotation (signature or type)
            -- menu = "[" .. client.name .. "]", -- Optional (usually shows sources)

            -- kind, -- the type icon
            -- menu, -- right-side annotation
            -- index -- ordering hint

            -- filterText - What text filtering uses
            -- sortText	- Custom sorting
            -- insertText	- Override insertion behavior
            -- textEdit	- Replace custom range
          }
        end
      })
    end

    -- Autotrigger setup (Trigger on every char)
    -- if client.server_capabilities.completionProvider then
    --   local chars = {}
    --   for c = 32, 126 do -- ASCII printable chars
    --     chars[#chars + 1] = string.char(c)
    --   end
    --   client.server_capabilities.completionProvider.triggerCharacters = chars
    -- end
  end
})

-- NOTE: Forse trigger autocompletion on every character insert
vim.api.nvim_create_autocmd("InsertCharPre", {
  callback = function()
    vim.lsp.completion.get()
	-- BUG: TRIGGERS A FREEZE !!!
    -- vim.fn.feedkeys("<C-x><C-o>") -- Also triggers omnifunc
  end,
})


-- Trim leading spaces (Clang adds on space in front of completion items)
-- Hacky way of doing it, It is now replaced by the convert entry in vim.lsp.completion.enable
--[[vim.lsp.handlers["textDocument/completion"] = function(err, result, ctx, config)
  if result and result.items then
    for _, item in ipairs(result.items) do
      item.label = item.label:gsub("^%s+", "")  -- remove leading spaces
    end
  end

  return vim.lsp.handlers["textDocument/completion"](err, result, ctx, config)
end]]--


-- NOTE:
-- https://www.reddit.com/r/vim/comments/qltep/what_is_your_wildmode_setting_and_why/
-- Configuration for the command line autocompletion setting
--[[
vim.o.wildmode = "list:longest,list:full"

-- https://www.reddit.com/r/neovim/comments/1mglgn4/simple_native_autocompletion_with_autocomplete/

vim.o.complete = ".,o" -- use buffer and omnifunc
vim.o.completeopt = "fuzzy,menuone,noselect" -- add 'popup' for docs (sometimes)
-- vim.o.autocomplete = true
vim.o.pumheight = 7 (PopUpMenu Height


vim.api.nvim_create_autocmd("LspAttach", {
  callback = function(ev)
    vim.lsp.completion.enable(true, ev.data.client_id, ev.buf, {
      -- Optional formating of items
      convert = function(item)
        -- Remove leading misc chars for abbr name,
        -- and cap field to 25 chars
        --local abbr = item.label
        --abbr = abbr:match("[%w_.]+.*") or abbr
        --abbr = #abbr > 25 and abbr:sub(1, 24) .. "…" or abbr
        --
        -- Remove return value
        --local menu = ""

        -- Only show abbr name, remove leading misc chars (bullets etc.),
        -- and cap field to 15 chars
        local abbr = item.label
        abbr = abbr:gsub("%b()", ""):gsub("%b{}", "")
        abbr = abbr:match("[%w_.]+.*") or abbr
        abbr = #abbr > 15 and abbr:sub(1, 14) .. "…" or abbr

        -- Cap return value field to 15 chars
        local menu = item.detail or ""
        menu = #menu > 15 and menu:sub(1, 14) .. "…" or menu

        return { abbr = abbr, menu = menu }
      end,
    })
  end,
})
]]--


-- NOTE: Completion Keybindings
-- Defaults are: <C-n> and <C-p> for navigation
-- And <CR> or <Tab> for selection

-- vim.keymap.set('i', '<Tab>', '<C-n>',
--   {desc="Scroll completion suggestions", silent=true, noremap=true})
-- vim.keymap.set('i', '<S-Tab>', '<C-p>',
--   {desc="Scroll previous completion suggestions", silent=true, noremap=true})
--
vim.keymap.set("i", "<Tab>", function()
  return vim.fn.pumvisible() == 1 and "<C-n>" or "<Tab>"
end, { expr = true })
vim.keymap.set("i", "<S-Tab>", function()
  return vim.fn.pumvisible() == 1 and "<C-p>" or "<S-Tab>"
end, { expr = true })


-- menu - show completion popup
-- menuone - show even if one match
-- noselect - do not select first item
-- fuzzy - fuzzy find sugestions
-- noinsert - do not insert until confirmation
vim.o.completeopt = "menu,menuone,noselect,fuzzy"

vim.o.complete = ".,o" -- use buffer and omnifunc
-- Omnifunc is part of neovim's native omnicompletion
-- It is a function which returns completion candidates
-- You can manually trigger it with <C-x><C-o>
vim.bo.omnifunc = "v:lua.vim.lsp.omnifunc"
-- vim.o.pumblend
-- vim.o.dictionary
-- vim.o.tags
-- vim.o.path

-- https://stackoverflow.com/questions/19580157/to-hide-user-defined-completion-message-at-vim
vim.o.shortmess = vim.o.shortmess .. "c" -- Hide "User defined completion" messages at every completion selection

vim.o.pumborder = 'rounded'
vim.o.pumblend = 15 -- 0 fully opaque to 100 fully transparent
vim.api.nvim_set_hl(0, "PmenuSel", { blend = 0 }) -- different opacity for selected item

-- • |hl-PmenuBorder| |hl-PmenuShadow| |hl-PmenuShadowThrough| see 'pumborder'.
vim.api.nvim_set_hl(0, 'PmenuBorder', { fg = 'White' })

----------------------------- SIGNATURE_HELP ------------------------------------

vim.api.nvim_create_autocmd('LspAttach', {
  callback = function(ev)
    -- local client = vim.lsp.get_client_by_id(ev.data.client_id)
    -- if client == nil then return end
    local bufnr = ev.buf
    -- NOTE: This replaces the nvim-lspsignature plugin completely
    -- Signature help keymap
    -- vim.keymap.set("n", "<C-k>", vim.lsp.buf.signature_help, opts "Signature help")
    -- Signature help autotrigger
    vim.keymap.set("i", "(", function()
      vim.lsp.buf.signature_help()
      return "("
    end, { buffer = bufnr, expr = true, silent=true, noremap=true})
    vim.keymap.set("i", ",", function()
      vim.lsp.buf.signature_help()
      return ","
    end, { buffer = bufnr, expr = true, silent=true, noremap=true})
  end
})

----------------------------- INLAY-HINTS ------------------------------------

vim.lsp.inlay_hint.enable(true)

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

--[[vim.lsp.inlay_hint.set({
  prefix = " » ", -- prefix used before the hint text
  only_current_line = false, -- highlight for the prefix
})]]--

vim.keymap.set("n", "<leader>ih", function()
  local bufnr = vim.api.nvim_get_current_buf()
  local enabled = vim.lsp.inlay_hint.is_enabled({ bufnr = bufnr })
  vim.lsp.inlay_hint.enable(not enabled, { bufnr = bufnr })
end, { desc = "Toggle LSP inlay hints" })

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


