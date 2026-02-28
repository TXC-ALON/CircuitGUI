# 📘 电路图绘制系统 — 技术方案（Qt C++）

## 🧱 一、总体架构

系统由四大模块组成：

1. **图形视图模块**：负责绘制所有图元（instance、port、netbus 等）
   - `QGraphicsScene` 管理所有图形项目（Items）
   - `QGraphicsView` 显示场景内容
   - 自定义 `QGraphicsItem` 实现电路图组件

2. **数据模型模块**：内存中的数据结构，处理实例、端口、连接关系，并与 UI 同步，支持 JSON 保存/导出

3. **图算法模块**：负责电路图内部信息控制与自动布局
   - 使用 **Boost Graph Library (BGL)** 表示图结构
   - 执行布局优化与布线策略生成

4. **文件导出/导入模块**：支持 JSON 格式电路图保存与加载
   - 使用 Qt JSON (`QJsonDocument`, `QJsonObject`, `QJsonArray`)

---

## 🧩 二、核心类设计

### 1) `InstanceItem` — 电路实例图形元素

```cpp
class InstanceItem : public QGraphicsRectItem {
public:
    InstanceItem(const QString &instName, const QString &moduleName);

    void addPort(PortItem *port);

    QString getInstName() const;
    QString getModuleName() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    QString instName;
    QString moduleName;
    QVector<PortItem*> ports;
};
```

显示规则：
- 实例是矩形
- `instName` 显示在矩形上方
- `moduleName` 显示在矩形下方

### 2) `PortItem` — 端口图元

```cpp
enum PortState { Connected, Disconnected };

class PortItem : public QGraphicsItem {
public:
    PortItem(const QString &name);

    void setState(PortState st);
    PortState state() const;

    void setNet(NetItem *net);

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *w) override;

private:
    QString portName;
    PortState portState;
    NetItem *net; // 当前连接的 Net
};
```

端点图标显示规则：
- **绿色菱形**：`Connected`
- **红色空心圆形**：`Disconnected`

### 3) `NetItem` — 连接线

```cpp
class NetItem : public QGraphicsPathItem {
public:
    NetItem();
    void addPort(PortItem *port);
    QVector<PortItem*> ports() const;

protected:
    void paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *widget) override;
private:
    QVector<PortItem*> connectedPorts;
    QPainterPath connectionPath;
};
```

特性：
- 支持多端口
- 使用 `QPainterPath` 绘制折线连接

### 4) `BusItem` / `PortBusItem` / `NetBusItem`

```cpp
class BusItem : public QGraphicsItem;
class PortBusItem : public BusItem {};
class NetBusItem : public BusItem {};
```

用于表示端口组或信号总线。

---

## 🧠 三、用户交互设计

### 鼠标事件：添加 `Instance`

1. 点击“添加 Instance”工具按钮
2. 输入 `Instance Name` + `Module Name`
3. 在 Scene 上点击放置位置

```cpp
connect(addInstBtn, &QPushButton::clicked, [&]() {
    QString name = getInput("Instance Name");
    QString module = getInput("Module Name");
    InstanceItem *item = new InstanceItem(name, module);
    scene->addItem(item);
});
```

### 添加 `Port`

```cpp
PortItem *port = new PortItem("p1");
instItem->addPort(port);
scene->addItem(port);
```

### 建立连接（`Net`）

1. 点击起始 Port
2. 点击结束 Port
3. 系统自动创建 `NetItem` 并连接两端 `PortItem`

---

## 💾 四、数据结构与 JSON Schema

### 内存模型

```cpp
struct CircuitModel {
    QVector<InstanceModel> instances;
    QVector<NetModel> nets;
};
```

### JSON 导入流程

1. **读取 JSON**：`QFile` + `QJsonDocument::fromJson()`
2. **解析数据**：填充中间数据结构 `Info`
3. **映射中间层**：实例、端口、连接信息标准化
4. **生成 GUI 项目**：根据 `Info` 创建 `QGraphicsItem`

示例中间层：

```cpp
class Info {
public:
    struct Port {
        QString id;
        QString type;
        QPointF position;
        bool connected;
    };

    struct Instance {
        QString id;
        QString name;
        QString module;
        QPointF position;
        QVector<Port> ports;
    };

    struct Net {
        QString id;
        QVector<QString> connectedPorts;
        QString status;
    };

    QVector<Instance> instances;
    QVector<Net> nets;
};
```

GUI 生成流程示例：

```cpp
void generateGuiFromInfo(const Info &info, QGraphicsScene *scene) {
    for (const auto &inst : info.instances) {
        InstanceItem *instanceItem = new InstanceItem(inst.name, inst.module);
        instanceItem->setPos(inst.position);
        scene->addItem(instanceItem);

        for (const auto &port : inst.ports) {
            PortItem *portItem = new PortItem(port.id);
            portItem->setPos(port.position);
            portItem->setState(port.connected ? PortState::Connected : PortState::Disconnected);
            scene->addItem(portItem);
            instanceItem->addPort(portItem);
        }
    }

    for (const auto &net : info.nets) {
        NetItem *netItem = new NetItem();
        for (const auto &portId : net.connectedPorts) {
            PortItem *portItem = findPortById(portId);
            netItem->addPort(portItem);
        }
        scene->addItem(netItem);
    }
}
```

---

## 🧠 五、图算法与布局布线（Boost）

### 1) 使用 Boost 图算法

使用 **Boost Graph Library (BGL)** 实现：
- 图表示：实例/端口/连接关系
- 布局优化：降低交叉、提升可读性
- 自动布线：通过最短路径等算法生成候选线路

### 2) 图算法应用示例

```cpp
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

void generateGraphAndOptimizeLayout(const Info &info) {
    Graph g;

    std::map<QString, Vertex> nodeMap;
    for (const auto &inst : info.instances) {
        Vertex v = add_vertex(g);
        nodeMap[inst.id] = v;
    }

    for (const auto &net : info.nets) {
        for (size_t i = 0; i + 1 < net.connectedPorts.size(); ++i) {
            Vertex u = nodeMap[net.connectedPorts[i]];
            Vertex v = nodeMap[net.connectedPorts[i + 1]];
            add_edge(u, v, g);
        }
    }

    std::vector<int> distances(num_vertices(g));
    Vertex start = nodeMap["port1"];
    boost::dijkstra_shortest_paths(g, start, boost::distance_map(&distances[0]));
}
```

### 3) 布局结果回写 GUI

算法计算后的节点坐标、路径点应回写到 `InstanceItem` 与 `NetItem`：
- 调整实例与端口位置
- 生成/更新 `QPainterPath` 折线路径
- 更新场景后触发重绘

---

## 🧱 六、工具与依赖

| 项目 | 版本 |
|---|---|
| Qt | 5.12+ |
| Boost Graph Library | 1.70+ |
| Qt Creator | 推荐 |
| JSON 序列化库 | Qt 内置 |
| 版本控制 | Git |

---

## 📌 七、扩展与未来方向

- JSON 加载并重建图形
- 导出 SVG / 图片
- Net 统计与报表
- 布线冲突检测与自动规避
- 布局约束（分层、对齐、模块组）
