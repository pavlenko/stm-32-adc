#include "adc.h"

extern DMA_HandleTypeDef DMAn;

void MX_ADC1_Init(ADC_HandleTypeDef *adc)
{
    ADC_ChannelConfTypeDef chConfig;

    /** Common config */
    adc->Instance                   = ADC1;
    adc->Init.ScanConvMode          = ADC_SCAN_ENABLE;
    adc->Init.ContinuousConvMode    = DISABLE;
    adc->Init.DiscontinuousConvMode = DISABLE;
    adc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    adc->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adc->Init.NbrOfConversion       = 4;

    if (HAL_ADC_Init(adc) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }

    chConfig.Channel      = ADC_CHANNEL_0;
    chConfig.Rank         = ADC_REGULAR_RANK_1;
    chConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    if (HAL_ADC_ConfigChannel(adc, &chConfig) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }

    chConfig.Channel      = ADC_CHANNEL_1;
    chConfig.Rank         = ADC_REGULAR_RANK_2;
    chConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    if (HAL_ADC_ConfigChannel(adc, &chConfig) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }

    chConfig.Channel      = ADC_CHANNEL_2;
    chConfig.Rank         = ADC_REGULAR_RANK_3;
    chConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    if (HAL_ADC_ConfigChannel(adc, &chConfig) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }

    chConfig.Channel      = ADC_CHANNEL_3;
    chConfig.Rank         = ADC_REGULAR_RANK_4;
    chConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    if (HAL_ADC_ConfigChannel(adc, &chConfig) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adc) {
    GPIO_InitTypeDef gpio;

    if (adc->Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**
         * PA0 ------> ADC1_IN0
         * PA1 ------> ADC1_IN1
         * PA2 ------> ADC1_IN2
         * PA3 ------> ADC1_IN3
         */
        gpio.Pin  = GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0;
        gpio.Mode = GPIO_MODE_ANALOG;

        HAL_GPIO_Init(GPIOA, &gpio);

        DMAn.Instance                 = DMA1_Channel1;
        DMAn.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DMAn.Init.PeriphInc           = DMA_PINC_DISABLE;
        DMAn.Init.MemInc              = DMA_MINC_ENABLE;
        DMAn.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        DMAn.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        DMAn.Init.Mode                = DMA_NORMAL;
        DMAn.Init.Priority            = DMA_PRIORITY_MEDIUM;

        if (HAL_DMA_Init(&DMAn) != HAL_OK) {
            Error_Handler(__FILE__, __LINE__);
        }

        __HAL_LINKDMA(adc, DMA_Handle, DMAn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adc)
{
    if (adc->Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_DISABLE();

        /**
         * PA0 ------> ADC1_IN0
         * PA1 ------> ADC1_IN1
         * PA2 ------> ADC1_IN2
         * PA3 ------> ADC1_IN3
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);

        HAL_DMA_DeInit(adc->DMA_Handle);
    }
}
