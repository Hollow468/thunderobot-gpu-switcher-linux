## 先决条件 / 依赖

在开始编译之前，请确保系统已安装用于编译内核模块的工具与内核头文件。下面列出常见发行版的安装命令：

- Debian / Ubuntu:
```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r) git
```

- Fedora:
```bash
sudo dnf install -y @development-tools kernel-headers kernel-devel git
```

- Arch Linux:
```bash
sudo pacman -S --needed base-devel linux-headers git
```

- 其他：
确保安装 `make`、`gcc`、内核头文件（版本需与当前运行内核一致）。

如果系统启用了 Secure Boot，加载未签名的内核模块可能失败（见常见问题）。

---

## 编译（构建）

1. 克隆仓库（如果尚未克隆）：
```bash
git clone https://github.com/Hollow468/thunderobot-gpu-switcher-linux.git
cd thunderobot-gpu-switcher-linux
```

2. 使用 Makefile 编译：
```bash
make
```

3. 编译成功后，通常会在当前目录生成一个或多个 `.ko` 文件（内核模块）。查看：
```bash
ls -lah *.ko
```

如果 `make` 报错，请把完整的编译错误信息记录下来用于排查。

---

## 安装与加载

下面给出临时加载（测试）与安装到系统模块目录并在引导时自动加载的说明。

### 临时加载（用于测试）
找到生成的模块文件名，例如 `lsgpu.ko`，然后：
```bash
# 在当前目录临时插入模块
sudo insmod ./lsgpu.ko

# 或使用 modprobe（如果已安装到模块目录）
sudo modprobe lsgpu
```

卸载（移除）模块：
```bash
sudo rmmod lsgpu
```

查看已加载模块：
```bash    
lsmod | grep lsgpu
```

### 使用说明

```bash
# 切换混合模式
echo “1” | sudo tee /sys/kernel/lsgpu/mode
# 切换独显模式
echo “2” | sudo tee /sys/kernel/lsgpu/mode
# 切换核显模式
echo “3” | sudo tee /sys/kernel/lsgpu/mode

