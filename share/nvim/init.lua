local function notify(method, ...)
  assert(type(method) == 'string')
  vim.rpcnotify(1, 'qnv', method, ...)
end

local function request(method, ...)
  assert(type(method) == 'string')
  return vim.rpcrequest(1, 'qnv', method, ...)
end

_G.qnv = {
  notify = notify,
  request = request,
}
