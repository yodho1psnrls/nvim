-- Highlight todo, notes, etc in comments
return {

  {
    'folke/todo-comments.nvim',

    --event = 'VimEnter',
    event = 'BufReadPost',

    dependencies = {
      'nvim-lua/plenary.nvim'
    },

    opts = {
      signs = false
    },

  },

}
