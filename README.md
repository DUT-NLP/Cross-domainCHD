# 词向量与迁移学习相结合的跨领域模糊限制语识别
提出一种词向量与迁移学习相结合的跨领域中文模糊限制语
识别方法，词向量与迁移学习结合方法流程如图 7-2 所示，分为训练和测试两个过程。<br>
训练过程，首先基于大量未标注语料训练获得词向量特征，然后结合基本特征，分别训练获得基于实例迁移与基于特征迁移的跨领域识别模型。
<br>测试过程，分别采用两个模型识别目标领域测试语料中的模糊限制语，得到各自的识别结模糊限制信息检测研究果；再融合两个识别结果，得到最终的跨领域中文模糊限制语识别结果。