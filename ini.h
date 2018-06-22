#ifndef INI_H
#define INI_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ini_keyval {
    const char *key;
    char *val;
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

enum ini_parse_type {
    INI_PARSE_TYPE_INT,
    INI_PARSE_TYPE_FLOAT,
    INI_PARSE_TYPE_STRING,
};

struct ini_parsed_val {
    enum ini_parse_type type;
    union {
        int i;
        float f;
        char *s;
    };
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
                                  char *val);
int ini_load(const char *path, struct ini_file *ini);
int ini_save(const char *path, struct ini_file *ini);
int ini_parse_int(struct ini_keyval *kv, int *out);
int ini_parse_float(struct ini_keyval *kv, float *out);
int ini_parse_string(struct ini_keyval *kv, char **out);
int ini_parse_keyval(struct ini_keyval *kv, struct ini_parsed_val *out_val);
int ini_parse_section_keyval(struct ini_file *ini, const char *section,
                             const char *key, struct ini_parsed_val *out_val);
void ini_destroy(struct ini_file *ini);

#ifdef __cplusplus
}
#endif

#endif
