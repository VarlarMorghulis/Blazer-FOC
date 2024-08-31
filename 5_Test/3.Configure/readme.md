## 模式配置

​		模式配置涉及驱动内部的参数更改，参数掉电不丢失依靠的是STM32内部flash，烧写flash是一个相对耗时且需要关闭全局中断的过程。因此，==请勿在电机运行过程中调用模式配置函数==，这可能会导致电机不正常运转甚至暴走。推荐的操作是可以单独开一个配置工程，在while(1)前调用配置函数，建议每次只配置一个参数，注释掉其他配置函数，依次配置完所有参数，每次配置完一个参数后，驱动若成功接收，等待2-3s后再进行下一次配置。

------

### **配置函数1：修改节点ID**

> ```
> void set_blazer_node_id(uint32_t node_id,float node_id_set);
> ```
>
> node_id：当前的CAN节点ID
>
> node_id_set：待设置的CAN节点ID

示例：

> ```
> set_blazer_node_id(0x02,0x01);
> ```

此段代码的作用是将节点ID从0x02修改为0x01。默认设置为0x01。

------

### **配置函数2： 设置极对数**

> ```
> void set_blazer_pole_pairs(uint32_t node_id,float pole_pairs);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> pole_pairs：极对数 范围2-40

示例：

> ```
> set_blazer_pole_pairs(0x02,7.0f);
> ```

此段代码的作用是将节点ID为0x02的驱动设置为7对极。默认设置为7对极。
------

### **配置函数3： 设置编码器类型**

> ```
> void set_blazer_encoder_type(uint32_t node_id,float enc_type);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> enc_type：编码器型号 0 TLE5012B 1 MT6816

示例：

> ```
> set_blazer_encoder_type(0x02,0.0f);
> ```

此段代码的作用是将节点ID为0x02的驱动设置为TLE5012B读取。默认编码器为TLE5012B。

------

### **配置函数4： 设置电机最大电流**

> ```
> void set_blazer_max_current(uint32_t node_id,float max_current);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> max_current：最大电流(A) 范围0-50A  

示例：

> ```
> set_blazer_max_current(0x02,30.0f);
> ```

此段代码的作用是将节点ID为0x02的驱动最大电流限制在30A。默认最大电流为30A。

------

### **配置函数5： 设置电机最大转速**

> ```
> void set_blazer_max_speed(uint32_t node_id,float max_speed);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> max_speed：最大机械转速(r/min) 

示例：

> ```
> set_blazer_max_speed(0x02,8000.0f);
> ```

此段代码的作用是将节点ID为0x02的驱动最大机械转速限制在8000r/min。默认最大转速为8000r/min。

### **配置函数6： 电机校准**

> ```
> void set_blazer_calib(uint32_t node_id,float mode);
> ```
>
> node_id：所控驱动的CAN节点ID
>
> mode：校准模式 0 校准ADC和编码器零偏

校准需要一定时间，期间请始终保持电机空载，电机超过5s没有任何反应且驱动未报错，则可认为校准完毕。

示例：

> ```
> set_blazer_max_speed(0x02,0.0f);
> ```

此段代码的作用是令节点ID为0x02的驱动开始进行ADC和编码器零偏校准。