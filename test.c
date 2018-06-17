#include <stdlib.h>
#include <stdio.h>

#include "ini.h"
#include "test.h"

static void test_structure(void)
{
    struct ini_file ini;
    struct ini_section *sec;
    struct ini_keyval *kv;

    /* Create */

    ini_init(&ini);
    ASSERT_EQ(ini.sections, NULL, "%s", "Invalid initial state");
    ASSERT_EQ(ini.n_sections, 0, "%s", "Invalid initial state");
    ASSERT_EQ(ini.buf, NULL, "%s", "Invalid initial state");

    sec = ini_add_section(&ini, "first section");
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_EQ(ini.n_sections, 1, "Wrong number of sections: %zu",
              ini.n_sections);
    ASSERT_EQ(sec->n_keyvals, 0, "Wrong number of keyvals: %zu",
              sec->n_keyvals);
    ASSERT_STREQ(sec->name, "first section", "Invalid section name: \"%s\"",
                 sec->name);

    kv = ini_add_keyval(sec, "x", "10");
    ASSERT_NEQ(sec, NULL, "Invalid keyval");
    ASSERT_EQ(sec->n_keyvals, 1, "Wrong number of keyvals: %zu",
              sec->n_keyvals);
    ASSERT_STREQ(sec->name, "first section", "Invalid section name: \"%s\"",
                 sec->name);
    ASSERT_STREQ(kv->key, "x", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "10", "Invalid value: \"%s\"", kv->val);

    kv = ini_add_keyval(sec, "y", "20");
    ASSERT_NEQ(sec, NULL, "Invalid keyval");
    ASSERT_EQ(sec->n_keyvals, 2, "Wrong number of keyvals: %zu",
              sec->n_keyvals);
    ASSERT_STREQ(kv->key, "y", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "20", "Invalid value: \"%s\"", kv->val);

    sec = ini_add_section(&ini, "second section");
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_EQ(ini.n_sections, 2, "Wrong number of sections: %zu",
              ini.n_sections);
    ASSERT_EQ(sec->n_keyvals, 0, "Wrong number of keyvals: %zu",
              sec->n_keyvals);
    ASSERT_STREQ(sec->name, "second section", "Invalid section name: \"%s\"",
                 sec->name);

    kv = ini_add_keyval(sec, "a", "12");
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "a", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "12", "Invalid value: \"%s\"", kv->val);

    kv = ini_add_keyval(sec, "b", "34");
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "b", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "34", "Invalid value: \"%s\"", kv->val);

    /* Verify */

    sec = ini_get_section(&ini, "irst section");
    ASSERT_EQ(sec, NULL, "Invalid section: %p, name: \"%s\"", sec, sec->name);

    sec = ini_get_section(&ini, "first section");
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_STREQ(sec->name, "first section", "Invalid section name \"%s\"",
                 sec->name);

    sec = ini_get_next_section(&ini, NULL);
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_STREQ(sec->name, "first section", "Invalid section name \"%s\"",
                 sec->name);

    sec = ini_get_next_section(&ini, sec);;
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_STREQ(sec->name, "second section", "Invalid section name \"%s\"",
                 sec->name);

    kv = ini_get_keyval(sec, "b");
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "b", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "34", "Invalid value: \"%s\"", kv->val);

    kv = ini_get_next_keyval(sec, NULL);
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "a", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "12", "Invalid value: \"%s\"", kv->val);

    kv = ini_get_next_keyval(sec, kv);
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "b", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "34", "Invalid value: \"%s\"", kv->val);

    kv = ini_get_section_keyval(&ini, "second section", "a");
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "a", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "12", "Invalid value: \"%s\"", kv->val);

    ini_destroy(&ini);
}

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
    struct ini_section *sec;
    struct ini_keyval *kv;
    struct ini_file ini;
    int err;

    err = ini_load("test.ini", &ini);
    if (err)
        fprintf(stderr, "Load failed\n");

    sec = ini_get_section(&ini, "irst section");
    ASSERT_EQ(sec, NULL, "Invalid section: %p, name: \"%s\"", sec, sec->name);

    sec = ini_get_section(&ini, "first section");
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_STREQ(sec->name, "first section", "Invalid section name \"%s\"",
                 sec->name);

    sec = ini_get_next_section(&ini, NULL);
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_STREQ(sec->name, "first section", "Invalid section name \"%s\"",
                 sec->name);

    sec = ini_get_next_section(&ini, sec);;
    ASSERT_NEQ(sec, NULL, "Invalid section");
    ASSERT_STREQ(sec->name, "second section", "Invalid section name \"%s\"",
                 sec->name);

    kv = ini_get_keyval(sec, "b");
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "b", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "34", "Invalid value: \"%s\"", kv->val);

    kv = ini_get_next_keyval(sec, NULL);
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "a", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "12", "Invalid value: \"%s\"", kv->val);

    kv = ini_get_next_keyval(sec, kv);
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "b", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "34", "Invalid value: \"%s\"", kv->val);

    kv = ini_get_section_keyval(&ini, "second section", "a");
    ASSERT_NEQ(kv, NULL, "Invalid keyval");
    ASSERT_STREQ(kv->key, "a", "Invalid key: \"%s\"", kv->key);
    ASSERT_STREQ(kv->val, "12", "Invalid value: \"%s\"", kv->val);

    ini_destroy(&ini);
    return err;
}

int main(void)
{
    int ret;

    test_structure();

    ret = test_save();
    if (ret)
        return EXIT_FAILURE;

    ret = test_load();
    if (ret)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
