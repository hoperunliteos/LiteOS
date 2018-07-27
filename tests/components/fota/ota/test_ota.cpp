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

#include "test_ota.h"
#include "ota.h"
#include "string.h"
#include "ota_default.h"

static ota_default_flag test_g_ota_flag;
static int flag;

static int test_hal_spi_flash_read(void* buf, int32_t len, uint32_t location)
{  
    switch (flag)
    {
    case 0:     break;
    case 1:
        memcpy(buf, &test_g_ota_flag, len);
        buf = (void *)(&test_g_ota_flag);
    
        break;
    case -1:
        return -1;
    default:    break;
    }
    return 0;
}

static int test_hal_spi_flash_erase_write(const void* buf, int32_t len, uint32_t location)
{
    switch (flag)
    {
    case 0:     break;
    case -1:    return -1;
    default:    break;
    }
    return 0;
}

void TestOta::test_ota_register_assist()
{
    ota_assist assist;
    assist.func_printf = printf;
    assist.func_ota_read = test_hal_spi_flash_read;
    assist.func_ota_write = test_hal_spi_flash_erase_write;
    ota_register_assist(&assist);
}

void TestOta::test_ota_init()
{
    int ret = ota_init();
    TEST_ASSERT_MSG((OTA_ERRNO_OK == ret),"ota_init() is failed");
}

TestOta::TestOta()
{
    TEST_ADD(TestOta::test_ota_register_assist);
    TEST_ADD(TestOta::test_ota_init);
}

TestOta::~TestOta()
{
}

void TestOta::setup()
{
    std::cout << "setup";
}

void TestOta::tear_down()
{
    std::cout << "test_down";
}


