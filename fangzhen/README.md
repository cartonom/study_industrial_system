# fangzhen — Proteus 仿真工程

基于 Proteus Design Suite 的 STM32F103C8T6 嵌入式系统硬件仿真，与 `hejinghengxitong/` 固件工程配合使用，实现无需真实硬件即可完成代码调试与功能验证。

---

## 软件要求

| 项目 | 说明 |
|------|------|
| **仿真平台** | Proteus Design Suite 8.15 或更高版本 |
| **仿真工程** | `hejinghengxitong.pdsprj` |
| **固件文件** | `hejinghengxitong.hex`（位于 `../hejinghengxitong/MDK-ARM/hejinghengxitong/`） |
| **编码环境** | Windows-936（中文简体） |

---

## 文件说明

```
fangzhen/
├── hejinghengxitong.pdsprj                      # Proteus 主工程文件（8.15 版本）
├── hejinghengxitong.pdsprj.CARTOMOON.86198.workspace  # 用户工作区布局
├── README.md                                     # 本说明文档
└── Project Backups/                              # 工程自动备份
    ├── hejinghengxitong [20251123, 14-15-07].pdsprj  # 初始工程备份
    └── hejinghengxitong [Autosaved].pdsprj           # 最近自动保存
```

---

## 仿真电路组成

仿真电路完整复现了实际硬件平台的各功能模块，包含以下组件：

### 主控与外围

| 元件编号 | 器件型号 | 功能 | 对应实物 |
|----------|----------|------|----------|
| **U1** | STM32F103C8（QFN50P） | 主控 MCU，72MHz 主频 | STM32F103C8T6 最小系统板 |
| **U2** | DHT11 | 温湿度传感器（DATA/VDD/GND） | DHT11 模块 |
| **LCD1** | OLED12864I2C | 0.96 寸 OLED 显示屏（I2C 接口） | SSD1306 OLED 模块 |
| **LDR1** | TORCH_LDR | 光敏电阻 + 模拟光源（ORP-12） | 光照传感器模块 |

### 人机交互

| 元件编号 | 器件类型 | 功能 | 对应引脚 |
|----------|----------|------|----------|
| **QUEDING** | SPST 按钮 | 确认键（切换页面） | PB0 |
| **XUANZE** | SPST 按钮 | 选择键（切换子状态） | PB1 |
| **SWITCH** | SPST 拨动开关 | 手动开关输入 | PA6 |

### LED 指示灯

| 元件编号 | 器件类型 | 功能 | 对应引脚 |
|----------|----------|------|----------|
| **D3** | LED-RED（红色） | LED1（流水灯） | PA3 |
| **D4** | LED-RED（红色） | LED2（流水灯） | PA4 |
| **D5** | LED-RED（红色） | LED3（流水灯） | PA5 |
| **D0** | LED-RED（红色） | PWM 输出指示 | PA0 |
| **D1/D2/D6/D7** | LED-RED（红色） | 自动控制输出指示 | PB12-PB15 |

### 无源器件

| 元件编号 | 类型 | 用途 |
|----------|------|------|
| **R1-R10** | 电阻（多种封装） | LED 限流、上拉电阻、LDR 分压 |

### 虚拟仪器与外设

| 仪器 | 连接 | 用途 |
|------|------|------|
| **虚拟终端（Virtual Terminal）** | PA9（TXD）/ PA10（RXD） | USART1 串口通信监视，9600 8N1 |
| **虚拟示波器（Oscilloscope）** | PA0（TIM2 CH1） | PWM 波形观测 |
| **信号发生器（Signal Generator）** | 备用 | 模拟外部信号输入 |

### 电源系统

| 电源轨 | 电压 | 用途 |
|--------|------|------|
| **VCC/VDD** | +5V | DHT11、OLED 供电 |
| **V33** | +3.3V | STM32 MCU 核心供电 |
| **GND** | 0V | 公共地 |
| **VEE** | -5V | 预留负电源 |

---

## 使用步骤

### 第一步：编译固件（如已有 HEX 可跳过）

1. 打开 Keil MDK-ARM V5
2. 加载工程 `../hejinghengxitong/MDK-ARM/hejinghengxitong.uvprojx`
3. 点击 **Build（F7）** 编译
4. 确认生成 `hejinghengxitong.hex`（位于 `MDK-ARM/hejinghengxitong/` 目录下）

### 第二步：打开仿真工程

1. 启动 Proteus Design Suite
2. 点击 **File → Open Project**，选择 `hejinghengxitong.pdsprj`
3. 确认电路原理图正确加载

### 第三步：加载 HEX 固件

1. 在原理图中**双击 U1（STM32F103C8）** 打开属性窗口
2. 在 **Program File** 栏中，点击文件夹图标
3. 选择编译生成的 `hejinghengxitong.hex` 文件
   - 路径：`../hejinghengxitong/MDK-ARM/hejinghengxitong/hejinghengxitong.hex`
4. 确认 **CKSEL Fuses** 配置：
   - **OSC Frequency**：72MHz
   - **OSC32 Frequency**：0（未使用外部 32kHz 晶振）
5. 点击 **OK** 确认

### 第四步：运行仿真

1. 点击左下角**运行按钮（▶ Play）** 启动仿真
2. 观察 OLED 显示屏是否正常显示欢迎界面
3. 通过点击 **QUEDING** 和 **XUANZE** 按钮进行页面导航
4. 通过虚拟终端窗口观察串口输出
5. 通过虚拟示波器观察 PWM 波形

### 第五步：调试与交互

- **按钮点击**：在原理图上点击按钮元件右侧的箭头图标来模拟按下
- **开关拨动**：点击 SWITCH 元件的箭头切换开关状态
- **LDR 光照调节**：点击 LDR1 旁的光源（TORCH）元件调整模拟光照强度
- **串口监视**：右键虚拟终端 → 设置波特率 9600、8N1
- **PWM 观测**：将示波器通道连接到 PA0 引脚网络

---

## 引脚与网络连接对照

仿真电路中的网络连接与实际硬件引脚分配完全一致（详见 [主 README 引脚分配表](../README.md#引脚分配)），关键连接如下：

| 网络标签 | 连接对象 | 功能 |
|----------|----------|------|
| PA0 | U1→D0→示波器 | PWM 输出（TIM2 CH1） |
| PA1 | U1→LDR1 | ADC1 CH1 光照采集 |
| PA2 | U1→U2(DATA) | DHT11 单总线数据 |
| PA3-PA5 | U1→D3/D4/D5 | 流水灯输出 |
| PA6 | U1→SWITCH | 开关量输入 |
| PA9 | U1→虚拟终端(TXD) | USART1 发送 |
| PA10 | U1→虚拟终端(RXD) | USART1 接收 |
| PB0 | U1→QUEDING | 确认键输入 |
| PB1 | U1→XUANZE | 选择键输入 |
| PB8 | U1→LCD1(SCL) | OLED I2C 时钟 |
| PB9 | U1→LCD1(SDA) | OLED I2C 数据 |
| PB12-PB15 | U1→D1/D2/D6/D7 | 自动控制输出 |

---

## 常见问题

### Q：仿真运行后 OLED 无显示？
- 确认 HEX 文件已正确加载到 U1
- 检查 VCC/VDD（5V）和 V33（3.3V）电源轨是否正常
- 确认 OSC Frequency 设置为 72MHz

### Q：DHT11 读数异常？
- DHT11 在 Proteus 中使用 DHTXX 仿真模型，存在约 1 秒初始化延迟
- 等待仿真运行 1-2 秒后再进入传感器页面查看

### Q：串口无输出？
- 检查虚拟终端波特率是否设为 9600
- 确认 PA9/PA10 网络连接正确
- 程序需切换到串口页面才会发送数据

### Q：PWM 波形不正确？
- TIM2 配置为 PSC=71、ARR=99，频率约 7.2kHz
- 在 PWM 页面切换不同占空比档位后观察变化

### Q：工程版本不兼容？
- 本工程使用 Proteus 8.15 创建
- 如使用更高版本打开，Proteus 会自动提示转换
- 如使用较低版本，请升级至 8.15 或更高

---

## 备份与恢复

`Project Backups/` 目录保存了工程的历史版本：

- **`hejinghengxitong [20251123, 14-15-07].pdsprj`**：原始工程备份（2025-11-23，初次创建时）
- **`hejinghengxitong [Autosaved].pdsprj`**：最近一次自动保存（2025-12-24）

若当前工程文件损坏或意外修改，可将备份文件复制到上级目录并重命名为 `hejinghengxitong.pdsprj` 恢复。

---

## 版本信息

| 日期 | 说明 |
|------|------|
| 2025-11-23 | 初始版本，完成全部电路搭建与仿真验证 |
| 2025-12-24 | 自动保存更新 |
| 2026-06-03 | 补充完整仿真说明文档 |

---

*仿真环境：Proteus Design Suite 8.15 + STM32F103C8T6 + DHT11 + OLED12864I2C*
