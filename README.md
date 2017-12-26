# 基于实例迁移和特征迁移相结合的跨领域模糊限制语识别
我们提出一种基于实例迁移和特征迁移相结合的跨领域中文模糊限制语识别方法，该方法的流程如下图所示，分为训练和测试两个过程。<br>
训练过程，首先基于基本特征，分别训练获得基于实例迁移与基于特征迁移的跨领域识别模型。然后将两个模型进行融合，形成组合模型。
<br>测试过程，分别采用两个模型识别目标领域测试语料中的模糊限制语，得到各自的识别结模糊限制信息检测研究果；再融合两个识别结果，得到最终的跨领域中文模糊限制语识别结果。

## 算法描述
特征迁移与实例迁移结合算法具体描述如下图所示<br>
![Method](https://github.com/DUT-NLP/Cross-domainCHd/blob/master/method.png)<br>
采用了经典的实例迁移学习方法 TrAdaBoost（简称TrA）和特征迁移学习方法 FruDA（简称 Fru）相结合的方法来进行特征和实例迁移<br>
## 数据集  
我们提供了CoNLL2010任务相关的模糊限制语识别维基百科领域的数据集和BioScope的生物医学领域的模糊限制于识别的数据集。
## 代码
我们提供了Fru和TrA的C++实现代码，详细的代码说明请查阅code文件夹下使用说明的PDF文件。
## 参考文献
Zhou H, Yang H, Chen L, et al. Combining Feature-Based and Instance-Based Transfer Learning Approaches for Cross-Domain Hedge Detection with Multiple Sources[M]// Social Media Processing. Springer Singapore, 2015.<br>
杨欢. 跨领域中文模糊限制语识别研究[D]. 大连:大连理工大学,2015.
