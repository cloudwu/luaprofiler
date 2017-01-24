local p = require "profiler"

local function fact(n)
	if n <=1 then
		return 1
	else
		return fact(n-1) * n
	end
end

local function foo(n)
	local s = 0
	for i=1,n do
		s = s + fact(i)
	end
	return s
end

p.start(1000,10)

foo(20)

local info,n = p.info()

p.stop()

for filename, line_t in pairs(info) do
	for line, count in pairs(line_t) do
		print(filename, line, count)
	end
end

print("total=", n)


