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
#include "test_object_security.h"
/* Defines ------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
extern "C"
{
#include "object_comm.h"
    extern void copy_security_object(lwm2m_object_t *objectDest, lwm2m_object_t *objectSrc);
    extern void display_security_object(lwm2m_object_t *object);
    extern void clean_security_object(lwm2m_object_t *objectP);
    extern lwm2m_object_t *get_security_object(uint16_t serverId,
            const char *serverUri,
            char *bsPskId,
            char *psk,
            uint16_t pskLen,
            bool isBootstrap);
    extern char *security_get_uri(lwm2m_object_t *objectP, uint16_t secObjInstID);

    extern lwm2m_data_t *lwm2m_data_new(int size);
    extern void *lwm2m_malloc(size_t s);


    static lwm2m_data_t *stub_lwm2m_data_new(int size)
    {
        return NULL;
    }

    static void *stub_lwm2m_malloc(size_t s)
    {
        return NULL;
    }

    static void *stub_lwm2m_malloc_ex(size_t s)
    {
        static int i = 0;
        if( i++ == 0)
        {
            void *mem = NULL;
            mem = atiny_malloc(s);
            return mem;
        }
        return NULL;
    }
}
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
TestObjectSecurity::TestObjectSecurity()
{
    TEST_ADD(TestObjectSecurity::test_copy_security_object);
    TEST_ADD(TestObjectSecurity::test_display_security_object);
    TEST_ADD(TestObjectSecurity::test_clean_security_object);
    TEST_ADD(TestObjectSecurity::test_get_security_object);
    TEST_ADD(TestObjectSecurity::test_security_get_uri);

    TEST_ADD(TestObjectSecurity::test_prv_security_read);
}
TestObjectSecurity::~TestObjectSecurity()
{
}

void TestObjectSecurity::test_copy_security_object(void)
{
    uint16_t serverId = 123;
    char *serverUri = (char *)"coap://192.168.1.106:5368";
    bool b_need_bootstrap_flag = true;
    char *psk_Id = NULL;
    char *psk = NULL;
    unsigned short psk_len = 0;
    lwm2m_object_t *pSrcSecurityObject = NULL;
    lwm2m_object_t *pDestSecurityObject = NULL;

    pSrcSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSrcSecurityObject != NULL), "get_security_object(...) failed");
    pDestSecurityObject = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    TEST_ASSERT_MSG((pDestSecurityObject != NULL), "lwm2m_malloc(...) failed");
    // p == NULL is not checked in function copy_security_object(lwm2m_object_t *, lwm2m_object_t *)
    uint8_t ret = 0;
    copy_security_object(pDestSecurityObject, pSrcSecurityObject);
    TEST_ASSERT_MSG((ret == 0), "copy_security_object(...) failed");

    clean_security_object(pDestSecurityObject);
    clean_security_object(pSrcSecurityObject);
}
void TestObjectSecurity::test_display_security_object(void)
{
    uint16_t serverId = 123;
    char *serverUri = (char *)"coap://192.168.1.106:5368";
    bool b_need_bootstrap_flag = true;
    char *psk_Id = NULL;
    char *psk = NULL;
    unsigned short psk_len = 0;
    lwm2m_object_t *pSecurityObject = NULL;

    // p == NULL is not checked in function display_security_object(lwm2m_object_t *)
    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject != NULL), "get_security_object(...) failed");
    uint8_t ret = 0;
    display_security_object(pSecurityObject);
    TEST_ASSERT_MSG((ret == 0), "display_security_object(...) failed");
    clean_security_object(pSecurityObject);
}

void TestObjectSecurity::test_clean_security_object(void)
{
    uint16_t serverId = 123;
    char *serverUri = (char *)"coap://192.168.1.106:5368";
    bool b_need_bootstrap_flag = true;
    char *psk_Id = NULL;
    char *psk = NULL;
    unsigned short psk_len = 0;
    lwm2m_object_t *pSecurityObject = NULL;

    stubInfo stub_info;
    setStub((void *)lwm2m_malloc, (void *)stub_lwm2m_malloc, &stub_info);
    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject == NULL), "get_security_object(...) failed");
    cleanStub(&stub_info);
    uint8_t ret = 0;
    clean_security_object(pSecurityObject);
    TEST_ASSERT_MSG((ret == 0), "clean_security_object(...) failed");

    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject != NULL), "get_security_object(...) failed");
    clean_security_object(pSecurityObject);
    TEST_ASSERT_MSG((ret == 0), "clean_security_object(...) failed");
}

void TestObjectSecurity::test_get_security_object(void)
{
    uint16_t serverId = 123;
    char *serverUri = (char *)"coap://192.168.1.106:5368";
    bool b_need_bootstrap_flag = true;
    char *psk_Id = NULL;
    char *psk = NULL;
    unsigned short psk_len = 0;
    lwm2m_object_t *pSecurityObject = NULL;

    stubInfo stub_info;
    setStub((void *)lwm2m_malloc, (void *)stub_lwm2m_malloc, &stub_info);
    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject == NULL), "get_security_object(...) failed");
    cleanStub(&stub_info);
    clean_security_object(pSecurityObject);

    setStub((void *)lwm2m_malloc, (void *)stub_lwm2m_malloc_ex, &stub_info);
    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject == NULL), "get_security_object(...) failed");
    cleanStub(&stub_info);

    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject != NULL), "get_security_object(...) failed");
    clean_security_object(pSecurityObject);

    // psk test
    unsigned char g_psk_value[16] = {0x02, 0x77, 0x68, 0xca, 0x0b, 0xf5, 0xdf, 0xba, 0x46, 0x43, 0x25, 0xdd, 0x4b, 0xe7, 0x0a, 0x9d};
    psk_Id = (char *)"88889999";
    psk = (char *)g_psk_value;
    psk_len = 16;
    serverUri = (char *)"coaps://192.168.1.106:5368";
    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject != NULL), "get_security_object(...) failed");
    clean_security_object(pSecurityObject);
}

void TestObjectSecurity::test_security_get_uri(void)
{
    uint16_t serverId = 123;
    char *serverUri = (char *)"coap://192.168.1.106:5368";
    bool b_need_bootstrap_flag = true;
    char *psk_Id = NULL;
    char *psk = NULL;
    unsigned short psk_len = 0;
    lwm2m_object_t *pSecurityObject = NULL;

    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject != NULL), "get_security_object(...) failed");

    char *uri = NULL;
    uri = security_get_uri(pSecurityObject, 0);
    TEST_ASSERT_MSG((uri != NULL), "security_get_uri(...) failed");
    if(uri != NULL)
    {
        lwm2m_free(uri);
    }

    clean_security_object(pSecurityObject);
}

void TestObjectSecurity::test_prv_security_read(void)
{
    uint16_t serverId = 123;
    char *serverUri = (char *)"coap://192.168.1.106:5368";
    bool b_need_bootstrap_flag = true;
    char *psk_Id = NULL;
    char *psk = NULL;
    unsigned short psk_len = 0;
    lwm2m_object_t *pSecurityObject = NULL;

    pSecurityObject = get_security_object(serverId, serverUri, psk_Id, psk, psk_len, b_need_bootstrap_flag);
    TEST_ASSERT_MSG((pSecurityObject != NULL), "get_security_object(...) failed");

    // read test
    int num = 0;
    lwm2m_data_t *pData = NULL;
    uint8_t ret = 0;
    ret = pSecurityObject->readFunc(0, &num, &pData, NULL, pSecurityObject);
    TEST_ASSERT_MSG((COAP_205_CONTENT == ret && pData != NULL), "prv_security_read(...) failed in object_security.c");
    if(pData != NULL)
    {
        lwm2m_data_free(num, pData);
    }

    clean_security_object(pSecurityObject);
}
/* Private functions --------------------------------------------------------*/

