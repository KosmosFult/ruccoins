## 



## 文件说明

- client
  - `client.h/client.cc`: client的类，实现功能逻辑
  - `client_main.cc`: client主程序
- coin_node
  - `coin_node.h/coin_node.cc`: coin node的类，实现功能逻辑
  - `coin_rpc.h/coin_rpc.cc`: 向外部暴露的rpc函数接口，被绑定到rpc server，会被client(发送交易消息)，worker(发送挖矿任务结果)和其他coin node(发送区块)调用。
- worker_node
  - 同coin_node
- utils
  - utils.h: 实现一些功能函数，例如签名和验证签名



## 其他说明
- `CoinNode`和`WorkerNode` 为单实例类，可以通过 `getInstance` 获取该实例的引用。因此在`xx_rpc.cc`中实现函数时，应当先用`getInstance` 获取类实例，进而可以调用类方法。
参见client.cc->TestRun
  - 调用node的`call`方法，进而调用一个名为 `AddTransx` 的函数
  - 该函数在`coin_rpc.cc`中定义，它先通过 `getInstance()` 静态方法获取CoinNode实例，进而可以使用实例方法。