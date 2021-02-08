# ESP32CAM plant monitor

基於ESP32 CAM的盆栽監視系統，使用ESP-IDF開發

Initial commit, nothing there

## 功能/TODO

- [ ] 土壤濕度監控
- [ ] 土壤酸鹼監視
- [ ] 日照亮度監控
- [ ] 水箱水位監視
- [ ] 類縮時攝影
- [ ] 系統狀態回報
- [ ] 接收控制指令

## 編譯/上傳

```sh
#設定平台
$ idf.py set-target esp32
#編譯
$ idf.py build
#上傳
$ idf.py -p <port> flash
```