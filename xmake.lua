

add_requires("linux-headers", {configs = {driver_modules = true}})
-- Kernel directory and current working directory
-- local KDIR = os.getenv("KDIR") or "/lib/modules/$(shell uname -r)/build"
-- local PWD = os.curdir()

-- Define the target for the kernel module
target("simplefs")
    add_rules("platform.linux.driver")
    add_files("fs.c","super.c","inode.c","file.c","dir.c","extent.c")
    -- add_includedirs(KDIR .. "/include")
    add_cflags("-std=gnu99", "-Wall")
    add_packages("linux-headers")

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