# SPSCQueue是一个高性能的无锁队列

## Warning：适用于单生产者单消费者。

## usage:

### 创建：

NEWPLAN::SPSCQueue<ElemType> *spsc_q = new NEWPLAN::SPSCQueue<ElemType>(102400);
  
### 插入：

spsc_q->push(data);

### 弹出：

#### 阻塞：

new_data = spsc_q->blocking_pop();

#### 非阻塞：

do
{recv_ptr = spsc_q->front();} while (recv_ptr == nullptr);
spsc_q->pop();
