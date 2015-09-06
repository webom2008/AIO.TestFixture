/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart1.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/2
  Last Modified :
  Description   : uart1 driver
  Function List :
  History       :
  1.Date        : 2015/9/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "defines.h"
#if defined(CONFIG_UART1_DMA_MODE) && defined(CONFIG_UART1_INT_MODE)
#Error "Only one of CONFIG_UART1_DMA_MODE and CONFIG_UART1_INT_MODE can be defined! "
#elif defined(CONFIG_UART1_DMA_MODE)
#include "driver_uart1_dma.c"
#elif defined(CONFIG_UART1_INT_MODE)
#include "driver_uart1_interrupt.c"
#else
#Error "must define one of CONFIG_UART1_DMA_MODE and CONFIG_UART1_INT_MODE! "
#endif
