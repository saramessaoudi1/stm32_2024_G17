//lora_sht31
/* USER CODE BEGIN Header */
/**
  **************************
  * @file           : main.c
  * @brief          : Main program body
  **************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.²
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "lib_lcd.h"

#define _OPEN_SYS_ITOA_EXT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//#define MODE_TRANSMITTER 0
//#define MODE_RECEIVER 1

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static const uint8_t CAPTEUR_ADRS = 0x44 << 1;
static const uint8_t CAPTEUR_CMD_MSB = 0x24;
static const uint8_t CAPTEUR_CMD_LSB = 0x16;
static rgb_lcd lcdData;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


char receivedData[1000]; // Pour stocker le caractère reçu


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	HAL_StatusTypeDef ret;
		uint8_t buf[12];
		volatile uint16_t valeur;
		volatile uint16_t valeur2;
		volatile float temp;
		float umid;
		float partieDecimal;
		float partieEntiere;
		float partieDecimal2;
		float partieEntiere2;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
 lcd_init(&hi2c1, &lcdData); // initialise le lcd
  lcd_position(&hi2c1,0,0);
  //lcd_print(&hi2c1,"mongo");
  reglagecouleur(0,0,255);


  // Transmit command at to LoRa module
         char dataToSend[] = "AT\r"; // Tableau d'octets à envoyer

  	   	  HAL_UART_Transmit(&huart1, (uint8_t*)dataToSend, strlen(dataToSend), 1000);
  	  // Attente de la réponse à la première commande
  	     HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);

  	     // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	     HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);
/*
  	     // Envoi de la deuxième commande "AT+ID"
  	     char secondCommand[] = "AT+ID\r";
  	     HAL_UART_Transmit(&huart1, (uint8_t*)secondCommand, strlen(secondCommand), 1000);

  	     // Attente de la réponse à la deuxième commande (si nécessaire)
  	     HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);
  	   // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	     HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);
*/

  	   // Envoi de la 3ème commande "AT+MODE"
  	    char thirdCommand[] = "AT+MODE=TEST\r";
  	    HAL_UART_Transmit(&huart1, (uint8_t*)thirdCommand, strlen(thirdCommand), 1000);

  	    // Attente de la réponse à la 3ième commande (si nécessaire)
  	    HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);
  	    // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	    HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  buf[0] = CAPTEUR_CMD_MSB;
	  		buf[1] = CAPTEUR_CMD_LSB;
	  		ret = HAL_I2C_Master_Transmit( &hi2c1, CAPTEUR_ADRS, buf, 2, HAL_MAX_DELAY);
	  		if ( ret != HAL_OK)
	  		{
	  			strcpy((char*)buf, "erreur_T!!\r\n");
	  		}
	  			else
	  		{

	  		ret = HAL_I2C_Master_Receive( &hi2c1, CAPTEUR_ADRS, buf, 6, HAL_MAX_DELAY);
	  					if ( ret != HAL_OK)
	  				{
	  					strcpy((char*)buf, "erreur_R!!\r\n");
	  				}
	  		else
	  		{

	  			valeur  =   buf[1] | buf[0] << 8;


	  			temp = -45 + 175 * ( (float)valeur / 65535);

	  			partieEntiere = (int) temp;
	  			partieDecimal = temp;
	  			partieDecimal *= 100;
	  			partieDecimal = partieDecimal - (partieEntiere * 100);

	  			valeur = buf[4] | buf[3] << 8;


	  			umid = 100*( (float)valeur / 65535);

	  			sprintf( (char*)buf, "%u.%u C ; %u  ", (unsigned int) partieEntiere,(unsigned int) partieDecimal,(unsigned int) umid  );
	  			lcd_position(&hi2c1,0,0);
	  			lcd_print(&hi2c1,"Temp :  ");
	  			lcd_position(&hi2c1,7,0);
	  			lcd_print(&hi2c1,buf);


	  			lcd_position(&hi2c1,0,1);
	  			lcd_print(&hi2c1,"Hum  : ");
	  			lcd_position(&hi2c1,7,1);

	  			lcd_print(&hi2c1,&buf[10]);
	  			lcd_print(&hi2c1,"%");









	  		}

	  		}

	  		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	  		HAL_Delay(1000);






	  // Envoi de la commande  AT+TEST=TXLRSTR (ascii) pour envoyer un message
	 //                        AT+TEST=TXLRPKT (hexa)
	    	  //char sendCommand[] = "AT+TEST=TXLRPKT AC1234 %s\r";
	  		/*************************************************************************/
	  		 // Conversion de la partie entière de la température en chaîne de caractères
	  		        char temperatureString[10]; // Assez grand pour stocker la température sous forme de chaîne
	  		      char UmidString[10]; // Assez grand pour stocker la umidité sous forme de chaîne
	  		        snprintf(temperatureString, sizeof(temperatureString), "%u%u", (unsigned int)partieEntiere,(unsigned int)partieDecimal);
	  		      snprintf(UmidString, sizeof(temperatureString), "%u",(unsigned int)umid);
	  		        // Construction de la commande à envoyer avec la température
	  		        char sendCommand[30]; // Assez grand pour contenir toute la commande
	  		        snprintf(sendCommand, sizeof(sendCommand), "AT+TEST=TXLRPKT AA%s%sAA \r", temperatureString,UmidString);



	  		/***************************************************************************/

	    	  HAL_UART_Transmit(&huart1, (uint8_t*)sendCommand, strlen(sendCommand), 1000);

	    	  // Attente de la réponse à la 5ème commande (si nécessaire)
	    	  HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);
	    	  // Transmission de la réponse reçue à une autre UART (si nécessaire)
	    	  HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);
	    //	  HAL_Delay(2000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  /*if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }*/

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
 /* RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;*/
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

 /* if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }*/
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
