-- https://www.reddit.com/r/neovim/comments/10q2mjq/i_dont_really_get_folding/

-- TODO: It either uses the lsp or the parser(treesitter)
-- , currently it uses the lsp, but later if you happen
-- to need folding for more files that you dont have lsp
-- for, change it to use treesitter

-- NOTE:
-- zc folds by method, h or l on the folded line, unfolds it

--  BUG: Sometimes the current numberline lags out, such that
--  two line numbers are rendered for a very short period of time


local ftMap = {
    vim = 'indent',
    python = {'indent'},
    git = '',
}


local handler = function(virtText, lnum, endLnum, width, truncate)
    local newVirtText = {}
    local suffix = (' 󰁂 %d '):format(endLnum - lnum)
    local sufWidth = vim.fn.strdisplaywidth(suffix)
    local targetWidth = width - sufWidth
    local curWidth = 0
    for _, chunk in ipairs(virtText) do
        local chunkText = chunk[1]
        local chunkWidth = vim.fn.strdisplaywidth(chunkText)
        if targetWidth > curWidth + chunkWidth then
            table.insert(newVirtText, chunk)
        else
            chunkText = truncate(chunkText, targetWidth - curWidth)
            local hlGroup = chunk[2]
            table.insert(newVirtText, {chunkText, hlGroup})
            chunkWidth = vim.fn.strdisplaywidth(chunkText)
            -- str width returned from truncate() may less than 2nd argument, need padding
            if curWidth + chunkWidth < targetWidth then
                suffix = suffix .. (' '):rep(targetWidth - curWidth - chunkWidth)
            end
            break
        end
        curWidth = curWidth + chunkWidth
    end
    table.insert(newVirtText, {suffix, 'MoreMsg'})
    return newVirtText
end


return {

  'kevinhwang91/nvim-ufo',
  dependencies = 'kevinhwang91/promise-async',

  -- lazy = true,
  -- event = { 'LspAttach' }, -- Remove that if you change it to treesitter instead of lsp
  -- event = { 'BufReadPost', 'BufWinEnter' },
  -- keys = {
  --   'zR', -- Open all folds
  --   'zM', -- Close all folds
  --   'za', -- Toggle fold
  --   'zc', -- Close fold
  --   'zo', -- Open fold
  -- },

  config = function ()

    -- Option 2: nvim lsp as LSP client
    -- Tell the server the capability of foldingRange,
    -- Neovim hasn't added foldingRange to default capabilities, users must add it manually
    local capabilities = vim.lsp.protocol.make_client_capabilities()
    capabilities.textDocument.foldingRange = {
      dynamicRegistration = false,
      lineFoldingOnly = true
    }
    local language_servers = require("lspconfig").util._available_servers() -- or list servers manually like {'gopls', 'clangd'}
    for _, ls in ipairs(language_servers) do
      require('lspconfig')[ls].setup({
        capabilities = capabilities
        -- you can add other fields for setting up lsp server in this table
      })
    end

    require('ufo').setup({
      fold_virt_text_handler = handler,

      open_fold_hl_timeout = 150,
      close_fold_kinds_for_ft = {
        default = {'imports', 'comment'},
        json = {'array'},
        c = {'comment', 'region'}
      },
      preview = {
        win_config = {
          --border = {'', '─', '', '', '', '─', '', ''},
          border = "rounded",

          --focusable = false, -- I dont think those two are options here
          --silent = true,

          winhighlight = 'Normal:Folded',
          winblend = 0,
        },
        mappings = {
          scrollU = '<C-u>',
          scrollD = '<C-d>',
          jumpTop = '[',
          jumpBot = ']'
        }
      },
      provider_selector = function(bufnr, filetype, buftype)
        -- if you prefer treesitter provider rather than lsp,
        -- return ftMap[filetype] or {'treesitter', 'indent'}
        return ftMap[filetype]

        -- refer to ./doc/example.lua for detail
      end
    })

    vim.keymap.set('n', 'zR', require('ufo').openAllFolds)
    vim.keymap.set('n', 'zM', require('ufo').closeAllFolds)
    vim.keymap.set('n', 'zr', require('ufo').openFoldsExceptKinds)
    vim.keymap.set('n', 'zm', require('ufo').closeFoldsWith) -- closeAllFolds == closeFoldsWith(0)


    -- This is managed in autocmds.lua
    --vim.cmd [[ autocmd CursorHold * lua require('ufo').peekFoldedLinesUnderCursor() ]]
    -- vim.keymap.set('n', 'K', function()
    --   local winid = require('ufo').peekFoldedLinesUnderCursor()
    --   if not winid then
    --     vim.lsp.buf.hover()
    --   end
    -- end)


    ------------------------------------------------------------------------------

    --

    -- Option 3: treesitter as a main provider instead
    -- (Note: the `nvim-treesitter` plugin is *not* needed.)
    -- ufo uses the same query files for folding (queries/<lang>/folds.scm)
    -- performance and stability are better than `foldmethod=nvim_treesitter#foldexpr()`
    -- require('ufo').setup({
    --     provider_selector = function(bufnr, filetype, buftype)
    --         return {'treesitter', 'indent'}
    --     end
    -- })

  end,

}
