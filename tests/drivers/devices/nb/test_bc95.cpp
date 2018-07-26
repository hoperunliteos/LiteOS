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
#include "test_bc95.h"
/* Defines ------------------------------------------------------------------*/
#define TEST_STATE_OK   0
#define TEST_STATE_ERR  1
/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
char g_state = TEST_STATE_OK;
/* Private function prototypes ----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
extern "C"
{

    #include "atiny_socket.h"
    #include "atiny_adapter.h"
	#include "atadapter.h"
    #include "bc95.h"
	

    extern at_task at;

    // in atadapter.c
    extern int32_t at_cmd(int8_t * cmd, int32_t len, const char * suffix, char * rep_buf);
    extern int32_t at_write(int8_t * cmd, int8_t * suffix, int8_t * buf, int32_t len);
    extern int32_t at_get_unuse_linkid();
	extern void at_init();
	extern void at_deinit();

    // in liteos queue
    extern UINT32 LOS_QueueCreate(CHAR *pcQueueName, UINT16 usLen, UINT32 *puwQueueID,
                            UINT32 uwFlags,UINT16 usMaxMsgSize );
    extern UINT32 LOS_QueueWriteCopy( UINT32 uwQueueID, VOID * pBufferAddr,
                            UINT32 uwBufferSize, UINT32 uwTimeOut );
    extern UINT32 LOS_QueueReadCopy(UINT32  uwQueueID, VOID *  pBufferAddr,
                            UINT32 * puwBufferSize, UINT32  uwTimeOut);
    // in atadapter.c
    extern void* at_malloc(size_t size);
    
    // stubs
    UINT32 stub_LOS_QueueCreate_fail(CHAR *pcQueueName, UINT16 usLen, UINT32 *puwQueueID,
                            UINT32 uwFlags,UINT16 usMaxMsgSize )
    {
		return 1;
    }
	
	char *g_buf = "\r1 \"abc\"2 2 \"def\" 22 \r \"hh\"";
	uint32_t g_len  = sizeof(g_buf);
	UINT32 stub_LOS_QueueReadCopy(UINT32  uwQueueID, VOID *  pBufferAddr,
                            UINT32 * puwBufferSize, UINT32  uwTimeOut)
    {
		QUEUE_BUFF	*qbuf = (QUEUE_BUFF *)pBufferAddr;
		qbuf->len  = g_len;
		qbuf->addr = (uint8_t *)at_malloc(g_len);
		if(qbuf->addr != NULL){
		    memcpy(qbuf->addr,(uint8_t *)g_buf,g_len);
		    return 0;
		}
		else
		{
			return 1;
		}
		
    }
	
	UINT32 stub_LOS_QueueReadCopy_fail(UINT32  uwQueueID, VOID *  pBufferAddr,
                            UINT32 * puwBufferSize, UINT32  uwTimeOut)
    {
		return 1;
    }
	
	int32_t stub_nb_create_udpsock_max(const int8_t * host, int port, int32_t proto)
	{
		return MAX_SOCK_NUM;
	}

    static int32_t stub_at_cmd(int8_t * cmd, int32_t len, const char * suffix, char * rep_buf)
    {
        char *tmp = NULL;

        if(g_state == TEST_STATE_ERR){
            return AT_FAILED;
        }
        if(rep_buf != NULL)
			memcpy(rep_buf,"SENT=2,\"1 2 3\"",sizeof("SENT=2,\"1 2 3\""));
        return AT_OK;
    }
	
	static int32_t stub_at_cmd2(int8_t * cmd, int32_t len, const char * suffix, char * rep_buf)
    {
        char *tmp = NULL;
        printf("come into stub_at_cmd2, g_state is %d, rep_buf is %p\n",g_state,rep_buf);
        if(g_state == TEST_STATE_ERR){
            return AT_FAILED;
        }
        if(rep_buf != NULL)
        {
			memcpy(rep_buf,"1\r 1 2 3 OK",sizeof("1\r 1 2 3 OK"));
        }
        return AT_OK;
    }
	
    static int32_t stub_at_cmd3(int8_t * cmd, int32_t len, const char * suffix, char * rep_buf)
    {
        char *tmp = NULL;

        if(g_state == TEST_STATE_ERR){
            return AT_FAILED;
        }
        if(rep_buf != NULL)
			memcpy(rep_buf,"-1\r 1 2 3 OK",sizeof("-1\r 1 2 3 OK"));
        return AT_OK;
    }
	
    static int32_t stub_at_write(int8_t * cmd, int8_t * suffix, int8_t * buf, int32_t len)
    {
        return AT_FAILED;
    }

    static int32_t stub_at_get_unuse_linkid()
    {
        return -1;
    }

    static UINT32 stub_LOS_QueueCreate(CHAR *pcQueueName, UINT16 usLen, UINT32 *puwQueueID,
            UINT32 uwFlags,UINT16 usMaxMsgSize )
    {
        return 1;
    }

    static UINT32 stub_LOS_QueueWriteCopy( UINT32 uwQueueID, VOID * pBufferAddr,
                     UINT32 uwBufferSize, UINT32 uwTimeOut )
    {
        if(g_state == TEST_STATE_ERR){
            return 1;
        }
        // simulate memory release
        QUEUE_BUFF *pqbuf = (QUEUE_BUFF *)pBufferAddr;
        if(pqbuf && pqbuf->addr){
            at_free(pqbuf->addr);
        }
        return 0;
    }

    static void* stub_at_malloc(size_t size)
    {
        return NULL;
    }
#if 0
    static UINT32 stub_LOS_QueueReadCopy(UINT32  uwQueueID, VOID *  pBufferAddr,
                    UINT32 * puwBufferSize, UINT32  uwTimeOut)
    {
        
        QUEUE_BUFF qbuf = {0, NULL};
        char buf[] = { 0x1,0x2,0x3,0x4,0x5 };

        if(g_state == TEST_STATE_ERR){
            return 1;
        }
        qbuf.addr = (uint8_t*)atiny_malloc(sizeof(buf));
        if (NULL == qbuf.addr)
        {
            printf("malloc for qbuf failed!");
            goto END;
        }

        qbuf.len = sizeof(buf);
        memcpy(qbuf.addr, buf, sizeof(buf));
        memcpy(pBufferAddr, &qbuf, sizeof(QUEUE_BUFF));
        *puwBufferSize = qbuf.len;
        
        END:
        return 0;
    }
#endif
}

TestBC95::TestBC95()
{
    at_init();
    TEST_ADD(TestBC95::test_str_to_hex);
    TEST_ADD(TestBC95::test_nb_reboot);
    TEST_ADD(TestBC95::test_nb_hw_detect);
    TEST_ADD(TestBC95::test_nb_err_cue);
    TEST_ADD(TestBC95::test_nb_set_cdpserver);
    TEST_ADD(TestBC95::test_nb_send_psk);
    TEST_ADD(TestBC95::test_nb_send_payload);
    TEST_ADD(TestBC95::test_nb_get_auto_connect);
    TEST_ADD(TestBC95::test_nb_check_csq);
    TEST_ADD(TestBC95::test_nb_query_ip);
    TEST_ADD(TestBC95::test_nb_get_netstat);
    TEST_ADD(TestBC95::test_nb_create_udpsock);
	
	TEST_ADD(TestBC95::test_nb_connect);
	TEST_ADD(TestBC95::test_nb_send);
	TEST_ADD(TestBC95::test_nb_recv);
	TEST_ADD(TestBC95::test_nb_recv_timeout);
	TEST_ADD(TestBC95::test_nb_close);
	TEST_ADD(TestBC95::test_nb_recv_cb);
	TEST_ADD(TestBC95::test_nb_deinit);
}

TestBC95::~TestBC95()
{
    at_deinit();
}

void TestBC95::test_str_to_hex(void)
{
    int32_t ret = 0;
	
	char bufin[3] = "22";
	char bufout[10];
    
    ret = str_to_hex(NULL,0,NULL);
    TEST_ASSERT_MSG((ret == -1), "test_str_to_hex null failed");
    ret = str_to_hex(bufin,2,bufout);
    TEST_ASSERT_MSG((ret == AT_OK), "test_str_to_hex normal failed");
    
}
void TestBC95::test_nb_reboot(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_reboot();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_reboot failed");
	
}
void TestBC95::test_nb_hw_detect(void)
{
    int32_t ret = AT_OK;
	
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_hw_detect();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_hw_detect failed");
}

void TestBC95::test_nb_err_cue(void)
{
    int32_t ret = AT_OK;
	
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_err_cue();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_err_cue failed");
}
void TestBC95::test_nb_set_cdpserver(void)
{
    int32_t ret = AT_OK;

    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	g_state = TEST_STATE_ERR;
	ret = nb_set_cdpserver("10.0.0.1","5683");
	TEST_ASSERT_MSG((ret == AT_FAILED), "test_nb_set_cdpserver failed");
	g_state = TEST_STATE_OK;
	ret = nb_set_cdpserver("10.0.0.1","5683");
	TEST_ASSERT_MSG((ret == AT_OK), "test_nb_set_cdpserver failed");
	cleanStub(&stub_info);
    
}
void TestBC95::test_nb_send_psk(void)
{
	int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_send_psk("123456","654321");
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_send_psk failed");
}
void TestBC95::test_nb_send_payload(void)
{
    int32_t ret = AT_OK;
	char buf[5]={'1','2','3','4'};
	g_state = TEST_STATE_ERR;
	stubInfo stub_info;
    setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_send_payload(buf,4);
	printf("ret is %d in test_nb_send_payload\n",ret);
	TEST_ASSERT_MSG((ret == -1), "test_nb_send_payload failed");
	g_state = TEST_STATE_OK;
	ret = nb_send_payload(buf,4);
	printf("ret is %d in test_nb_send_payload\n",ret);
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_send_payload failed");
	
}
void TestBC95::test_nb_get_auto_connect(void)
{
	int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_get_auto_connect();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_get_auto_connect failed");
}
void TestBC95::test_nb_check_csq(void)
{
	int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_check_csq();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_check_csq failed");
}
void TestBC95::test_nb_query_ip(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_query_ip();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_query_ip failed");
	
}
void TestBC95::test_nb_get_netstat(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_get_netstat();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == AT_OK), "test_nb_get_netstat failed");
}
void TestBC95::test_nb_create_udpsock(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd2,&stub_info);
	ret = nb_create_udpsock((int8_t *)"192.168.1.116",5683,1);
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret >= 0), "test_nb_create_udpsock failed");
	
	setStub((void *)at_cmd,(void *)stub_at_cmd3,&stub_info);
	ret = nb_create_udpsock((int8_t *)"192.168.1.116",5683,1);
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == -1), "test_nb_create_udpsock failed");
	
}
void TestBC95::test_nb_connect(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	stubInfo stub_info2;
	stubInfo stub_info3;
	
	printf("come into test_nb_connect \n");
	setStub((void *)at_cmd,(void *)stub_at_cmd2,&stub_info);
	ret = nb_connect((int8_t *)"192.168.1.116",(int8_t *)"5683",1);
	TEST_ASSERT_MSG((ret >= 0), "test_nb_connect failed");
	setStub((void *)nb_create_udpsock,(void *)stub_nb_create_udpsock_max,&stub_info3);
	ret = nb_connect((int8_t *)"192.168.1.116",(int8_t *)"5683",1);
	TEST_ASSERT_MSG((ret == -1), "test_nb_connect failed");
	cleanStub(&stub_info3);
	setStub((void *)LOS_QueueCreate,(void *)stub_LOS_QueueCreate_fail,&stub_info2);
	ret = nb_connect((int8_t *)"192.168.1.116",(int8_t *)"5683",1);
	TEST_ASSERT_MSG((ret == -1), "test_nb_connect failed");
	
	cleanStub(&stub_info2);
	cleanStub(&stub_info);
	
	
	printf("exit test_nb_connect\n");
	
	
	
}
void TestBC95::test_nb_send(void)
{
    int32_t ret = AT_OK;
	uint8_t buf[5]={0,1,2,3};
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_send(0,buf,4);
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret >= 0), "test_nb_send failed");
}
void TestBC95::test_nb_recv(void)
{
	int32_t ret = AT_OK;
	uint8_t buf[100];
	printf("come into test_nb_recv\n");
    stubInfo stub_info;
	setStub((void *)LOS_QueueReadCopy,(void *)stub_LOS_QueueReadCopy,&stub_info);
	ret = nb_recv(0,buf,100);
	TEST_ASSERT_MSG((ret >= 0), "test_nb_recv failed");
	cleanStub(&stub_info);
	
	setStub((void *)LOS_QueueReadCopy,(void *)stub_LOS_QueueReadCopy_fail,&stub_info);
	ret = nb_recv(0,buf,100);
	TEST_ASSERT_MSG((ret == -1), "test_nb_recv failed");
	cleanStub(&stub_info);
    
	
    
}
void TestBC95::test_nb_recv_timeout(void)
{
    int32_t ret = AT_OK;
	uint8_t buf[100];
	
    stubInfo stub_info;
	setStub((void *)LOS_QueueReadCopy,(void *)stub_LOS_QueueReadCopy,&stub_info);
	ret = nb_recv_timeout(0,buf,100,5);
	TEST_ASSERT_MSG((ret >= 0), "test_nb_recv_timeout failed");
	cleanStub(&stub_info);
	
	setStub((void *)LOS_QueueReadCopy,(void *)stub_LOS_QueueReadCopy_fail,&stub_info);
	ret = nb_recv_timeout(0,buf,100,5);
	TEST_ASSERT_MSG((ret == -1), "test_nb_recv_timeout failed");
	cleanStub(&stub_info);
}

void TestBC95::test_nb_close(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_close(1);
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == 0), "nb_close failed");
}

void TestBC95::test_nb_recv_cb(void)
{
    int32_t ret = AT_OK;
	
	
	
	ret = nb_recv_cb(1);
	TEST_ASSERT_MSG((ret == -1), "test_nb_recv_cb failed");
	
}

void TestBC95::test_nb_deinit(void)
{
    int32_t ret = AT_OK;
    stubInfo stub_info;
	setStub((void *)at_cmd,(void *)stub_at_cmd,&stub_info);
	ret = nb_deinit();
	cleanStub(&stub_info);
    TEST_ASSERT_MSG((ret == 0), "test_nb_deinit failed");
}




/* Private functions --------------------------------------------------------*/

