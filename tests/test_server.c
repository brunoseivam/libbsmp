#include "../src/sllp_server.h"
#include <stdlib.h>
#include <check.h>

START_TEST (test_sllp_new)
{
    sllp_server_t *sllp = sllp_server_new();
    ck_assert(sllp != NULL);
    sllp_server_destroy(sllp);
}
END_TEST

START_TEST (test_sllp_register_variable)
{
    struct sllp_var dummy;
    enum sllp_err err;

    sllp_server_t *sllp = sllp_server_new();

    err = sllp_register_variable(NULL, NULL);
    ck_assert_msg(err == SLLP_ERR_PARAM_INVALID, "Got return %d", err);

    err = sllp_register_variable(NULL, &dummy);
    ck_assert_msg(err == SLLP_ERR_PARAM_INVALID, "Got return %d", err);

    err = sllp_register_variable(sllp, NULL);
    ck_assert_msg(err == SLLP_ERR_PARAM_INVALID, "Got return %d", err);

    dummy.info.size = 0;
    err = sllp_register_variable(sllp, &dummy);
    ck_assert_msg(err == SLLP_ERR_PARAM_OUT_OF_RANGE, "Got return %d", err);

    dummy.info.size = 200;
    err = sllp_register_variable(sllp, &dummy);
    ck_assert_msg(err == SLLP_ERR_PARAM_OUT_OF_RANGE, "Got return %d", err);

    dummy.info.size = 5;
    dummy.data = NULL;
    err = sllp_register_variable(sllp, &dummy);
    ck_assert_msg(err == SLLP_ERR_PARAM_INVALID, "Got return %d", err);

    int i;
    for(i = 0; i < 256; ++i)
    {
        uint8_t vdata[1];
        struct sllp_var *v = malloc(sizeof(*v));
        v->info.id = 0xFF;
        v->info.writable = false;
        v->info.size = 1;
        v->data = vdata;

        enum sllp_err err = sllp_register_variable(sllp, v);

        if(i < 128)
        {
            ck_assert(err == SLLP_SUCCESS);
            ck_assert(v->info.id == i);
        }
        else
        {
            ck_assert(err == SLLP_ERR_OUT_OF_MEMORY);
        }
    }

    sllp_server_destroy(sllp);
}
END_TEST

Suite *libsllpserver_suite (void)
{
    Suite *s = suite_create("SLLP Server");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_sllp_new);
    tcase_add_test(tc_core, test_sllp_register_variable);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = libsllpserver_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
