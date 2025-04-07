set_symbols("debug")

includes("xf_utils/xmake.lua")

-- xf_task所有的内容
function add_xf_task() 
    add_files("src/kernel/*.c")
    add_files("src/port/*.c")
    add_files("src/utils/*.c")
    add_files("src/task/*.c")
    add_includedirs("src")
    add_xf_utils("xf_utils")
end

-- xf_task移植的内容
function add_port() 
    add_files("port/*.c")
    add_includedirs("port")
    add_files("port/asm/jump_gas.S")
    add_files("port/asm/make_gas.S")
end

-- 模板化添加示例工程
function add_target(name)
    target(name)
        set_kind("binary")
        add_cflags("-Wall")
        add_files(string.format("example/%s/*.c", name))
        add_includedirs(string.format("example/%s", name))
        add_xf_task()
        add_cflags("-O0")
        add_port()
end

-- 扫描example目录下的文件夹名
for _, dir in ipairs(os.dirs("example/*")) do
    local name = path.basename(dir)
    add_target(name)
end
