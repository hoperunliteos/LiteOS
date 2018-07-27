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

#ifndef TEST_OTA_DEFAULT_H_
#define TEST_OTA_DEFAULT_H_

#include <cpptest.h>
#include "stub.h"

extern int test_hal_spi_flash_read(void* buf, int32_t len, uint32_t location);
extern int test_hal_spi_flash_erase_write(const void* buf, int32_t len, uint32_t location);

class TestOtaDefault:public Test::Suite {
public:
    void test_ota_default_init();
    void test_ota_default_set_reboot();
    void test_ota_default_check_update_state();
    
//  USE_BOOTLOADER
    void test_ota_default_update_process();
    void test_ota_default_jump_to_application();
    void test_ota_default_roll_back_image();
    void test_prv_get_update_record();

//  test ota.c
    void test_ota_register_module();
    void test_ota_register_assist();
    void test_ota_init();
    void test_ota_update_process();
    void test_ota_jump_to_application();
    void test_ota_roll_back_image();
//  USE_BOOTLOADER NO!
//    void test_ota_set_reboot();
//    void test_ota_check_update_state()

    // test ota_crc.c
//    void test_calc_crc32();


    TestOtaDefault();
    ~TestOtaDefault();
    
protected:
    void tear_down();
    void setup();
};


#endif

