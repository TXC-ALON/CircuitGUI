# CircuitGUI

Qt C++ 电路图绘制系统原型。

## 文档

- 技术方案：`docs/circuitgui-technical-plan.md`

## 当前实现

- `QGraphicsScene/QGraphicsView` 场景渲染
- `InstanceItem` / `PortItem` / `NetItem` 基础图元
- JSON 导入为中间模型 `Info`
- 基于 Boost Graph 的实例层级距离计算示例

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/circuitgui [optional-json-file]
```
