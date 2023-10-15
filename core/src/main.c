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
#include "nn.h"
#include "model.h"


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
int input_size = 57;
int hidden_size = 8;
int output_size = 18;

const char *categories[] = {
  "Arabic", "Chinese", "Czech", "Dutch", "English", "French", "German", 
        "Greek", "Irish", "Italian", "Japanese", "Korean", "Polish", "Portuguese", 
        "Russian", "Scottish", "Spanish", "Vietnamese"
};


int len_mapping = 57;
char letter_mapping[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' ','.',',',';','\''};


void encodeOneHot(Matrix *input, char c) {
  memset(input->data, 0, input->rows * input->cols * sizeof(float));
  for (int i=0; i<len_mapping; i+=1) {
    if (letter_mapping[i] == c) {
      input->data[i] = 1;
      return;
    }
  }
}

char *input_strs[] = {
  "sakura",
  "Vandroogenbroeck",
  "Zhong-Wen",
};
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


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    printf("================================\n");
    printf("initializing matrix...\n");
    Matrix output;
    NN_initMatrix(&output, 1, output_size);
    
    Matrix input;
    NN_initMatrix(&input, 1, input_size + hidden_size);
    
    Matrix hidden;
    NN_initMatrix(&hidden, 1, hidden_size);

    int index;
    for (int i=0; i<3; i+=1) {
      char *str = input_strs[i];

      memset(hidden.data, 0, hidden.rows * hidden.cols * sizeof(float));
      
      for (int j=1; j<strlen(str); j+=1) {
        encodeOneHot(&input, str[j]);
        NN_linear(&hidden, &i2h_weight_transposed, &i2h_bias, &input);

        forward(&output, &hidden, &input);
      }
      
      // printMatrix(&output);
      index = argmax(&output);
      
      printf("entered: %s\n", str);
      printf("predicted: (%d, %s), \tscore: ", index, categories[index]);
      printDouble(output.data[index], 2);
      printf("\n");
      printf("\n");
    }
        
        
    NN_deinitMatrix(&output);
    NN_deinitMatrix(&input);
    NN_deinitMatrix(&hidden);

    HAL_delay(1000);
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
