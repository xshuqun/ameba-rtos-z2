#ifndef _BT_EXAMPLE_ENTRY_H_
#define _BT_EXAMPLE_ENTRY_H_

#include <app_common_flags.h>

#if defined(CONFIG_PLATFORM_8710C) && defined(CONFIG_FTL_ENABLED)
void app_ftl_init(void);
#endif

#if (F_BT_LE_USE_RANDOM_ADDR==1)
#include <gap.h>
typedef struct
{
	uint8_t 	 is_exist;
	uint8_t 	 reserved;		   /**< remote BD type*/
	uint8_t 	 bd_addr[GAP_BD_ADDR_LEN];	/**< remote BD */
} T_APP_STATIC_RANDOM_ADDR;

/**
 * @brief   Save static random address information into flash.
 * @param[in] p_addr Pointer to the buffer for saving data.
 * @retval 0 Save success.
 * @retval other Failed.
 */
uint32_t ble_app_save_static_random_address(T_APP_STATIC_RANDOM_ADDR *p_addr);

/**
  * @brief  Load static random address information from flash.
  * @param[out]  p_addr Pointer to the buffer for loading data.
  * @retval 0 Load success.
  * @retval other Failed.
  */
uint32_t ble_app_load_static_random_address(T_APP_STATIC_RANDOM_ADDR *p_addr);
#endif /* F_BT_LE_USE_RANDOM_ADDR */

#endif //_BT_EXAMPLE_ENTRY_H_
