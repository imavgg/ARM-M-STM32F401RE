/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
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
#include <string.h>
//#define USERNAME "admin"
//#define PASSWORD "12345"

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;



/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
	
void UART_Transmit(const char *data) {
    HAL_UART_Transmit(&huart2, (uint8_t*)data, strlen(data), 1000);
}

void UART_Receive( char *buffer) {
	while(HAL_OK != HAL_UART_Receive(&huart2, (uint8_t*)buffer, sizeof(buffer),1000 ) );	//break until received data};
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, sizeof(buffer),1000);
}



char *usr="root";
char *pwd="1234";

int main(void)
{

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
	HAL_Delay(1000);
	
	uint8_t buf1[] = "Initializing UART ....\n";
	uint8_t buf2[] = "UART is initialized.\n\n\n "; //buffer size


	if ( HAL_UART_Transmit(&huart2, buf1, sizeof(buf1),1000) == HAL_OK) 
	{
		
		HAL_UART_Transmit(&huart2, buf2, sizeof(buf2),100); //UART Setup
		HAL_Delay(1000);

	}
	//initialize 
	char username[10]; // imcomplete error for char username[]; 
	char password[10];
  int loggedIn = 0; // initialized False 
	int usr_N = sizeof(*usr);//careful: sizeof(usr)= 4(pointer)
	int pwd_N = sizeof(*pwd);
	
	/* Forever Loop */
    while (1) {
        if (!loggedIn) {
					/* Begin: Loginning*/
					UART_Transmit("\n --------------------------------------------------\nPlease Log in your account. ");
					UART_Transmit("\n ID: ");
					memset(username,'$',sizeof(username));//reset
					UART_Receive(username); //+\n
            
					UART_Transmit("\n PWD: ");
					memset(password,'$',sizeof(password));
					UART_Receive(password);					
					
					UART_Transmit("\n Checking your identity.... ");
					HAL_Delay(2000);
					
					int checked_i=0;
					//check username
					for (int i = 0; i < usr_N; i++) 
					{
						if (username[i]==usr[i]  ){
								checked_i ++;			
							//check password
							for (int i = 0; i < pwd_N; i++) 
							{
								if (password[i]==pwd[i]  ){
										checked_i ++;								
								}else{
										break; //break the loop
								}						
							}								
						}else{
							break; //break the loop
						}						
					}	

					
					//checked i is verfied.
					if (checked_i == usr_N + pwd_N) {
							UART_Transmit("\nLogin Success!\n");
							uint8_t buf3[] = "\nType exit to log out!\n";
							HAL_UART_Transmit_IT(&huart2, buf3, sizeof(buf3) );
							loggedIn = 1;  
					}else{
							UART_Transmit("\nLogin Fail!\n");
					} 
					/* End: Logging*/
        } else {
						/* Begin: Logged in*/
					  char rec[4];
					  memset( rec, '$', sizeof(rec));
					
					/* Begin: Exit Use Interrupt method to check if received "exit" */
						UART_Receive(rec);						
							
							if(strcmp("exit", rec ) == 0 ){
									UART_Transmit("\n\nDo you want to exit (y/n): ");
									char choice[1];
									memset(choice,'$',sizeof(choice));//reset
									UART_Receive(choice);

									if (choice[0] == 'y' || choice[0] == 'Y') {
											UART_Transmit("Log out!\n");
											loggedIn = 0; 
									} else {
											UART_Transmit("Welcome Back!\n");
									}									
							}						
						
						/* End: Exit */					
					
        }
				/* End: Logged in*/
    }

    return 0;
}
/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart2)
{

}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
