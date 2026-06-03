# hejinghengxitong 嵌入式实验系统

基于 STM32F103C8T6 的多功能嵌入式综合实验平台，集成传感器采集、OLED显示、执行器控制和串口通信等功能。

---

## 硬件平台

| 项目 | 参数 |
|------|------|
| **主控芯片** | STM32F103C8T6（Cortex-M3） |
| **系统时钟** | 72MHz（外部 HSE + PLL ×9） |
| **开发环境** | STM32CubeMX 6.1.2 / MDK-ARM V5 |
| **HAL 库版本** | STM32Cube FW_F1 V1.8.6 |
| **显示模块** | 0.96寸 OLED 128×64（I2C） |
| **温湿度传感器** | DHT11 |
| **仿真环境** | Proteus（见 `fangzhen/` 目录） |

---

## 引脚分配

### 输入信号

| 引脚 | 功能 | 模式 |
|------|------|------|
| PB0 | 确认键（切换页面） | GPIO Input，上拉 |
| PB1 | 选择键（切换子状态） | GPIO Input，上拉 |
| PA1 | ADC1 CH1（光照传感器） | ADC Input |
| PA6 | 拨动开关 | GPIO Input |
| PA10 | USART1 RX | USART 异步接收 |
| PA13 | SWDIO | 调试接口 |
| PA14 | SWCLK | 调试接口 |

### 输出信号

| 引脚 | 功能 | 模式 |
|------|------|------|
| PA0 | TIM2 CH1（PWM 输出） | 复用推挽输出 |
| PA2 | DHT11 数据线 | GPIO Output / Input |
| PA3 | LED1（流水灯） | 推挽输出 |
| PA4 | LED2（流水灯） | 推挽输出 |
| PA5 | LED3（流水灯） | 推挽输出 |
| PA9 | USART1 TX | USART 异步发送 |
| PB8 | OLED SCL（I2C 时钟） | 推挽输出 |
| PB9 | OLED SDA（I2C 数据） | 推挽输出 |
| PB12 | 自动控制输出1（温度） | 推挽输出 |
| PB13 | 自动控制输出2（湿度） | 推挽输出 |
| PB14 | 自动控制输出3（光照） | 推挽输出 |
| PB15 | 自动控制输出4（开关） | 推挽输出 |

---

## 系统架构

```
hejinghengxitong/
├── Src/                    # 用户源码
│   ├── main.c              # 主程序（界面逻辑、业务调度）
│   ├── adc.c               # ADC 初始化与采集
│   ├── gpio.c              # GPIO 初始化
│   ├── tim.c               # 定时器初始化（TIM2 PWM, TIM4 1s 定时）
│   ├── usart.c             # USART1 初始化与中断
│   └── stm32f1xx_it.c      # 中断服务函数
├── Inc/                    # 头文件
│   ├── main.h              # 主头文件
│   ├── adc.h / gpio.h / tim.h / usart.h
│   └── stm32f1xx_hal_conf.h
├── Hardware/               # 硬件驱动层
│   ├── dht11.c/h           # DHT11 温湿度传感器驱动
│   ├── delay.c/h           # 微秒/毫秒延时函数
│   └── sys.h               # 系统宏定义
├── OLED/                   # OLED 显示驱动
│   ├── oled.c/h            # I2C OLED 驱动（含中文字库）
│   ├── oledfont.c/h        # 字库数据
│   └── bmp.c/h             # 位图资源
├── Drivers/                # STM32 HAL 库
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── MDK-ARM/                # Keil 工程文件及编译产物
├── hejinghengxitong.ioc    # STM32CubeMX 工程配置
└── fangzhen/               # Proteus 仿真工程
```

---

## 功能说明

系统通过 **PB0（确认键）** 和 **PB1（选择键）** 进行菜单导航，在 0.96 寸 OLED 上显示中文交互界面。

### 页面导航结构

```
┌─────────────────────────────────────────────┐
│              开机画面（欢迎界面）              │
│        "欢迎使用何靖恒系统" + "确定进入"       │
│                  ↓ PB0                       │
│            ┌─────────────────┐              │
│            │  1. 主菜单       │              │
│            │  · 传感器        │ ← PB1 切换    │
│            │  · 灯            │              │
│            │  · PWM           │              │
│            │  · 串口          │              │
│            │  · 自动控制      │              │
│            │  · 关于          │              │
│            └───┬───┬───┬─────┘              │
│        PB0 ↓  │   │   │  各子项 PB0 → 进入   │
│     ┌─────────┘   │   └──────────┐           │
│     ↓             ↓              ↓           │
│  2.传感器页面  3.流水灯页面  4.PWM页面        │
│  · 温度       · 全亮      · PWM-0           │
│  · 湿度       · 流水灯    · PWM-20          │
│  · 光照       · 全灭      · PWM-50          │
│  · 开关状态   · 返回      · PWM-80          │
│  · 返回                   · 返回            │
│                                             │
│  5.串口页面         6.自动控制页面           │
│  · 回声显示        · 阈值自动控制            │
│  · 返回            · 返回                    │
└─────────────────────────────────────────────┘
```

### 功能模块详解

#### 1. 传感器采集（Page 2）
- **温度**：DHT11 实时采集（整数显示），每秒更新
- **湿度**：DHT11 实时采集（整数显示），每秒更新
- **光照**：ADC1 CH1 读取模拟值（0-4095），每秒更新
- **开关**：检测 PA6 引脚数字电平（0/1）

#### 2. 流水灯控制（Page 3）
- **全亮模式**：PA3/PA4/PA5 同时输出高电平
- **流水灯模式**：三路 LED 依次点亮，每秒切换
- **全灭模式**：PA3/PA4/PA5 同时输出低电平

#### 3. PWM 输出（Page 4）
- 4 档占空比可调：0%、20%、50%、80%
- 使用 TIM2 CH1，频率约 7.2kHz（PSC=71, ARR=99）
- 可用于 LED 调光或电机调速

#### 4. 串口通信（Page 5）
- USART1，波特率 9600，8N1
- 接收数据并以 `#` 作为帧结束标志
- 收到 `#` 后将整帧数据回显（echo）并显示在 OLED 上

#### 5. 自动控制（Page 6）
根据传感器阈值自动控制 4 路继电器/输出：

| 输出引脚 | 触发条件 | 用途示例 |
|----------|----------|----------|
| PB12 | 温度 > 30°C | 散热风扇 |
| PB13 | 湿度 > 30% | 除湿机 |
| PB14 | 光照 ADC > 2000 | 遮光帘/补光灯 |
| PB15 | PA6 开关导通（=0） | 手动联动设备 |

---

## 时钟与外设配置

| 外设 | 时钟源 | 频率 |
|------|--------|------|
| SYSCLK | PLL（HSE ×9） | 72 MHz |
| HCLK | SYSCLK ÷1 | 72 MHz |
| APB1 | HCLK ÷2 | 36 MHz |
| APB2 | HCLK ÷1 | 72 MHz |
| TIM4（1s 定时） | APB1 ×2 | 72 MHz → PSC=7199, ARR=9999 → 1s |
| TIM2（PWM） | APB1 ×2 | 72 MHz → PSC=71, ARR=99 → ~7.2kHz |
| ADC1 | APB2 ÷6 | 12 MHz |
| USART1 | APB2 | 72 MHz |

---

## 编译与烧录

### 使用 MDK-ARM（Keil）
1. 双击 `hejinghengxitong/MDK-ARM/hejinghengxitong.uvprojx` 打开工程
2. 点击 **Build（F7）** 编译
3. 通过 ST-Link/DAP-Link 连接开发板
4. 点击 **Download（F8）** 烧录

### 使用 Proteus 仿真
1. 打开 `fangzhen/hejinghengxitong.pdsprj`
2. 加载编译生成的 `.hex` 文件（已包含在 `MDK-ARM/hejinghengxitong/` 中）
3. 点击运行仿真

---

## 版本历史

| 日期 | 说明 |
|------|------|
| 2025-11-23 | 初始版本，完成全部功能开发与 Proteus 仿真验证 |

---

## 许可证

本项目基于 STM32CubeMX 生成，HAL 库部分版权归 STMicroelectronics 所有（BSD 3-Clause）。

---

*Powered by STM32F103C8T6 + STM32CubeMX + MDK-ARM*
