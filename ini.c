#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "ini.h"

void ini_init(struct ini_file *ini)
{
    memset(ini, 0, sizeof(*ini));
}

struct ini_section *ini_get_section(struct ini_file *ini, const char *name)
{
    size_t i;

    for (i = 0; i < ini->n_sections; ++i)
        if (!strcmp(ini->sections[i].name, name))
            return ini->sections + i;

    return NULL;
}

struct ini_keyval *ini_get_keyval(struct ini_section *sec, const char *key)
{
    size_t i;

    if (!sec)
        return NULL;

    for (i = 0; i < sec->n_keyvals; ++i)
        if (!strcmp(sec->keyvals[i].key, key))
            return sec->keyvals + i;

    return NULL;
}

struct ini_keyval *ini_get_next_keyval(struct ini_section *sec,
                                       struct ini_keyval *keyval)
{
    if (!keyval)
        return sec->keyvals;
    if ((keyval + 1 - sec->keyvals) >= sec->n_keyvals)
        return NULL;
    return keyval + 1;
}

struct ini_section *ini_get_next_section(struct ini_file *ini,
                                         struct ini_section *sec)
{
    if (!sec)
        return ini->sections;
    if ((sec + 1 - ini->sections) >= ini->n_sections)
        return NULL;
    return sec + 1;
}

struct ini_keyval *ini_get_section_keyval(struct ini_file *ini,
                                          const char *section,
                                          const char *key)
{
    return ini_get_keyval(ini_get_section(ini, section), key);
}

struct ini_section *ini_add_section(struct ini_file *ini, const char *name)
{
    ini->sections = realloc(ini->sections, (ini->n_sections + 1) *
                                           sizeof(ini->sections[0]));
    ini->sections[ini->n_sections] = (struct ini_section){ .name = name };
    ++ini->n_sections;

    return &ini->sections[ini->n_sections - 1];
}

struct ini_keyval *ini_add_keyval(struct ini_section *sec, const char *key,
                                  char *val)
{
    sec->keyvals = realloc(sec->keyvals, (sec->n_keyvals + 1) *
                                         sizeof(sec->keyvals[0]));
    sec->keyvals[sec->n_keyvals] = (struct ini_keyval){ key, val };
    ++sec->n_keyvals;

    return &sec->keyvals[sec->n_keyvals - 1];
}

int ini_load(const char *path, struct ini_file *ini)
{
    char *iter, *begin_sec, *end_sec, *keyval_assign, *newline, *prev_newline;
    struct ini_section *sec;
    struct stat st;
    int err;
    int fd;

    fd = open(path, O_RDONLY);
    if (!fd) {
        perror("open");
        return -1;
    }

    err = fstat(fd, &st);
    if (err) {
        perror("stat");
        close(fd);
        return -1;
    }

    ini_init(ini);

    ini->buf = calloc(1, st.st_size + 1);
    read(fd, ini->buf, st.st_size);
    close(fd);

    iter = ini->buf;
    while (iter < ini->buf + st.st_size) {
        prev_newline = newline;
        begin_sec = strchr(iter, '[');
        keyval_assign = strchr(iter, '=');
        newline = strchr(iter, '\n');

        if ((begin_sec < newline || begin_sec < keyval_assign) &&
                keyval_assign && begin_sec) {
            end_sec = strchr(iter, ']');
            *end_sec = '\0';
            sec = ini_add_section(ini, begin_sec + 1);
            iter = end_sec + 1;
        } else if (newline < keyval_assign ) {
            iter = keyval_assign;
        } else {
            *keyval_assign = '\0';
            *newline = '\0';
            ini_add_keyval(sec, prev_newline + 1, keyval_assign + 1);
            iter = newline + 1;
        }
    }

    return 0;
}

int ini_save(const char *path, struct ini_file *ini)
{
    struct ini_section *sec;
    struct ini_keyval *kv;
    int fd;

    fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (!fd) {
        perror("open");
        return -1;
    }

    for (sec = ini->sections; sec; sec = ini_get_next_section(ini, sec)) {
        dprintf(fd, "[%s]\n", sec->name);
        for (kv = sec->keyvals; kv; kv = ini_get_next_keyval(sec, kv)) {
            dprintf(fd, "%s=%s\n", kv->key, kv->val);
        }
    }

    close(fd);
    return 0;
}

int ini_parse_int(struct ini_keyval *kv, int *out)
{
    int val;
    char *endptr;

    if (!kv)
        return -1;

    val = strtol(kv->val, &endptr, 0);

    if (endptr < kv->val + strlen(kv->val) || endptr == kv->val)
        return -1;

    *out = val;

    return 0;
}

int ini_parse_float(struct ini_keyval *kv, float *out)
{
    float val;
    char *endptr;

    if (!kv)
        return -1;

    val = strtof(kv->val, &endptr);

    if (endptr < kv->val + strlen(kv->val) || endptr == kv->val)
        return -1;

    *out = val;

    return 0;
}

int ini_parse_string(struct ini_keyval *kv, char **out)
{
    if (!kv)
        return -1;

    if (!kv->val)
        return -1;

    *out = kv->val;

    return 0;
}

int ini_parse_section_keyval(struct ini_file *ini, const char *section,
                             const char *key, struct ini_parsed_val *out_val)
{
    struct ini_keyval *kv;
    struct ini_parsed_val parsed;
    int err;

    kv = ini_get_section_keyval(ini, section, key);
    if (!kv)
        return -1;

    parsed.type = INI_PARSE_TYPE_INT;
    err = ini_parse_int(kv, &parsed.i);
    if (!err)
        goto out;

    parsed.type = INI_PARSE_TYPE_FLOAT;
    err = ini_parse_float(kv, &parsed.f);
    if (!err)
        goto out;

    parsed.type = INI_PARSE_TYPE_STRING;
    parsed.s = kv->val;
    err = ini_parse_float(kv, &parsed.f);
    if (!err)
        goto out;

    return -1;

out:
    *out_val = parsed;
    return 0;
}

void ini_destroy(struct ini_file *ini)
{
    struct ini_section *sec;

    for (sec = ini->sections; sec; sec = ini_get_next_section(ini, sec))
        free(sec->keyvals);
    free(ini->sections);
    free(ini->buf);
}
