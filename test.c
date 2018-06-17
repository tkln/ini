#include <stdlib.h>
#include <stdio.h>

#include "ini.h"

static int test_save(void)
{
    struct ini_file ini;
    struct ini_section *sec;
    int err;

    ini_init(&ini);
    sec = ini_add_section(&ini, "first section");
    ini_add_keyval(sec, "x", "10");
    ini_add_keyval(sec, "y", "20");
    sec = ini_add_section(&ini, "second section");
    ini_add_keyval(sec, "a", "12");
    ini_add_keyval(sec, "b", "34");

    err = ini_save("test.ini", &ini);
    if (err)
        fprintf(stderr, "Save failed\n");
    ini_destroy(&ini);
    return err;
}

static int test_load(void)
{
    struct ini_file ini;
    int err;

    err = ini_load("test.ini", &ini);
    if (err)
        fprintf(stderr, "Load failed\n");
    ini_destroy(&ini);
    return err;
}

int main(void)
{
    int ret;

    ret = test_save();
    if (ret)
        return EXIT_FAILURE;

    ret = test_load();
    if (ret)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
