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
#include "test_atiny_fota_manager.h"

/* Defines ------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
extern "C"
{
extern void atiny_fota_manager_init(atiny_fota_manager_s *thi);
extern int atiny_fota_start_download(atiny_fota_state_s * thi, const char *uri);
extern int atiny_fota_manager_finish_download(atiny_fota_manager_s *thi, int result);
extern void lwm2m_resource_value_changed(lwm2m_context_t * contextP, lwm2m_uri_t * uriP);
extern void atiny_fota_manager_update_notify(firmware_update_rst_e rst, void *param);
extern void atiny_fota_manager_destroy(atiny_fota_manager_s *thi);
extern lwm2m_context_t* atiny_fota_manager_get_lwm2m_context(atiny_fota_manager_s *thi);
extern atiny_fota_manager_s * atiny_fota_manager_get_instance(void);
extern char * atiny_fota_manager_get_pkg_uri(const atiny_fota_manager_s *thi);
extern int atiny_fota_manager_get_state(const atiny_fota_manager_s *thi);
extern int atiny_fota_manager_get_update_result(const atiny_fota_manager_s *thi);
extern void atiny_fota_manager_set_update_result(atiny_fota_manager_s *thi, atiny_update_result_e result);
extern int atiny_fota_manager_get_deliver_method(const atiny_fota_manager_s *thi);

extern int atiny_fota_downloading_state_finish_download(atiny_fota_state_s *thi, int result);
extern int atiny_fota_idle_state_report_result(atiny_fota_state_s * thi);
extern int lwm2m_stringToUri(const char * buffer, size_t buffer_len, lwm2m_uri_t * uriP);
extern atiny_update_info_s *atiny_update_info_get_instance(void);
extern int atiny_update_info_set(atiny_update_info_s *thi, atiny_fota_storage_device_s *device);
}

int si_atiny_fota_start_download()
{
    return -1;
}

int si_atiny_fota_downloading_state_finish_download(atiny_fota_state_s *thi, int result)
{
    return -1;
}

int si_atiny_fota_idle_state_report_result(atiny_fota_state_s * thi)
{
    return -1;
}

int si_lwm2m_stringToUri(const char * buffer, size_t buffer_len, lwm2m_uri_t * uriP)
{
    return 0;
}

void si_lwm2m_resource_value_changed(lwm2m_context_t * contextP, lwm2m_uri_t * uriP)
{
    return;
}

int si_atiny_update_info_set(atiny_update_info_s *thi, atiny_fota_storage_device_s *device)

{
    return 0;
}

atiny_update_info_s * si_atiny_update_info_get_instance(void)
{
    return NULL;
}




struct atiny_fota_manager_tag_s
{
    char *pkg_uri;
    atiny_fota_state_e state;
    atiny_update_result_e update_result;
    atiny_fota_idle_state_s idle_state;
    atiny_fota_downloading_state_s downloading_state;
    atiny_fota_downloaded_state_s downloaded_state;
    atiny_fota_updating_state_s updating_state;
    atiny_fota_state_s *current;
    atiny_fota_storage_device_s *device;
    lwm2m_context_t*  lwm2m_context;
    bool init_flag;
};

TestAtinyFotaManager::TestAtinyFotaManager()
{
   TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_start_download);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_execute_update);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_finish_download);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_repot_result);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_set_state);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_set_storage_device);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_get_storage_device);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_update_notify);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_destroy);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_set_lwm2m_context);
//    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_get_lwm2m_context);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_get_instance);
    TEST_ADD(TestAtinyFotaManager::test_atiny_fota_manager_get_pkg_uri);
}

TestAtinyFotaManager::~TestAtinyFotaManager()
{
}

void TestAtinyFotaManager::test_atiny_fota_manager_get_pkg_uri()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;
    test_thi = (atiny_fota_manager_s*)malloc(sizeof(atiny_fota_manager_s));
    memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    test_thi->pkg_uri = (char *)atiny_malloc(sizeof(char));
    char * test_uri = atiny_fota_manager_get_pkg_uri(test_thi);
    TEST_ASSERT_MSG((NULL != test_uri), "atiny_fota_manager_get_pkg_uri() failed");

    ret = atiny_fota_manager_get_state(test_thi);
    TEST_ASSERT_MSG((ret == 0), "atiny_fota_manager_get_state() failed");

    ret = atiny_fota_manager_get_update_result(test_thi);
    TEST_ASSERT_MSG((ret == 0), "atiny_fota_manager_get_state() failed"); 

    atiny_fota_manager_set_update_result(test_thi,1);
    TEST_ASSERT_MSG((test_thi->update_result == 1), "atiny_fota_manager_set_update_result() failed");

    ret = atiny_fota_manager_get_deliver_method(test_thi);
    TEST_ASSERT_MSG((ret == 0), "atiny_fota_manager_get_deliver_method() failed"); 

    atiny_free(test_thi->pkg_uri);
    atiny_free(test_thi);
    
}

void TestAtinyFotaManager::test_atiny_fota_manager_start_download()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;
    char test_uri[]  = "hello,world";
    uint32_t test_len = 11;
//    ret = atiny_fota_manager_start_download(test_thi, test_uri, test_len);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_start_download() failed");

    test_thi = (atiny_fota_manager_s*)malloc(sizeof(atiny_fota_manager_s));
    memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    ret = atiny_fota_manager_start_download(test_thi, test_uri, test_len);
    TEST_ASSERT_MSG((ret == ATINY_ERR), "atiny_fota_manager_start_download() failed");
    
    test_thi->current = (atiny_fota_state_s*)malloc(sizeof(atiny_fota_state_s));
        if(NULL != test_thi->current)   memset(test_thi->current, 0, sizeof(atiny_fota_state_s));
    test_thi->pkg_uri = (char *)malloc(sizeof(char));
        if(NULL != test_thi->pkg_uri)  memset(test_uri, 0, sizeof(char));

    //atiny_fota_manager_init(test_thi);

    test_thi->current->start_download = atiny_fota_start_download;

    stubInfo si;
    setStub((void *)atiny_fota_start_download, (void *)si_atiny_fota_start_download, &si);
    ret = atiny_fota_manager_start_download(test_thi, test_uri, test_len);
    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_start_download() failed");
    cleanStub(&si);
    printf("%s:%d\n\n",__FUNCTION__,__LINE__);

    free(test_thi->current);printf("%s:%d\n\n",__FUNCTION__,__LINE__);
    free(test_thi->pkg_uri);
    free(test_thi);
    //free(test_uri);
}

void TestAtinyFotaManager::test_atiny_fota_manager_execute_update()
{   
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;     
//    ret = atiny_fota_manager_start_download(test_thi, NULL, NULL);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_start_download() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    ret = atiny_fota_manager_start_download(test_thi, NULL, 0);
    TEST_ASSERT_MSG((ret == ATINY_ERR), "atiny_fota_manager_start_download() failed");

//    test_thi->current = (atiny_fota_state_s*)atiny_malloc(sizeof(atiny_fota_state_s));
//        if(NULL != test_thi->current)
//            memset(test_thi->current, 0, sizeof(atiny_fota_state_s));

    atiny_fota_manager_init(test_thi);
    ret = atiny_fota_manager_execute_update(test_thi);
    TEST_ASSERT_MSG((ret == ATINY_ERR), "atiny_fota_manager_execute_update() failed"); 

    //atiny_free(test_thi->current);
    atiny_free(test_thi);
}

void TestAtinyFotaManager::test_atiny_fota_manager_finish_download()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;     
//    ret = atiny_fota_manager_start_download(test_thi, NULL, NULL);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_start_download() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    ret = atiny_fota_manager_finish_download(test_thi,0);
    TEST_ASSERT_MSG((ret == ATINY_ERR), "atiny_fota_manager_start_download() failed");

    test_thi->current = (atiny_fota_state_s*)atiny_malloc(sizeof(atiny_fota_state_s));
        if(NULL != test_thi->current)
            memset(test_thi->current, 0, sizeof(atiny_fota_state_s));
        
    test_thi->current->finish_download = atiny_fota_downloading_state_finish_download;
    stubInfo si;
    setStub((void *)atiny_fota_downloading_state_finish_download, (void *)si_atiny_fota_downloading_state_finish_download, &si);
    
    ret = atiny_fota_manager_finish_download(test_thi,0);
    TEST_ASSERT_MSG((ret == -1), "atiny_fota_manager_execute_update() failed"); 

    atiny_free(test_thi->current);
    atiny_free(test_thi);
    cleanStub(&si);
}

void TestAtinyFotaManager::test_atiny_fota_manager_repot_result()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;     
//    ret = atiny_fota_manager_start_download(test_thi, NULL, NULL);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_repot_result() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    ret = atiny_fota_manager_repot_result(test_thi);
    TEST_ASSERT_MSG((ret == ATINY_ERR), "atiny_fota_manager_start_download() failed");

    test_thi->current = (atiny_fota_state_s*)atiny_malloc(sizeof(atiny_fota_state_s));
        if(NULL != test_thi->current)
            memset(test_thi->current, 0, sizeof(atiny_fota_state_s));  

    test_thi->current->repot_result = atiny_fota_idle_state_report_result;
    stubInfo si;
    setStub((void *)atiny_fota_idle_state_report_result, (void *)si_atiny_fota_idle_state_report_result,&si);
    ret = atiny_fota_manager_repot_result(test_thi);
    TEST_ASSERT_MSG((ret == -1), "atiny_fota_manager_start_download() failed");

    atiny_free(test_thi->current);
    atiny_free(test_thi);
    cleanStub(&si);
}

void TestAtinyFotaManager::test_atiny_fota_manager_set_state()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;     
//    ret = atiny_fota_manager_set_state(test_thi, 0);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_set_state() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));

    ret = atiny_fota_manager_set_state(test_thi, ATINY_FOTA_UPDATING + 1);
    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_start_download() failed");

    stubInfo si_1, si_2;
    setStub((void *)lwm2m_stringToUri,(void *)si_lwm2m_stringToUri, &si_1);
    setStub((void *)lwm2m_resource_value_changed, (void *)si_lwm2m_resource_value_changed, &si_2);
    ret = atiny_fota_manager_set_state(test_thi, ATINY_FOTA_UPDATING);
    TEST_ASSERT_MSG((ret == ATINY_OK), "atiny_fota_manager_start_download() failed");

    atiny_free(test_thi);
    cleanStub(&si_1);
    cleanStub(&si_2);
}

void TestAtinyFotaManager::test_atiny_fota_manager_set_storage_device()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;  
    atiny_fota_storage_device_s * test_device = NULL;
//    ret = atiny_fota_manager_set_storage_device(test_thi, test_device);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_set_storage_device() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));

    stubInfo si_1, si_2;
    setStub((void *)atiny_update_info_set,(void *)si_atiny_update_info_set, &si_1);
    setStub((void *)atiny_update_info_get_instance, (void *)si_atiny_update_info_get_instance, &si_2);   
    ret = atiny_fota_manager_set_storage_device(test_thi, test_device);
    TEST_ASSERT_MSG((ret == 0), "atiny_fota_manager_start_download() failed");

    atiny_free(test_thi);
    cleanStub(&si_1);
    cleanStub(&si_2);
}

void TestAtinyFotaManager::test_atiny_fota_manager_get_storage_device()
{
    atiny_fota_manager_s * test_thi = NULL;  
    atiny_fota_storage_device_s * test_device = NULL;
//    ret = atiny_fota_manager_set_storage_device(test_thi, test_device);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_get_storage_device() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    test_thi->device = NULL;
    test_device = atiny_fota_manager_get_storage_device(test_thi);
    TEST_ASSERT_MSG((test_device == NULL), "atiny_fota_manager_start_download() failed");

    atiny_free(test_thi);
}

void TestAtinyFotaManager::test_atiny_fota_manager_update_notify()
{
    firmware_update_rst_e test_rst = FIRMWARE_UPDATE_RST_SUCCESS;
    atiny_fota_manager_s * test_thi = NULL; 
    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    test_thi->current = (atiny_fota_state_s *)atiny_malloc(sizeof(atiny_fota_state_s));
        if(NULL != test_thi->current)
            memset(test_thi->current, 0, sizeof(atiny_fota_state_s));

    test_thi->current->finish_download = atiny_fota_downloading_state_finish_download;
    stubInfo si;
    setStub((void *)atiny_fota_downloading_state_finish_download, (void *)si_atiny_fota_downloading_state_finish_download, &si);

    atiny_fota_manager_update_notify(test_rst,(void *)test_thi) ;

    atiny_free(test_thi->current);
    atiny_free(test_thi);
    cleanStub(&si);
}

void TestAtinyFotaManager::test_atiny_fota_manager_destroy()
{
    atiny_fota_manager_s * test_thi = NULL;  
    atiny_fota_storage_device_s * test_device = NULL;
//    ret = atiny_fota_manager_set_storage_device(test_thi, test_device);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_destroy() failed");

    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
        if(NULL != test_thi)
            memset(test_thi, 0, sizeof(atiny_fota_manager_s));
    test_thi->pkg_uri = (char *)atiny_malloc(sizeof(char));
    atiny_fota_manager_destroy(test_thi);
    //TEST_ASSERT_MSG((0 == strcmp(test_thi->pkg_uri,'\0')), "atiny_fota_manager_destroy() failed");
    atiny_free(test_thi);
}

void TestAtinyFotaManager::test_atiny_fota_manager_set_lwm2m_context()
{
    int ret = 0;
    atiny_fota_manager_s * test_thi = NULL;  
    atiny_fota_storage_device_s * test_device = NULL;
//    ret = atiny_fota_manager_set_storage_device(test_thi, NULL);
//    TEST_ASSERT_MSG((ret == ATINY_ARG_INVALID), "atiny_fota_manager_set_lwm2m_context() failed");
    test_thi = (atiny_fota_manager_s*)atiny_malloc(sizeof(atiny_fota_manager_s));
            if(NULL != test_thi)
                memset(test_thi, 0, sizeof(atiny_fota_manager_s));

    lwm2m_context_t*  test_lwm2m_context = NULL;
    ret = atiny_fota_manager_set_lwm2m_context(test_thi,test_lwm2m_context);
    atiny_fota_manager_get_lwm2m_context(test_thi);
    TEST_ASSERT_MSG((ret == ATINY_OK), "atiny_fota_manager_set_lwm2m_context() failed");
    atiny_free(test_thi);
}

void TestAtinyFotaManager::test_atiny_fota_manager_get_instance()
{
    atiny_fota_manager_s * test_thi = atiny_fota_manager_get_instance();
    TEST_ASSERT_MSG((NULL != test_thi), "atiny_fota_manager_get_instance() failed");
}

void TestAtinyFotaManager::setup()
{
    std::cout<<"in steup\n";
}

void TestAtinyFotaManager::tear_down()
{
    std::cout<<"in teardown\n";
}




