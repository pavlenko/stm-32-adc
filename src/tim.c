#include "tim.h"

void MX_TIM2_Init(TIM_HandleTypeDef *tim) {
    TIM_OC_InitTypeDef sConfigOC;
    TIM_MasterConfigTypeDef sMasterConfig;

    tim->Instance           = TIM2;
    //tim->Init.Prescaler     = (uint32_t) (SystemCoreClock / 1000000) - 1;
    tim->Init.Prescaler     = (uint32_t) (SystemCoreClock / 125000) - 1;
    tim->Init.CounterMode   = TIM_COUNTERMODE_UP;
    tim->Init.Period        = 20000;
    tim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    HAL_TIM_PWM_Init(tim);

    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse      = 1000;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(tim, &sConfigOC, TIM_CHANNEL_1);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_ENABLE;

    HAL_TIMEx_MasterConfigSynchronization(tim, &sMasterConfig);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* timPWM) {
    if (timPWM->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();

        HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* timPWM) {
    if (timPWM->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();

        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
}
