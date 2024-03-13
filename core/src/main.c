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
// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "systolic.h"
// #include "util.h"

#define N (2)

void operands(int c, int * a, int * b, int * d) {
  *d = c % N;
  *b = (c / N) % N;
  *a = c / (N*N);
}

#if 3*N*DIM > (BANK_NUM * BANK_ROWS) || N*N*N*DIM > ACC_ROWS
//#error scratchpad or accumulator not big enough
#endif

#define PRINT_LOG 1


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

    // HART_DATA->hart_0 = IDLE;
    // HART_DATA->hart_1 = IDLE;
    // HART_DATA->hart_2 = IDLE;
    // HART_DATA->hart_3 = IDLE;
    
    // boot 0-1
    // RCC->CLUSTER_CLKSEL = 0b000100100100100100100100100100101;
    // RCC->CLUSTER_RESETS = 0x000003FE;

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
    static elem_t ZERO[DIM][DIM];


    printf("performing 2 C = AB + D operations with CPU and Gemmini...\n");

    uint64_t t = CLINT->MTIME;
    uint64_t cpu_time = 0;
    uint64_t gemmini_time = 0;

    for (int activation = 0; activation <= 2; ++activation) {
      for (int shift = 0; shift <= 12; shift += 4) {
        static elem_t A[N][DIM][DIM] row_align(1);
        static elem_t B[N][DIM][DIM] row_align(1);
        static elem_t D[N][DIM][DIM] row_align(1);

        // We will try out every combination of A, B, D possible
        static elem_t C[N*N*N][DIM][DIM] row_align(1);
        static int64_t gold_full[N*N*N][DIM][DIM];
        static elem_t gold[N*N*N][DIM][DIM];

        int relu6_shift = shift+1;

        // ...taking into account whether we preload new weights or re-use the old ones
        static int preload[N*N*N] = {1};
        for (int i = 1; i < N*N*N; ++i)
          preload[i] = rand() % 2;

        // ...whether we pass in a D or just use zeros
        static int add_to_zeros[N*N*N];
        for (int i = 0; i < N*N*N; ++i)
          add_to_zeros[i] = rand() % 2;

        // ...and whether we accumulate on top of the previous result
        static int accumulate[N*N*N] = {0};
        for (int i = 1; i < N*N*N; ++i)
          accumulate[i] = rand() % 2;

        static int no_output[N*N*N];
        for (int i = 0; i < N*N*N-1; ++i)
          no_output[i] = accumulate[i+1];
        no_output[N*N*N-1] = 0;

        // Print the sequence out
        #if PRINT_LOG
        printf("Preloads: ");
        for (int i = 0; i < N*N*N; ++i)
          printf("%d, ", preload[i]);
        printf("\n");
        printf("Zeros: ");
        for (int i = 0; i < N*N*N; ++i)
          printf("%d, ", add_to_zeros[i]);
        printf("\n");
        printf("Accumulates: ");
        for (int i = 0; i < N*N*N; ++i)
          printf("%d, ", accumulate[i]);
        printf("\n");
        printf("No outputs: ");
        for (int i = 0; i < N*N*N; ++i)
          printf("%d, ", no_output[i]);
        printf("\n");
        #endif

        for (size_t n = 0; n < N; ++n) {
          for (size_t i = 0; i < DIM; ++i) {
            for (size_t j = 0; j < DIM; ++j) {
              A[n][i][j] = (rand() % 64) - 32;
              B[n][i][j] = (rand() % 64) - 32;
              D[n][i][j] = (rand() % 64) - 32;
            }
          }
        }
        t = CLINT->MTIME;
        for (size_t g = 0; g < N*N*N; ++g) {
          int a, b, d;
          operands(g, &a, &b, &d);

          // We need to find the last B value in case we aren't preloading new weights
          for (int last_g = g; last_g >= 0; --last_g) {
              int tmp_a, tmp_d;
              if (preload[last_g]) {
                  operands(last_g, &tmp_a, &b, &tmp_d);
                  break;
              }
          }

          if (add_to_zeros[g])
            matmul(A[a], B[b], ZERO, gold_full[g]);
          else
            matmul(A[a], B[b], D[d], gold_full[g]);

          if (accumulate[g])
            matadd(gold_full[g], gold_full[g-1], gold_full[g]);
        }

        for (size_t g = 0; g < N*N*N; ++g) {
          matshift(gold_full[g], gold[g], shift);
          if (activation == RELU)
            matrelu(gold[g], gold[g]);
          else if (activation == RELU6)
            matrelu6(gold[g], gold[g], 1 << relu6_shift);
        }
        cpu_time += CLINT->MTIME - t;

        int A_addr = 0;
        int B_addr = N*DIM;
        int D_addr = 2*N*DIM;
        int C_addr = 3*N*DIM;
        uint32_t C_addr_acc = 1 << (ADDR_LEN-1);

        t = CLINT->MTIME;
        
        // Calculate the proper destination addresses of everything
        int C_addrs[N*N*N];
        for (size_t c = 0; c < N*N*N; ++c)
          C_addrs[c] = C_addr_acc + c*DIM;
        for (size_t c = 0; c < N*N*N; ++c) {
          int last_c;
          for (last_c = c; last_c >= 0; --last_c)
            if (!accumulate[last_c])
              break;
          if (c != last_c)
            C_addrs[c] = C_addrs[last_c] | (1 << (ADDR_LEN-2));
        }

        #if PRINT_LOG
        printf("Moving in\n");
        #endif
        for (size_t n = 0; n < N; ++n)
          matmul_mvin(A[n], A_addr + n*DIM, 0, 0, 0, 0);

        for (size_t n = 0; n < N; ++n)
          matmul_mvin(B[n], B_addr + n*DIM, 0, 0, 0, 0);

        for (size_t n = 0; n < N; ++n)
          if (n == N-1) {
            matmul_mvin(D[n], D_addr + n*DIM, 0, 0, 1, 0);
          } else {
            matmul_mvin(D[n], D_addr + n*DIM, 0, 0, 0, 0);
          }

        #if PRINT_LOG
        printf("Setting mode\n");
        #endif
        matmul_config_ex(WEIGHT_STATIONARY, activation, shift, relu6_shift, 0, 1, 0, 0);

        #if PRINT_LOG
        printf("Matmulling\n");
        #endif
        for (size_t c = 0; c < N*N*N; ++c) {
          int a, b, d;
          operands(c, &a, &b, &d);

          uint64_t d_addr = D_addr + d*DIM;
          if (add_to_zeros[c])
            d_addr = GARBAGE_ADDR;

          if (!preload[c]) {
            if (c == N*N*N-1) {
              matmul_preload_zeros(C_addrs[c], 0, 0, 1, 0);
            } else {
              matmul_preload_zeros(C_addrs[c], 0, 0, 0, 0);
            }
            matmul_compute_accumulated(A_addr + a*DIM, d_addr);
          } else {
            if (c == N*N*N-1) {
              matmul_preload(B_addr + b*DIM, C_addrs[c], 0, 0, 1, 0);
            } else {
              matmul_preload(B_addr + b*DIM, C_addrs[c], 0, 0, 0, 0);
            }
            matmul_compute_preloaded(A_addr + a*DIM, d_addr);
          }
        }

        #if PRINT_LOG
        printf("Moving out\n");
        #endif
        int first_store = 1;
        for (size_t c = 0; c < N*N*N; ++c)
          if (!no_output[c])
            if (first_store) {
              matmul_mvout(C[c], C_addrs[c] & ~(1 << (ADDR_LEN-2)), 0, 0, 0, 1);
              first_store = 0;
            } else {
              matmul_mvout(C[c], C_addrs[c] & ~(1 << (ADDR_LEN-2)), 0, 0, 0, 0);
            }

        matmul_fence();

        #if PRINT_LOG
        printf("Moved out\n");
        for (int n = 0; n < N*N*N; ++n) {
          if (!no_output[n]) {
            printf("C:\n");
            printMatrix(C[n]);
            printf("Gold:\n");
            printMatrix(gold[n]);
            printf("\n");
          }
        }
        #endif
        gemmini_time += CLINT->MTIME - t;


        for (int n = 0; n < N*N*N; ++n)
          if (!no_output[n] && !is_equal(C[n], gold[n])) {
            printf("FAIL! activation: %d, shift: %d\n", activation, shift);
          }
          else {
            #if PRINT_LOG
            printf("PASSED! activation: %d, shift: %d\n", activation, shift);
            #endif
          }
      }
    }
    printf("cpu time taken: %u ms,\t gemmini time taken: %u ms\n", cpu_time / MTIME_FREQ, gemmini_time / MTIME_FREQ);
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
