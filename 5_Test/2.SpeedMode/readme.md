## 速度控制模式

### **step1：调用启动函数**

> ```
>  void set_blazer_start(uint32_t node_id,float mode);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> mode：0 电流模式 1 速度模式 2 位置模式

启动函数在while(1)前调用一次即可，确保成功通信，驱动才会使能，进入后续的闭环控制。

在速度控制模式下，mode取值为1.0f。

示例：

> ```
> set_blazer_start(0x02,1.0f);
> ```

此段代码的作用是让节点ID为0x02的驱动进入速度控制模式。

------

### **step2： 设置电机转速**

> ```
> void set_blazer_speed(uint32_t node_id,float speed);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> current：发送的设定机械转速(r/min)

此函数需要在运行中持续调用。

示例：

> ```
> set_blazer_speed(0x02,60.0f);
> ```

此段代码的作用是让节点ID为0x02的驱动在速度控制模式下，让电机以60r/min的转速运行。
