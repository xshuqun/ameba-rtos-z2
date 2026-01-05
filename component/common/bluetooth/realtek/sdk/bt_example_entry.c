#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#include <platform_opts_bt.h>
#include <bt_example_entry.h>

#if CONFIG_BT

#if defined(CONFIG_PLATFORM_8710C) && defined(CONFIG_FTL_ENABLED)
#include <ftl_int.h>

const uint8_t ftl_phy_page_num = 3;	/* The number of physical map pages, default is 3: BT_FTL_BKUP_ADDR, BT_FTL_PHY_ADDR1, BT_FTL_PHY_ADDR0 */
const uint32_t ftl_phy_page_start_addr = BT_FTL_BKUP_ADDR;

void app_ftl_init(void)
{
	ftl_init(ftl_phy_page_start_addr, ftl_phy_page_num);
}
#endif /* CONFIG_PLATFORM_8710C && CONFIG_FTL_ENABLED */

#if (F_BT_LE_USE_RANDOM_ADDR==1)
#include "device_lock.h"
#include "ftl_app.h"
#include <flash_api.h>
#include <trace_app.h>

/**
 * @brief   Save static random address information into flash.
 * @param[in] p_addr Pointer to the buffer for saving data.
 * @retval 1 Save success.
 * @retval other Failed.
 */
uint32_t ble_app_save_static_random_address(T_APP_STATIC_RANDOM_ADDR *p_addr)
{
    uint32_t result;
    flash_t flash;
    APP_PRINT_INFO0("ble_app_save_static_random_address");
    device_mutex_lock(RT_DEV_LOCK_FLASH);
    result = (uint32_t)flash_stream_write(&flash, BT_STATIC_RANDOM_ADDR, sizeof(T_APP_STATIC_RANDOM_ADDR), (uint8_t *)p_addr);
    device_mutex_unlock(RT_DEV_LOCK_FLASH);
    return result;
}
/**
  * @brief  Load static random address information from flash.
  * @param[out]  p_addr Pointer to the buffer for loading data.
  * @retval 1 Load success.
  * @retval other Failed.
  */
uint32_t ble_app_load_static_random_address(T_APP_STATIC_RANDOM_ADDR *p_addr)
{
    uint32_t result;
    flash_t flash;
    device_mutex_lock(RT_DEV_LOCK_FLASH);
    result = (uint32_t)flash_stream_read(&flash, BT_STATIC_RANDOM_ADDR, sizeof(T_APP_STATIC_RANDOM_ADDR), (uint8_t *)p_addr);
    device_mutex_unlock(RT_DEV_LOCK_FLASH);
    APP_PRINT_INFO1("ble_app_load_static_random_address: result 0x%x", result);
    if (!result)
    {
        memset(p_addr, 0, sizeof(T_APP_STATIC_RANDOM_ADDR));
    }
    return result;
}
#endif /* F_BT_LE_USE_RANDOM_ADDR */

#endif /* CONFIG_BT */
