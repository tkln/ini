#ifndef INI_H
#define INI_H

#include <sys/types.h>

struct ini_keyval {
    const char *key, *val;
};

struct ini_section {
    const char *name;
    struct ini_keyval *keyvals;
    size_t n_keyvals;
};

struct ini_file {
    struct ini_section *sections;
    size_t n_sections;
    char *buf;
};

void ini_init(struct ini_file *ini);
struct ini_section *ini_get_section(struct ini_file *ini, const char *name);
struct ini_keyval *ini_get_keyval(struct ini_section *sec, const char *key);
struct ini_keyval *ini_get_next_keyval(struct ini_section *sec,
                                       struct ini_keyval *keyval);
struct ini_section *ini_get_next_section(struct ini_file *ini,
                                         struct ini_section *sec);
struct ini_keyval *ini_get_section_keyval(struct ini_file *ini,
                                          const char *section,
                                          const char *key);
struct ini_section *ini_add_section(struct ini_file *ini, const char *name);
struct ini_keyval *ini_add_keyval(struct ini_section *sec, const char *key,
                                  const char *val);
int ini_load(const char *path, struct ini_file *ini);
int ini_save(const char *path, struct ini_file *ini);
void ini_destroy(struct ini_file *ini);

#endif
