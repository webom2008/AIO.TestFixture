/******************************************************************************

   Copyright (C), 2005-2015, CVTE.

 ******************************************************************************
  File Name     : driver_uart4.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/8
  Last Modified :
  Description   : uart4 driver file
  Function List :
  History       :
  1.Date        : 2015/9/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "defines.h"

#if defined(CONFIG_UART4_DMA_MODE) && defined(CONFIG_UART4_INT_MODE)
#Error "Only one of CONFIG_UART4_DMA_MODE and CONFIG_UART4_INT_MODE can be defined! "
#elif defined(CONFIG_UART4_DMA_MODE)
#include "driver_uart4_dma.c"
#elif defined(CONFIG_UART4_INT_MODE)
#include "driver_uart4_interrupt.c"
#else
#Error "must define one of CONFIG_UART4_DMA_MODE and CONFIG_UART4_INT_MODE! "
#endif

