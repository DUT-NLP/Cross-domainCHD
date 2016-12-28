# 词向量与迁移学习相结合的跨领域模糊限制语识别
我们提出一种词向量与迁移学习相结合的跨领域中文模糊限制语识别方法，该方法的流程如下图所示，分为训练和测试两个过程。<br>
训练过程，首先基于大量未标注语料训练获得词向量特征，然后结合基本特征，分别训练获得基于实例迁移与基于特征迁移的跨领域识别模型。
<br>测试过程，分别采用两个模型识别目标领域测试语料中的模糊限制语，得到各自的识别结模糊限制信息检测研究果；再融合两个识别结果，得到最终的跨领域中文模糊限制语识别结果。
![Framework](https://github.com/DUT-NLP/Cross-domainCHd/blob/master/framework.png)<br>
图1.词向量与迁移学习结合方法流程
## 算法描述
特征迁移与实例迁移结合算法具体描述如下图所示<br>
![Method](https://github.com/DUT-NLP/Cross-domainCHd/blob/master/method.png)<br>
采用了经典的实例迁移学习方法 TrAdaBoost（简称TrA）和特征迁移学习方法 FruDA（简称 Fru）相结合的方法来进行特征和实例迁移<br>
## 代码
我们提供了Fru和TrA的C++实现代码
## 参考文献
Zhou H, Yang H, Chen L, et al. Combining Feature-Based and Instance-Based Transfer Learning Approaches for Cross-Domain Hedge Detection with Multiple Sources[M]// Social Media Processing. Springer Singapore, 2015.<br>
杨欢. 跨领域中文模糊限制语识别研究[D]. 大连:大连理工大学,2015.
