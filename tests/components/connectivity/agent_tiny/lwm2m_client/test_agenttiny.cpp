/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/


/* Includes -----------------------------------------------------------------*/
#include "test_agenttiny.h"
/* Defines ------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
extern "C"
{
#include "liblwm2m.h" // lwm2m_context_t
#include "object_comm.h" // client_data_t
#include "agenttiny.h"
#include "atiny_adapter.h"
#include "atiny_rpt.h"
#include "los_sem.ph"
#include "connection.h"
#include "atiny_fota_manager.h"

    enum
    {
        OBJ_SECURITY_INDEX = 0,
        OBJ_SERVER_INDEX,
        OBJ_DEVICE_INDEX,
        OBJ_FIRMWARE_INDEX,
        OBJ_CONNECT_INDEX,
        OBJ_APP_INDEX,
        OBJ_MAX_NUM,
    };

    typedef struct
    {
        lwm2m_context_t  *lwm2m_context;
        atiny_param_t     atiny_params;
        client_data_t     client_data;
        lwm2m_object_t   *obj_array[OBJ_MAX_NUM];
        int atiny_quit;
        int reconnect_flag;
        void *quit_sem;
        int reboot_flag;
        uint8_t *recv_buffer;
    } handle_data_t;

    extern int g_reboot;

    static handle_data_t *s_handle = NULL;
    static SEM_CB_S s_sem;

    extern void atiny_event_handle(module_type_t type, int code, const char *arg, int arg_len);
    extern void observe_handle_ack(lwm2m_transaction_t *transacP, void *message);
    extern int atiny_init_objects(atiny_param_t *atiny_params, const atiny_device_info_t *device_info,
                                  handle_data_t *handle);
    extern void atiny_get_set_bootstrap_info(atiny_param_t *atiny_params,
            lwm2m_context_t *lwm2m_context, char **bs_ip, char **bs_port, bool *need_bs_flag);
    extern void lwm2m_close(lwm2m_context_t *contextP);

    extern void atiny_destroy(void *handle);
    extern void atiny_set_reboot_flag();

    extern void *atiny_mutex_create(void);
    extern void atiny_mutex_lock(void *mutex);
    extern void atiny_mutex_unlock(void *mutex);
    extern void atiny_mutex_destroy(void *mutex);
    extern int atiny_cmd_ioctl(atiny_cmd_e cmd, char *arg, int len);

    extern void ack_callback(atiny_report_type_e type, int cookie, data_send_status_e status);

    static void *stub_atiny_mutex_create(void)
    {
        memset(&s_sem, 0, sizeof(SEM_CB_S));
        return (void *)&s_sem;
    }

    static int stub_atiny_cmd_ioctl(atiny_cmd_e cmd, char *arg, int len)
    {
        if(NULL != s_handle)
        {
            s_handle->atiny_quit = 1;
            s_handle = NULL;
        }
        return 0;
    }

    static void transaction_callback(void *message)
    {
        printf("test lwm2m_transaction_t.callback\n");
    }

    static int stub_atiny_fota_manager_repot_result(atiny_fota_manager_s *thi)
    {
        return ATINY_ERR;
    }
}
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
TestAgenttiny::TestAgenttiny()
{
    TEST_ADD(TestAgenttiny::test_atiny_cmd_ioctl);
    TEST_ADD(TestAgenttiny::test_atiny_event_notify);
    TEST_ADD(TestAgenttiny::test_atiny_init);
    TEST_ADD(TestAgenttiny::test_atiny_bind);
    TEST_ADD(TestAgenttiny::test_atiny_deinit);
    TEST_ADD(TestAgenttiny::test_atiny_data_report);
    TEST_ADD(TestAgenttiny::test_atiny_data_change);
    TEST_ADD(TestAgenttiny::test_atiny_set_log_level);
    TEST_ADD(TestAgenttiny::test_atiny_get_log_level);
    TEST_ADD(TestAgenttiny::test_atiny_reconnect);
    TEST_ADD(TestAgenttiny::test_atiny_event_handle);
    TEST_ADD(TestAgenttiny::test_observe_handle_ack);
    TEST_ADD(TestAgenttiny::test_atiny_get_set_bootstrap_info);
    TEST_ADD(TestAgenttiny::test_atiny_init_objects);
}

TestAgenttiny::~TestAgenttiny()
{

}

void TestAgenttiny::test_atiny_cmd_ioctl(void)
{

}

void TestAgenttiny::test_atiny_event_notify(void)
{

}

void TestAgenttiny::test_atiny_init(void)
{
    void *pHandle = NULL;
    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    int ret = 0;

    ret = atiny_init(NULL, NULL);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_init(NULL, NULL) failed");

    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_init(...) failed");

    // fill security param
    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_init(...) failed");

    security_param->bootstrap_mode = BOOTSTRAP_CLIENT_INITIATED;
    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_init(...) failed");

    security_param->bootstrap_mode = BOOTSTRAP_SEQUENCE;
    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_RESOURCE_NOT_ENOUGH == ret), "atiny_init(...) failed");

    security_param->bootstrap_mode = (atiny_bootstrap_type_e)0xff;
    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_init(...) failed");

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_RESOURCE_NOT_ENOUGH == ret), "atiny_init(...) failed");

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    ret = atiny_init(&atiny_params, &pHandle);
    cleanStub(&si_mutex_create);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_init(...) failed");
}

void TestAgenttiny::test_atiny_bind(void)
{
    void *pHandle = NULL;
    atiny_device_info_t device_info;

    memset(&device_info, 0, sizeof(atiny_device_info_t));

    int ret = 0;
    ret = atiny_bind(NULL, NULL);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_bind(NULL, NULL) failed");

    // atiny_init()
    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    ret = atiny_init(&atiny_params, &pHandle);
    cleanStub(&si_mutex_create);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_init(...) failed");
    /////////////////////

    ret = atiny_bind(&device_info, pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_bind(NULL, NULL) failed");

    device_info.endpoint_name = (char *)"44440003";

    ret = atiny_bind(&device_info, pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_bind(NULL, NULL) failed");

    device_info.manufacturer = (char *)"Agent_Tiny";

    // atiny_init_rpt() failed
    ret = atiny_bind(&device_info, pHandle);
    TEST_ASSERT_MSG((ATINY_RESOURCE_NOT_ENOUGH == ret), "atiny_bind(NULL, NULL) failed");

    s_handle = (handle_data_t *)pHandle;
    g_reboot = 1;
    stubInfo si_cmd_ioctl;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    setStub((void *)atiny_cmd_ioctl, (void *)stub_atiny_cmd_ioctl, &si_cmd_ioctl);
    ret = atiny_bind(&device_info, pHandle);
    cleanStub(&si_mutex_create);
    cleanStub(&si_cmd_ioctl);
    g_reboot = 0;
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_bind(NULL, NULL) failed");

    // destory
    ret = 0;
    atiny_deinit(pHandle);
    TEST_ASSERT_MSG((0 == ret), "atiny_deinit(...) failed");
}

void TestAgenttiny::test_atiny_deinit(void)
{
    void *pHandle = NULL;

    int ret = 0;
    atiny_deinit(pHandle);
    TEST_ASSERT_MSG((0 == ret), "atiny_deinit(...) failed");

    // through atiny_init(...) initialize pHandle
    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    ret = atiny_init(&atiny_params, &pHandle);
    cleanStub(&si_mutex_create);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_init(...) failed");

    ret = 0;
    atiny_deinit(pHandle);
    TEST_ASSERT_MSG((0 == ret), "atiny_deinit(...) failed");

}

void TestAgenttiny::test_atiny_data_report(void)
{
    uint8_t buf[5] = {0, 1, 6, 5, 9};
    data_report_t report_data;
    int ret;
    int cnt = 0;
    report_data.buf = buf;
    report_data.callback = ack_callback;
    report_data.cookie = 0;
    report_data.len = sizeof(buf);
    report_data.type = APP_DATA;
    report_data.cookie = cnt;

    void *pHandle = NULL;
    // through atiny_init(...) initialize pHandle
    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    ret = atiny_init(&atiny_params, &pHandle);
    cleanStub(&si_mutex_create);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_init(...) failed");
    //////////

    ret = atiny_data_report(NULL, NULL);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_data_report(...) failed");

    atiny_init_rpt(); // must
    ret = atiny_data_report(pHandle, &report_data);
    TEST_ASSERT_MSG((ATINY_RESOURCE_NOT_FOUND == ret), "atiny_data_report(...) failed");

    report_data.type = FIRMWARE_UPDATE_STATE;
    ret = atiny_data_report(pHandle, &report_data);
    TEST_ASSERT_MSG((ATINY_RESOURCE_NOT_FOUND == ret), "atiny_data_report(...) failed");


    ret = 0;
    atiny_deinit(pHandle);
    TEST_ASSERT_MSG((0 == ret), "atiny_deinit(...) failed");
}

void TestAgenttiny::test_atiny_data_change(void)
{
    int ret = 0;
    void *pHandle = NULL;
    // through atiny_init(...) initialize pHandle
    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    ret = atiny_init(&atiny_params, &pHandle);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_init(...) failed");
    //////////

    char *data_type = (char *)DEVICE_MEMORY_FREE;

    ret = atiny_data_change(NULL, NULL);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_data_change(...) failed");

    // NULL == handle->lwm2m_context
    //    ret = atiny_data_change(pHandle, data_type);
    //    TEST_ASSERT_MSG((ATINY_CLIENT_UNREGISTERED == ret), "atiny_data_change(...) failed");

    handle_data_t *pHandleData = (handle_data_t *)pHandle;

    atiny_device_info_t device_info;
    memset(&device_info, 0, sizeof(atiny_device_info_t));
    device_info.endpoint_name = (char *)"44440003";
    device_info.manufacturer = (char *)"Agent_Tiny";

    ret = atiny_init_objects(&pHandleData->atiny_params, &device_info, pHandleData);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_data_change(...) failed");

    ret = atiny_data_change(pHandle, data_type);
    TEST_ASSERT_MSG((ATINY_CLIENT_UNREGISTERED == ret), "atiny_data_change(...) failed");

    if(pHandleData->lwm2m_context)
    {
        pHandleData->lwm2m_context->state = STATE_READY;
    }
    ret = atiny_data_change(pHandle, data_type);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_data_change(...) failed");

    cleanStub(&si_mutex_create);

    ret = 0;
    atiny_destroy(pHandle);
    atiny_deinit(pHandle);
    TEST_ASSERT_MSG((0 == ret), "atiny_deinit(...) failed");
}

void TestAgenttiny::test_atiny_set_log_level(void)
{
    // like test_atiny_get_log_level
}

void TestAgenttiny::test_atiny_get_log_level(void)
{
    atiny_log_e logLevel = LOG_INFO;
    atiny_log_e ori_logLevel;
    int ret = 0;

    ori_logLevel = logLevel = atiny_get_log_level();
    TEST_ASSERT_MSG((LOG_INFO == logLevel), "atiny_get_log_level() failed");

    logLevel = LOG_DEBUG;
    atiny_set_log_level(logLevel);
    TEST_ASSERT_MSG((0 == ret), "atiny_set_log_level(...) failed");

    logLevel = atiny_get_log_level();
    TEST_ASSERT_MSG((LOG_DEBUG == logLevel), "atiny_get_log_level() failed");

    atiny_set_log_level(ori_logLevel);
    TEST_ASSERT_MSG((0 == ret), "atiny_set_log_level(...) failed");
}

void TestAgenttiny::test_atiny_reconnect(void)
{
    void *pHandle = NULL;

    int ret = 0;
    ret = atiny_reconnect(pHandle);
    TEST_ASSERT_MSG((ATINY_ARG_INVALID == ret), "atiny_reconnect(...) failed");

    // through atiny_init(...) initialize pHandle
    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    ret = atiny_init(&atiny_params, &pHandle);
    cleanStub(&si_mutex_create);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_init(...) failed");

    ret = atiny_reconnect(pHandle);
    TEST_ASSERT_MSG((ATINY_OK == ret), "atiny_reconnect(...) failed");

    ret = 0;
    atiny_deinit(pHandle);
    TEST_ASSERT_MSG((0 == ret), "atiny_deinit(...) failed");
}

void TestAgenttiny::test_atiny_event_handle(void)
{
    module_type_t module_type = MODULE_LWM2M;
    lwm2m_uri_t uri;
    int code = STATE_REGISTERED;
    char *arg = NULL;
    int arg_len = 0;
    int ret = 0;

    stubInfo stub_info;
    setStub((void *)atiny_fota_manager_repot_result, (void *)stub_atiny_fota_manager_repot_result, &stub_info);
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");

    code = STATE_REG_FAILED;
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");

    module_type = MODULE_NET;
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");

    module_type = MODULE_URI;
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");

    memset(&uri, 0, sizeof(lwm2m_uri_t));
    arg = (char *)&uri;
    arg_len = sizeof(lwm2m_uri_t);

    atiny_init_rpt(); // must
    code = OBSERVE_UNSUBSCRIBE;
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");

    code = OBSERVE_SUBSCRIBE;
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");

    module_type = (module_type_t)0xff;
    atiny_event_handle(module_type, code, arg, arg_len);
    TEST_ASSERT_MSG((0 == ret), "atiny_event_handle(...) failed");
    cleanStub(&stub_info);
}


void TestAgenttiny::test_observe_handle_ack(void)
{
    lwm2m_transaction_t transaction;
    char *message = (char *)"observe_handle_ack";
    int ret = 0;

    memset(&transaction, 0, sizeof(lwm2m_transaction_t));
    transaction.cfg.callback = transaction_callback;

    transaction.ack_received = 1;
    observe_handle_ack(&transaction, message);
    TEST_ASSERT_MSG((0 == ret), "observe_handle_ack(...) failed");

    transaction.ack_received = 0;
    transaction.retrans_counter = COAP_MAX_RETRANSMIT + 1;
    observe_handle_ack(&transaction, message);
    TEST_ASSERT_MSG((0 == ret), "observe_handle_ack(...) failed");

    transaction.retrans_counter = COAP_MAX_RETRANSMIT;
    observe_handle_ack(&transaction, message);
    TEST_ASSERT_MSG((0 == ret), "observe_handle_ack(...) failed");
}

void TestAgenttiny::test_atiny_get_set_bootstrap_info(void)
{
    int ret = 0;

    atiny_security_param_t  *security_param = NULL;
    atiny_param_t atiny_params;
    memset(&atiny_params, 0, sizeof(atiny_param_t));
    atiny_params.server_params.binding = (char *)"UQ";
    atiny_params.server_params.life_time = 20;
    atiny_params.server_params.storing_cnt = 0;

    security_param = &(atiny_params.security_params[0]);

    security_param->bootstrap_mode = BOOTSTRAP_FACTORY;
    security_param->iot_server_ip = (char *)"192.168.0.106";
    security_param->bs_server_ip = (char *)"192.168.0.106";

    security_param->iot_server_port = (char *)"5683";
    security_param->bs_server_port = (char *)"5683";

    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;

    atiny_device_info_t device_info;
    memset(&device_info, 0, sizeof(atiny_device_info_t));
    device_info.endpoint_name = (char *)"44440003";
    device_info.manufacturer = (char *)"Agent_Tiny";


    char *temp_ip = NULL;
    char *temp_port = NULL;
    bool b_need_bootstrap_flag = true;

    client_data_t client_data;
    memset(&client_data, 0, sizeof(client_data_t));
    lwm2m_context_t *lwm2m_context = NULL;
    lwm2m_context = lwm2m_init(&client_data);
    TEST_ASSERT_MSG((NULL != lwm2m_context), "lwm2m_init(...) failed");

    stubInfo si_mutex_create;
    setStub((void *)atiny_mutex_create, (void *)stub_atiny_mutex_create, &si_mutex_create);
    if (NULL != lwm2m_context)
    {
        lwm2m_context->observe_mutex = atiny_mutex_create();
        if (NULL != lwm2m_context->observe_mutex)
        {
            atiny_get_set_bootstrap_info(NULL, NULL, &temp_ip,
                                         &temp_port, &b_need_bootstrap_flag);
            TEST_ASSERT_MSG((0 == ret), "atiny_get_set_bootstrap_info(...) failed");

            atiny_get_set_bootstrap_info(&atiny_params, lwm2m_context, &temp_ip,
                                         &temp_port, &b_need_bootstrap_flag);
            TEST_ASSERT_MSG((0 == ret), "atiny_get_set_bootstrap_info(...) failed");

            security_param->bootstrap_mode = BOOTSTRAP_SEQUENCE;
            atiny_get_set_bootstrap_info(&atiny_params, lwm2m_context, &temp_ip,
                                         &temp_port, &b_need_bootstrap_flag);
            TEST_ASSERT_MSG((0 == ret), "atiny_get_set_bootstrap_info(...) failed");

            security_param->bootstrap_mode = BOOTSTRAP_CLIENT_INITIATED;
            atiny_get_set_bootstrap_info(&atiny_params, lwm2m_context, &temp_ip,
                                         &temp_port, &b_need_bootstrap_flag);
            TEST_ASSERT_MSG((0 == ret), "atiny_get_set_bootstrap_info(...) failed");

            security_param->bootstrap_mode = (atiny_bootstrap_type_e)0xff;
            atiny_get_set_bootstrap_info(&atiny_params, lwm2m_context, &temp_ip,
                                         &temp_port, &b_need_bootstrap_flag);
            TEST_ASSERT_MSG((0 == ret), "atiny_get_set_bootstrap_info(...) failed");
        }
        atiny_mutex_destroy(lwm2m_context->observe_mutex);
        lwm2m_close(lwm2m_context);
    }
    cleanStub(&si_mutex_create);
}


void TestAgenttiny::test_atiny_init_objects(void)
{
    // like test_atiny_data_change()
}

/* Private functions --------------------------------------------------------*/
void TestAgenttiny::setup()
{
}

void TestAgenttiny::tear_down()
{
}

