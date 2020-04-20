# 大数据场景应用

基于TenDB Cluster透明分库分表、动态扩缩容的特点， TenDB Cluster适合大数据量应用的场景。如果应用具备如下特点则尤为推荐使用TenDB Cluster：
- 数据量大（50T+）
- 写多读少
- 读以点查询为主
- 少量复杂计算
- 可能需要定期清理数据   

大数据量的TenDB Cluster集群，运营过程中最重要的是避免TSpider计算量过大和存储层TenDB的IO负载过高。本文提供一些大数据量集群的运营经验。   

### **集群分片数**

TenDB Cluster集群如果想调整分片数，需要对全部数据进行重导，大数据量下这个运营成本很高。因此提前规划好集群分片数尤为重要，通常可以按需求的1.5倍到2倍来评估分片数。 为了便于存储实例扩缩，要选择公约数较多的数作为初始分片数，比如48、64、72分片。

### **表定义及操作**

大数据量集群下，对TenDB Cluster集群建表及请求，有如下建议：
1. TSpider建表时，默认使用主键/唯一键的第一个字段作为shard_key；如果没有主键/唯一键，需要显式指定shard_key
2. 建表指定shard_key时，数据按照shard_key hash取模后需要是分布均匀的。如果数据分布不均匀会造成部分分片负载过高成为瓶颈
3. 写数据时使用批量写的方法
4. 查询、更新条件尽量带shard_key作为等值条件
5. 避免在TSpider节点拉取大量数据，导致OOM
6. 避免执行计算量大的请求
7. 避免返回结果集大的join

### **存储实例管理**

大数据集群下，存储实例主要关注IO，对存储实例的管理有如下建议：
1. 数据压缩。推荐使用TenDB的大字段压缩功能，或者使用TokuDB及压缩效率高的引擎
2. 使用物理备份的方式来备份数据
3. 如有定期清理数据的需求，存储实例可以使用分区表
4. 对数据清理（drop partition）可使用硬链的方式防抖动
5. 非SSD磁盘下，尤其注意减少产生随机IO；比如减少二级索引反查、或者使用堆表写数据