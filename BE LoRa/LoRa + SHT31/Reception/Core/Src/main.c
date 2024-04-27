/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.²
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "lib_lcd.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define BUFFER_SIZE 100

// Variables globales
char buffer[BUFFER_SIZE];
int index = 0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static rgb_lcd lcdData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


char receivedData[1000]; // Pour stocker le caractère reçu
long int extractedValue;

// Fonction pour traiter les données reçues
long int processReceivedData(char inByte) {
    buffer[index] = inByte;
    index++;

    // Si on reçoit un retour à la ligne, on analyse la ligne
    if (inByte == '\n') {
        // On réinitialise l'index
        index = 0;

        // Si la ligne contient "RX", on la traite
        if (strstr(buffer, "+TEST: RX") != NULL) {
            // On recherche les positions de "AA"
            char *debut = strstr(buffer, "AA");
            char *fin = strstr(debut + 2, "AA");

            if (debut != NULL && fin != NULL) {
                // On extrait les données entre "AA" et "AA"
                char resultat[20];
                strncpy(resultat, debut + 2, fin - debut - 2);
                resultat[fin - debut - 2] = '\0';

                // Conversion de la chaîne de caractères en long int
                extractedValue = strtol(resultat, NULL, 10);

                return extractedValue;
            }
        }

        // On efface le buffer
        memset(buffer, 0, BUFFER_SIZE);
    }

    return 0; // Retourne 0 si aucune valeur n'a été extraite
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  lcd_init(&hi2c1, &lcdData); // initialise le lcd
  /* USER CODE BEGIN 2 */
/*
  // Transmit command to LoRa module
  	  HAL_UART_Transmit(&huart1, (uint8_t*)dataToSend, strlen(dataToSend), 1000);

     // Wait for response from LoRa module
     HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);

*/

  // Transmit command at to LoRa module
         char dataToSend[] = "AT\r"; // Tableau d'octets à envoyer

  	   	  HAL_UART_Transmit(&huart1, (uint8_t*)dataToSend, strlen(dataToSend), 1000);
  	  // Attente de la réponse à la première commande
  	     HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);

  	     // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	     HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);

  	     // Envoi de la deuxième commande "AT+ID"
  	     char secondCommand[] = "AT+ID\r";
  	     HAL_UART_Transmit(&huart1, (uint8_t*)secondCommand, strlen(secondCommand), 1000);

  	     // Attente de la réponse à la deuxième commande (si nécessaire)
  	     HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);
  	   // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	     HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);


  	   // Envoi de la 3ème commande "AT+MODE"
  	    char thirdCommand[] = "AT+MODE=TEST\r";
  	    HAL_UART_Transmit(&huart1, (uint8_t*)thirdCommand, strlen(thirdCommand), 1000);

  	    // Attente de la réponse à la 3ième commande (si nécessaire)
  	    HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);
  	    // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	    HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);


  	  // Envoi de la 4ème commande "AT+MODE"
  	    char fourthCommand[] = "AT+TEST=RXLRPKT\r";
  	    HAL_UART_Transmit(&huart1, (uint8_t*)fourthCommand, strlen(fourthCommand), 1000);

  	   // Attente de la réponse à la 3ième commande (si nécessaire)
  	    HAL_UART_Receive(&huart1, (uint8_t*)receivedData, sizeof(receivedData), 1000);
  	    // Transmission de la réponse reçue à une autre UART (si nécessaire)
  	    HAL_UART_Transmit(&huart2, (uint8_t*)receivedData, strlen(receivedData), 1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  // Réception des données caractère par caractère
	      uint8_t receivedData;
	      HAL_UART_Receive(&huart1, &receivedData, 1, HAL_MAX_DELAY);

	      long int valeur = processReceivedData((char)receivedData);

	     // long int temperature = valeur % 100;
	      int temperatureint = 0;
	      int temperaturedec = 0;
	      double temperature = 0;

	      int humidity = 0;

	      // Vérifie si la valeur extraite a été mise à jour
	      if (valeur != 0)
	      {

	          // Afficher la température
	          temperatureint = valeur / 10000; // Les deux premiers chiffres représentent la température
	           temperaturedec = (valeur/100) % 100 ;

	           temperature= temperatureint + ((double) temperaturedec)/100;


	          // Afficher l'humidité
	          humidity = valeur % 100; // Les deux derniers chiffres représentent l'humidité

	          // Affiche la valeur sur UART2
	          char bufferTemp[10];
	          char bufferTempint[10];

	          char bufferTempdec[10];
	          char bufferHum[10];

	          snprintf(bufferTempint, sizeof(bufferTempint), "%d", temperatureint);
	          snprintf(bufferTempdec, sizeof(bufferTempdec), "%d", temperaturedec);
	          snprintf(bufferTemp, sizeof(bufferTemp), "%.2lf", temperature);

	          snprintf(bufferHum, sizeof(bufferHum), "%d", humidity);


	          HAL_UART_Transmit(&huart2, (uint8_t *)"temperatureint: ", strlen("temperatureint: "), HAL_MAX_DELAY);
	          HAL_UART_Transmit(&huart2, (uint8_t *)bufferTempint, strlen(bufferTempint), HAL_MAX_DELAY);

	          HAL_UART_Transmit(&huart2, (uint8_t *)"temperaturedec: ", strlen("temperaturedec: "), HAL_MAX_DELAY);
	          HAL_UART_Transmit(&huart2, (uint8_t *)bufferTempdec, strlen(bufferTempdec), HAL_MAX_DELAY);


	          HAL_UART_Transmit(&huart2, (uint8_t *)"Température: ", strlen("Température: "), HAL_MAX_DELAY);
	          HAL_UART_Transmit(&huart2, (uint8_t *)bufferTemp, strlen(bufferTemp), HAL_MAX_DELAY);


	          HAL_UART_Transmit(&huart2, (uint8_t *)" Humidité: ", strlen(" Humidité: "), HAL_MAX_DELAY);
	          HAL_UART_Transmit(&huart2, (uint8_t *)bufferHum, strlen(bufferHum), HAL_MAX_DELAY);
	          HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

	          clearlcd();
	          	  sprintf(bufferHum,"Humidite: %d", humidity);
	          	  sprintf(bufferTemp, "Temp.: %.2f C", temperature);
	          	  lcd_position(&hi2c1,0,0);
	          	  lcd_print(&hi2c1,bufferHum);
	          	  lcd_print(&hi2c1,"%");
	          	  lcd_position(&hi2c1,0,1);
	          	  lcd_print(&hi2c1,bufferTemp);
	      }


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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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
