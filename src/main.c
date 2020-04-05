#include "main.h"

#include "adc.h"
#include "dma.h"
#include "tim.h"

TIM_HandleTypeDef TIMn;
ADC_HandleTypeDef ADCn;
DMA_HandleTypeDef DMAn;

void SystemClock_Config(void);
void MX_GPIO_Init();

float mapRange(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
    float slope = (dstMax - dstMin) / (srcMax - srcMin);
    return dstMin + slope * (value - srcMin);
}

volatile uint16_t adc_values[4] = {0};

int main() {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init(&ADCn);
    MX_TIM2_Init(&TIMn);

    HAL_TIM_Base_Start_IT(&TIMn);
    HAL_TIM_PWM_Start_IT(&TIMn, TIM_CHANNEL_1);

    HAL_ADCEx_Calibration_Start(&ADCn);
    HAL_ADC_Start_DMA(&ADCn, (uint32_t*) &adc_values, sizeof(adc_values) / sizeof(*(adc_values)));

    while (1);
}

void DMA1_Channel1_IRQHandler(void) {// REQUIRED for success use of HAL_ADC_Start_DMA
    HAL_DMA_IRQHandler(&DMAn);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* adc) {
    if (adc->Instance == ADC1) {
        HAL_ADC_Stop_DMA(adc);

        TIMn.Instance->CCR1 = (uint16_t) mapRange((float) (adc_values[0] << 4), 0, UINT16_MAX, 1000, 19000);

        // Reset results
        adc_values[0] = 0;
        adc_values[1] = 0;
        adc_values[2] = 0;
        adc_values[3] = 0;

        HAL_ADCEx_Calibration_Start(&ADCn);
        HAL_ADC_Start_DMA(adc, (uint32_t*) &adc_values, sizeof(adc_values) / sizeof(*(adc_values)));
    }
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIMn);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *tim) {
    if (tim->Instance == TIM2) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *tim) {
    if (tim->Instance == TIM2) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    }
}

void MX_GPIO_Init() {
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO clock enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* GPIO Configuration */
    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            HSE PREDIV1                    = 1
 *            PLL2MUL                        = 9
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    // Initializes the CPU clock source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }

    // Initializes the CPU, AHB and APB buses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK
                                  | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1
                                  | RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }
}

/**
 * @brief  Initializes the Global MSP.
 * @param  None
 * @retval None
 */
void HAL_MspInit(void)
{
    /* Alternate Function I/O clock enable */
    __HAL_RCC_AFIO_CLK_ENABLE();

    /* Power interface clock enable */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_AFIO_REMAP_SWJ_NOJTAG();
}

/**
 * @brief  DeInitializes the Global MSP.
 * @param  None
 * @retval None
 */
void HAL_MspDeInit(void)
{}

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
        __NOP();
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
        __NOP();
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
        __NOP();
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
        __NOP();
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @brief This function is executed in case of error occurrence.
 *
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void Error_Handler(const char * file, int line)
{
    UNUSED(file);
    UNUSED(line);

    while (1) {
        __NOP();
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
}
#endif
