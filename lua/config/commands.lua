
local util = require("utilities")


vim.api.nvim_create_user_command("Root", function()
  print(util.get_project_root())
end, {})


vim.api.nvim_create_user_command("PrintBuffers", function()
  local bufs = vim.fn.getbufinfo({ bufloaded = 1 })
  vim.notify(vim.inspect(bufs), vim.log.levels.INFO)
end, {})
