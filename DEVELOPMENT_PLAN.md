# ofxCv OpenCV 4.x 新機能サンプル開発計画

## 概要
OpenCV 4.x の新機能を活用した18個のサンプルを開発する。

## 開発フェーズ

### Phase 1: DNN系サンプル (6個)
深層学習モジュールを使用したサンプル。外部モデルファイルが必要。

| # | サンプル名 | 説明 | モデル | 難易度 |
|---|-----------|------|--------|--------|
| 1 | example-dnn-pose | 人体姿勢推定 | OpenPose/MoveNet | ★★★ |
| 2 | example-dnn-segmentation | セマンティックセグメンテーション | DeepLabV3 | ★★★ |
| 3 | example-dnn-depth | 単眼深度推定 | MiDaS | ★★★ |
| 4 | example-dnn-style | ニューラルスタイル転送 | fast-neural-style | ★★ |
| 5 | example-dnn-super-resolution | 画像超解像 | ESPCN/FSRCNN | ★★ |
| 6 | example-dnn-text | シーンテキスト検出 | EAST | ★★ |

### Phase 2: 画像処理系サンプル (6個)
OpenCV の画像処理機能を使用したサンプル。モデル不要。

| # | サンプル名 | 説明 | 主要API | 難易度 |
|---|-----------|------|---------|--------|
| 7 | example-stitching | パノラマ合成 | cv::Stitcher | ★★ |
| 8 | example-inpainting | 画像修復 | cv::inpaint | ★ |
| 9 | example-hdr | HDR画像生成 | cv::createMergeDebevec | ★★ |
| 10 | example-document-scanner | 書類スキャナー | cv::getPerspectiveTransform | ★★ |
| 11 | example-grabcut | 前景抽出 | cv::grabCut | ★★ |
| 12 | example-clahe | 適応的コントラスト | cv::createCLAHE | ★ |

### Phase 3: 検出・認識系サンプル (4個)
物体検出・認識機能を使用したサンプル。

| # | サンプル名 | 説明 | 主要API | 難易度 |
|---|-----------|------|---------|--------|
| 13 | example-barcode | バーコード検出 | cv::BarcodeDetector | ★ |
| 14 | example-face-landmark | 顔ランドマーク | cv::face::Facemark | ★★ |
| 15 | example-template-matching | テンプレートマッチング | cv::matchTemplate | ★ |
| 16 | example-color-detection | 色検出・追跡 | cv::inRange | ★ |

### Phase 4: その他のサンプル (4個)
その他の高度な機能を使用したサンプル。

| # | サンプル名 | 説明 | 主要API | 難易度 |
|---|-----------|------|---------|--------|
| 17 | example-background-subtraction | 動体検出 | cv::BackgroundSubtractorMOG2 | ★ |
| 18 | example-stereo | ステレオ深度推定 | cv::StereoBM | ★★★ |
| 19 | example-fisheye | 魚眼レンズ補正 | cv::fisheye | ★★ |
| 20 | example-morphology | 形態学的処理 | cv::morphologyEx | ★ |

---

## チェックリスト

### Phase 1: DNN系サンプル

#### - [x] 1. example-dnn-pose (人体姿勢推定)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] download_model.sh 作成
- [x] ビルド確認
- [x] 動作確認

#### [x] 2. example-dnn-segmentation (セマンティックセグメンテーション)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] download_model.sh 作成
- [x] ビルド確認
- [x] 動作確認

#### [x] 3. example-dnn-depth (単眼深度推定)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] download_model.sh 作成
- [x] ビルド確認
- [x] 動作確認

#### [x] 4. example-dnn-style (ニューラルスタイル転送)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] download_model.sh 作成
- [x] ビルド確認
- [x] 動作確認

#### [x] 5. example-dnn-super-resolution (画像超解像)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] download_model.sh 作成
- [x] ビルド確認
- [x] 動作確認

#### [x] 6. example-dnn-text (シーンテキスト検出)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] download_model.sh 作成
- [x] ビルド確認
- [x] 動作確認

### Phase 2: 画像処理系サンプル

#### [x] 7. example-stitching (パノラマ合成)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] サンプル画像追加
- [x] ビルド確認
- [x] 動作確認

#### [x] 8. example-inpainting (画像修復)
- [x] ディレクトリ作成
- [x] ofApp.h 作成
- [x] ofApp.cpp 作成
- [x] main.cpp 作成
- [x] addons.make 作成
- [x] Makefile/config.make コピー
- [x] ビルド確認
- [x] 動作確認

#### [ ] 9. example-hdr (HDR画像生成)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] サンプル画像追加 (異なる露出の画像セット)
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 10. example-document-scanner (書類スキャナー)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 11. example-grabcut (前景抽出)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 12. example-clahe (適応的コントラスト)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

### Phase 3: 検出・認識系サンプル

#### [ ] 13. example-barcode (バーコード検出)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 14. example-face-landmark (顔ランドマーク)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] download_model.sh 作成
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 15. example-template-matching (テンプレートマッチング)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 16. example-color-detection (色検出・追跡)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

### Phase 4: その他のサンプル

#### [ ] 17. example-background-subtraction (動体検出)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 18. example-stereo (ステレオ深度推定)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] サンプルステレオ画像追加
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 19. example-fisheye (魚眼レンズ補正)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] サンプル魚眼画像追加
- [ ] ビルド確認
- [ ] 動作確認

#### [ ] 20. example-morphology (形態学的処理)
- [ ] ディレクトリ作成
- [ ] ofApp.h 作成
- [ ] ofApp.cpp 作成
- [ ] main.cpp 作成
- [ ] addons.make 作成
- [ ] Makefile/config.make コピー
- [ ] ビルド確認
- [ ] 動作確認

---

## 必要なモデルファイル一覧

### DNN モデル

| サンプル | モデル名 | サイズ (概算) | ダウンロード元 |
|---------|---------|--------------|---------------|
| dnn-pose | openpose_pose_coco.caffemodel | ~200MB | CMU OpenPose |
| dnn-segmentation | deeplabv3_mnv2.onnx | ~8MB | TensorFlow Model Zoo |
| dnn-depth | midas_v21_small.onnx | ~80MB | Intel MiDaS |
| dnn-style | starry_night.t7 等 | ~7MB each | fast-neural-style |
| dnn-super-resolution | ESPCN_x4.pb | ~100KB | OpenCV DNN |
| dnn-text | frozen_east_text_detection.pb | ~100MB | EAST |
| face-landmark | lbfmodel.yaml | ~54MB | OpenCV Face |

---

## 開発順序 (依存関係考慮)

```
Phase 1 (DNN) ─┬─ 1. dnn-pose
               ├─ 2. dnn-segmentation
               ├─ 3. dnn-depth
               ├─ 4. dnn-style
               ├─ 5. dnn-super-resolution
               └─ 6. dnn-text

Phase 2 (画像処理) ─┬─ 7. stitching
                    ├─ 8. inpainting
                    ├─ 9. hdr
                    ├─ 10. document-scanner
                    ├─ 11. grabcut
                    └─ 12. clahe

Phase 3 (検出) ─┬─ 13. barcode
                ├─ 14. face-landmark
                ├─ 15. template-matching
                └─ 16. color-detection

Phase 4 (その他) ─┬─ 17. background-subtraction
                  ├─ 18. stereo
                  ├─ 19. fisheye
                  └─ 20. morphology
```

---

## 進捗サマリー

| Phase | 完了 | 合計 | 進捗 |
|-------|-----|------|------|
| Phase 1: DNN | 6 | 6 | 100% |
| Phase 2: 画像処理 | 2 | 6 | 33% |
| Phase 3: 検出 | 0 | 4 | 0% |
| Phase 4: その他 | 0 | 4 | 0% |
| **合計** | **8** | **20** | **40%** |

---

## 備考

- 各サンプルは独立して動作可能
- カメラを使用するサンプルは ofVideoGrabber を使用
- 画像ファイルを使用するサンプルは bin/data/ にサンプル画像を配置
- DNN サンプルは download_model.sh スクリプトでモデルをダウンロード
- ビルドは `make -j8` で並列コンパイル
- テストは起動後3秒間クラッシュしないことを確認
