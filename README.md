# &emsp; dalib &emsp; 一些常见数据结构和算法实现

## 分配器
用malloc封装的一个简易的分配器  
**构造对象**  
>construct  

**析构对象**  
>destroy  

**申请空间**  
>allocate  

**释放空间**  
>deallocate  

----------------------  
## avl树
平衡二叉搜索树的一种, 插入、删除、查找最坏情况的时间复杂度为O(nlgn)  

**树平衡的操作(代码见tree.h):**  
+ 左旋node节点  
>&emsp;&emsp;&emsp;&emsp;parent　　　　　　　　　　　　parent  
　　　　 　　\  　　　　　　　　　　　　　　\  
　　　　　　node  　　　　左旋  　　　　　　right  
　　　　　　/　\　　　　------->　 　　　　/　　\  
　　　　　left　right  　　　　　　　　　node　　rr  
　　　　　　　/　　\　　　　　　　　　 /　　\  
　　　　　　rl　　　rr　　　　　　　left　　　rl  

+ 右旋node节点  
>&emsp;　　　parent　　　　　　　　　　　  parent  
　　　　　 /  　　　　　　　　　　　　　　/  
　　　　node  　　　　右旋  　　　　　　left  
　　　　/　\　　　　------->　 　　　　/　　\  
　　　left　right  　　　　　　　　　　ll　　node  
　　　/　\　　　　　　　　　 　　　　　　/　　\  
 　　ll　　lr　　　　　　　　　　　　　　lr　　　right  

+ 左右旋转node节点(先左旋node的左孩子, 再右旋node)  
>&emsp;　　　parent　　　　　　　　　patent　　　　　　　　parent  
　　　　　/  　　　　　　　　　　　/  　　　　　　　　　　/  
　　　　node  　　左旋left  　　　　node  　右旋node　　　lr  
　　　　/　　　 ---------->  　　　/  　　----------->　　　/　\    
　　　left  　　　　　　　　　　lr　　　　　　　　　　left    node  
　　　　　\  　　　　　　　　　/  
　　　　　lr　　　　　　　　left  

+ 右左旋转node节点(先右旋node的右孩子, 再左旋node)
>&emsp;　　　parent　　　　　　　　　　patent　　　　　　　　　　　　parent  
　　　　　　\  　　　　　　　　　　　　　\  　　　　　　　　　　　　　\  
　　　　　　node  　　　右旋right  　　　　node  　　左旋node　　　　　rl
　　　　　　　　\　　　 ---------->  　　　　　\  　　----------->　　　／　＼  
　　　　　　　　right  　　　　　　　　　　　　rl　　　　　　　　　node    right
　　　　　　　　/  　　　　　　　　　　　　　　\  
　　　　　　　 rl　　　　　　　　　　　　　　right

**插入操作:**  
> 假设要插入的值为key。  
>  1. 当前节点为curr, 用parent指针表示curr的父亲。从根节点开始，curr = root  
>  2. 如果curr指向的节点值小于key，curr指向curr的左孩子  
>  3. 否则(curr指向节点值大于等于key), curr指向curr的右孩子
>  4. 重复步骤2和3, 每一次先把curr的值先赋给parent，再更改curr,直到curr为空就可以>     找到要插入的地方.  

**插入后的平衡操作:**
>  定义平衡因子(bf)为当前节点左子树高度减去右子树高度
>  1. 从插入的节点开始向根节点回溯(每次平衡完之后就回到上一层)
>  2. 若当前节点的bf>1  
>     &emsp;若左子树的bf>0, 进行右旋操作  
>     &emsp;若左子树的bf<0, 进行左右旋转操作  
>     若当前节点的bf<-1  
>     &emsp;若右子树的bf<0, 进行左旋操作  
>     &emsp;若左子树的bf>0, 进行右左旋转操作
>  3. 每次平衡完之后，更新节点的高度

**删除操作:**
>  1. 找到目标节点(没找到就不用删除了), 和该节点的后继节点
>  2. 若目标节点只有左孩子，就用左孩子来替代目标节点  
      若目标节点只有右孩子，就用右孩子来替代目标节点  
      若目标节点左右孩子都有，就用后继节点来替代目标节点

**删除后的平衡操作:**  
>  1. 从被删除节点的替代节点开始向根节点回溯   
>  2. 若当前节点的bf>1  
      &emsp;若左子树的bf>0, 进行右旋操作  
      &emsp;否则, 进行左右旋转操作  
      若当前节点的bf<-1  
      &emsp;若右子树的bf<0, 进行左旋操作  
      &emsp;否则, 进行右左旋转操作  
>  3. 每次平衡完之后，更新节点的高度