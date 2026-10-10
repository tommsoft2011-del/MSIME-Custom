# 水杉输入法定制版 (MSIME Custom)

基于 [metasequoiaime/MSIME-Windows](https://github.com/metasequoiaime/MSIME-Windows) 的定制修改版。
上游分支：`develop`（本仓库的 develop 与上游保持同步，定制改动以 commit 形式叠加）。

## 定制改动一览

### 1. 部分拼音输入（engine/quanpin/quanpin_query.cpp）
合法音节的**不完整前缀**可作为完整形式参与切分与候选，独立于纠错开关：

| 输入 | 切分 | 结果 |
|------|------|------|
| `zhge` | zhe + ge | "这个" 置顶第 1 位 |
| `zhg` | zhe + ge（末尾单字母） | "中国" 置顶第 1 位 |
| `maifz` | mai + fang + zi（全拼+简拼混合） | "买房子" 置顶第 1 位 |

- 部分拼音的最佳词强制放在第 1 位，词优先于单字。
- 单字母别名按词库实际词频排序（`fang` 排在 `fo` 前），避免 k-best 截断时常用音节被挤掉。
- 英文判定仍要求至少 2 个字母的部分拼音前缀，避免 `abc` 被误判成拼音。

相关提交：`e7a1d5d`、`d37751a`、`71bdf1d`、`fdb9b4a`

### 2. 中文状态下连续输入网址 / 英文串（engine/contracts/url_english_input.h 等）
组合中敲 `.` `@` 不再上屏、也不清空候选，而是作为**编码键**进入输入串（仿照 V 模式）；串里有了网址符号后，`-` `_` `/` `:` 也是编码键。

| 输入 | 候选 | 回车上屏 |
|------|------|----------|
| `nihao.` | 与 `nihao` 相同（你好……） | `nihao.` |
| `aaa.com` | 与 `aaa` 相同 | `aaa.com` |
| `test@example.com` | 与 `test` 相同 | `test@example.com` |

- 候选只由第一个网址符号之前的部分产生（引擎把它当作光标前缀解码，`InputSession::url_symbol_prefix_end`）。
- 空格 / 数字选词：选的是前缀的候选，符号及之后的部分留在组合里，再按回车一起上屏（如 `你好.com`）。
- 退格删掉符号后自动回到普通拼音。
- 只在全拼 / 双拼生效；Server 通过 `VModeChanged` 载荷的第二位告诉 TSF 当前方案。开了"逗号句号翻页"时 `.` 仍然翻页。
- 需要同时替换 **Server** 和 **TSF DLL**（两边按同一条规则吃键，只换一边输入串会分叉）。

### 3. 英文输入中的 `.` 不翻页（server/src/ipc/event_listener.cpp）
- Server 的 `paging_comma_period` 原本会在 InputSession 之前把 `.` 当翻页键。
- 检测到英文临时模式，或本次 `.` 会触发英文模式时，放行 `.` 给 InputSession，不翻页。
- 正常中文输入时的逗号/句点翻页保留。

### 4. 英文翻译词典（custom_translations.txt）
- 基于 CC-CEDICT（开源中英词典）清洗生成，共 105,044 条。
- 放在 `english.db` 同目录，Server 启动时自动加载，优先级高于 `english.db` 大表。
- 效果：每个中文候选词后显示英文翻译（如 书→book、输入法→input method）。

## 安装

### 一键安装（推荐）
从 [Releases](../../releases) 页面下载 `MSIME-Custom-v5-install.zip`，解压后右键 `install.ps1` → 用 PowerShell **管理员权限**运行。

脚本自动完成：签名（用本机自签名证书 `CN=Metasequoia IME Local Test`）→ 先杀 Watchdog 再杀 Server → 备份旧版 → 覆盖安装 → 安装翻译词典。

### 手动安装
1. 用自签名证书签名 `MetasequoiaImeServer.exe`（manifest 含 `uiAccess=true`，未签名无法被 Watchdog 拉起）。
2. 任务管理器：先结束 `MetasequoiaImeWatchdog.exe`，再结束 `MetasequoiaImeServer.exe`。
3. 覆盖到 `C:\Program Files\metasequoiaime\server\MetasequoiaImeServer.exe`（管理员权限）。
4. 把 `custom_translations.txt` 放到 `english.db` 同目录。
5. 切换输入法，Server 自动启动。

## 构建
本仓库的 `.github/workflows/build-server-binary.yml` 可在 GitHub Actions（windows-2025）上编译出 `MetasequoiaImeServer.exe`（未签名，需自行签名）。

## 许可
上游代码 GPL-3.0，本定制版同样遵循 GPL-3.0。对外分发修改版必须遵守 GPL-3.0。
