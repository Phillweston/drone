# 小四轴飞控
version V0.1.0

## 代码说明

### 用户代码
尽量不改动自动生成的代码，用户添加的代码文件在user文件夹中，与自动生成的代码文件分离。

自动生成的代码中加入的用户代码包括：

main.c:初始化，死循环，定时器3任务调度

main.h:宏定义u8,u16,u32

### 变量/函数/宏定义命名格式
变量:全部小写或首字母大写 xxx/Xxx/XxxXxx

函数:首字母大写加下划线分隔 Xxx_Xxx()

宏定义:全部大写加下划线分隔 XXX_XXX

### 控制链路
控制协议见 https://githuub.com/xd15zhn/GroundStation

## 功能说明
* 1秒内未接收到正确的遥控信号后或飞机姿态倾角过大，将控制指令设为横滚/俯仰/偏航均为0，油门大小为预设的比定高稍小的值。

## bug
* 功能基本完整,但不能飞。

* 异步接收数据帧有较大概率漏接。