# ESP32CAM plant monitor

基於ESP32 CAM的盆栽監視系統，使用ESP-IDF開發

Nothing here, it's not done yet.

or you can checkout `dev` branch.

## 功能/TODO

- [ ] 土壤濕度監控
- [ ] 土壤酸鹼監視
- [ ] 日照亮度監控
- [ ] 水箱水位監視
- [ ] 類縮時攝影
- [ ] 系統狀態回報
- [ ] 接收控制指令
- [ ] 任務管理

## 編譯/上傳

```sh
#設定平台
$ idf.py set-target esp32
#設定參數
$ idf.py menuconfig
#編譯
$ idf.py build
#上傳
$ idf.py -p <port> flash
```