#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

#define ASSERT(v_, msg_, ...)                                           \
        do {                                                            \
            if (v_)                                                     \
                break;                                                  \
            fprintf(stderr, "%s, %d: Assertion failed %s: " msg_ "\n",  \
                    __FUNCTION__,                                       \
                  __LINE__, #v_, __VA_ARGS__);                          \
        } while (0);

#define ASSERT_EQ(a_, b_, msg_, ...)                                    \
        do {                                                            \
            if ((a_) == (b_))                                           \
                break;                                                  \
            fprintf(stderr,                                             \
                    "%s, %d: Assertion failed %s == %s: " msg_ "\n",    \
                   __FUNCTION__, __LINE__, #a_, #b_, __VA_ARGS__);      \
        } while (0);

#define ASSERT_NEQ(a_, b_, msg_)                                        \
        do {                                                            \
            if ((a_) != (b_))                                           \
                break;                                                  \
            fprintf(stderr,                                             \
                    "%s, %d: Assertion failed %s != %s: " msg_ "\n",    \
                   __FUNCTION__, __LINE__, #a_, #b_);                   \
        } while (0);

#define ASSERT_FEQ(a_, b_, e_, msg_, ...)                               \
        do {                                                            \
            if (abs(a_ - b_) < e_)                                      \
                break;                                                  \
            fprintf(stderr,                                             \
                    "%s, %d: Assertion failed %s == %s: " msg_ "\n",    \
                   __FUNCTION__, __LINE__, #a_, #b_, __VA_ARGS__);      \
        } while (0);

#define ASSERT_STREQ(a_, b_, msg_, ...)                                 		\
        do {                                                            		\
            if (!strcmp(a_, b_))                                        		\
                break;                                                  		\
            fprintf(stderr,                                             		\
                    "%s, %d: Assertion failed %s == %s: " msg_ "\n",	\
                   __FUNCTION__, __LINE__, #a_, #b_, __VA_ARGS__); 				\
        } while (0);

#endif
