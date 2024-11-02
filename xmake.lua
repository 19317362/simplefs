-- add_requires("linux-headers", {configs = {driver_modules = true}})
-- Kernel directory and current working directory
-- local KDIR = "/usr/src/linux-headers-6.8.0-47-generic"
-- local PWD = os.curdir()
add_rules("mode.debug", "mode.release")
-- Set the default mode to debug
set_defaultmode("debug")

set_project("test-asio")
set_exceptions("cxx")

-- 默认都使用动态库
add_requireconfs("*", {configs = {shared = true, YW_PLAT_ID = get_config("YW_PLAT_ID")}})
-- 使用C++20
set_languages("cxx20")

-- 默认都使用动态库
add_requireconfs("*", {configs = {shared = true, YW_PLAT_ID = get_config("YW_PLAT_ID")}})

-- 依赖项目工程
add_requires("asio")
add_requires("gtest")
-- add_requires("linux-headers", {configs = {driver_modules = false}})

-- 共享库
target("yw_fpa")
    set_kind("shared")
    add_files("src/yw_fpa.cpp")
    add_packages("asio")
    add_includedirs("./pb")
    add_files("./pb/*.c")    

-- src 目录下的 fpa-simu.c 生成 fpa-simu 可执行程序, 模拟 fpa,开发时使用
target("fpa-simu")
    set_kind("binary")
    add_packages("asio")
    add_deps("yw_fpa")
    add_files("src/fpa-simu.cpp")
    add_includedirs("./pb")

-- 使用gtest测试用例
target("test-fpa")
    set_kind("binary")
    add_packages("asio")
    add_packages("gtest", "gtest_main")
    add_deps("yw_fpa")
    --add_defines("__STDC_LIMIT_MACROS", "__STDC_CONSTANT_MACROS")
    --add_includedirs("/usr/include/libnl3")
    add_files("test-fpa/*.cpp")  
    add_includedirs("./pb")

target("test-asio")
    set_kind("binary")
    add_packages("asio")
    add_deps("yw_fpa")
    --add_defines("__STDC_LIMIT_MACROS", "__STDC_CONSTANT_MACROS")
    --add_includedirs("/usr/include/libnl3")
    add_files("test-asio/*.cpp")  


-- Define the target for the kernel module
-- target("simplefs")
--     add_rules("platform.linux.driver")
--     add_files("fs.c","super.c","inode.c","file.c","dir.c","extent.c")
--     add_cflags("-std=gnu99", "-Wall", "-nostdinc")
--     add_defines("__KERNEL__")
--     add_cflags("-include " .. path.join(KDIR, "include/linux/compiler-version.h"))
--     add_cflags("-include " .. path.join(KDIR, "include/linux/kconfig.h"))
--     add_cflags("-I" .. path.join(KDIR, "arch/x86/include"))
--     add_cflags("-I" .. path.join(KDIR, "arch/x86/include/generated"))
--     add_cflags("-I" .. path.join(KDIR, "include"))
--     add_cflags("-I" .. path.join(KDIR, "arch/x86/include/uapi"))
--     add_cflags("-I" .. path.join(KDIR, "arch/x86/include/generated/uapi"))
--     add_cflags("-I" .. path.join(KDIR, "include/uapi"))
--     add_cflags("-I" .. path.join(KDIR, "include/generated/uapi"))
--     add_cflags("-I" .. path.join(KDIR, "ubuntu/include"))
--     add_cflags("-include " .. path.join(KDIR, "include/linux/compiler_types.h"))
--     set_values("linux.driver.linux-headers", KDIR)

-- Define the target for mkfs.simplefs
-- target("mkfs.simplefs")
--     set_kind("binary")
--     add_files("mkfs.c")
--     add_cflags("-std=gnu99", "-Wall")

-- Define the task to create the image
task("create_image")
    set_menu {
        usage = "xmake create_image [options]",
        description = "Create a filesystem image",
        options = {
            {'i', "image", "kv", "test.img", "The name of the image file"},
            {'s', "size", "kv", "200", "The size of the image in MB"}
        }
    }
    on_run(function (opt)
        local image = opt.image or "test.img"
        local size = opt.size or "200"
        os.exec("dd if=/dev/zero of=" .. image .. " bs=1M count=" .. size)
        os.exec("./mkfs.simplefs " .. image)
    end)

-- Define the task to create the journal
task("create_journal")
    set_menu {
        usage = "xmake create_journal [options]",
        description = "Create a journal image",
        options = {
            {'j', "journal", "kv", "journal.img", "The name of the journal file"},
            {'s', "size", "kv", "8", "The size of the journal in MB"}
        }
    }
    on_run(function (opt)
        local journal = opt.journal or "journal.img"
        local size = opt.size or "8"
        os.exec("dd if=/dev/zero of=" .. journal .. " bs=1M count=" .. size)
        os.exec("mke2fs -b 4096 -O journal_dev " .. journal)
    end)