/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define HART_DATA_BASE 0x5000FFF0

typedef struct {
  __IO uint32_t hart_0;
  __IO uint32_t hart_1;
  __IO uint32_t hart_2;
  __IO uint32_t hart_3;
} HART_DATA_TypeDef;

enum {
  IDLE = 0,
  DISPATCHED = 1,
  RUNNING = 2,
  DONE = 6,
};

#define HART_DATA ((HART_DATA_TypeDef *) HART_DATA_BASE)

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* USER CODE BEGIN 1 */
  uint8_t counter = 0;
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  // set up UART registers
  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  HAL_UART_init(UART0, &UART_init_config);

  uint8_t mhartid = READ_CSR("mhartid");

  if (mhartid == 20) {
    printf("hart 20 booting up others...\n");

    HART_DATA->hart_0 = IDLE;
    HART_DATA->hart_1 = IDLE;
    HART_DATA->hart_2 = IDLE;
    HART_DATA->hart_3 = IDLE;
    
    // boot 0-1
    RCC->CLUSTER_CLKSEL = 0b000100100100100100100100100100101;
    RCC->CLUSTER_RESETS = 0x000003FE;

    // boot all
    // RCC->CLUSTER_CLKSEL = 0b000101101101101101101101101101101;
    // RCC->CLUSTER_RESETS = 0x00000000;
  }
  else {
    while (1) {
      printf("i shouldnt be here :((\n");
    }
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (HART_DATA->hart_0 == IDLE) {
      HART_DATA->hart_0 = DISPATCHED;
      printf("hart 0 is running\n");
    }
    if (HART_DATA->hart_0 == DONE) {
      printf("hart 0 is done\n");
      HART_DATA->hart_0 = IDLE;
    }
    if (HART_DATA->hart_1 == IDLE) {
      HART_DATA->hart_1 = DISPATCHED;
      printf("hart 1 is running\n");
    }
    if (HART_DATA->hart_1 == DONE) {
      printf("hart 1 is done\n");
      HART_DATA->hart_1 = IDLE;
    }
    printf("hart status: (%d, %d, %d, %d)\n", HART_DATA->hart_0, HART_DATA->hart_1, HART_DATA->hart_2, HART_DATA->hart_3);
    counter += 1;
    
    HAL_delay(200);
    /* USER CODE END WHILE */
  }
  /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {
  while (1) {
    uint8_t mhartid = READ_CSR("mhartid");
    if (mhartid == 0) {
      while (HART_DATA->hart_0 != DISPATCHED) {
        // wait for hart 0 to boot
        HAL_delay(100);
      }
      HART_DATA->hart_0 = RUNNING;
      HAL_delay(1000);
      HART_DATA->hart_0 = DONE;
    }
    if (mhartid == 1) {
      while (HART_DATA->hart_1 != DISPATCHED) {
        // wait for hart 0 to boot
        HAL_delay(100);
      }
      HART_DATA->hart_1 = RUNNING;
      HAL_delay(1000);
      HART_DATA->hart_1 = DONE;
    }
  }
}
