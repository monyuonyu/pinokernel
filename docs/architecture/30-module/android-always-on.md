# Android の常駐の作り直し（inbox #7 の1、設計メモ）
<!-- vmodel rung: OPEN -->
<!-- vmodel certs: - -->

2026-10-04。ストアは新しいアプリに targetSdk 36 を求める。今のアプリは targetSdk 34、常駐は `PKernelService`（foregroundServiceType=dataSync）。

## 原文で確かめた決まり（developer.android.com、2026-10-04 に読んだ）
- dataSync と mediaProcessing は「24時間で合計6時間」まで。超えると `onTimeout()`、数秒で `stopSelf()` しないと例外。アプリを前に出すとタイマーは戻る（fgs/timeout）
- targetSdk 35 以上では、BOOT_COMPLETED の受け手から dataSync・camera・mediaPlayback・phoneCall・mediaProjection・microphone を始められない（`ForegroundServiceStartNotAllowedException`）。specialUse はこの一覧に無い（behavior-changes-15）
- 裏から前面サービスを始めてよい例外の一覧に、JobScheduler／JobService／WorkManager は**無い**。電池の最適化を利用者が切った場合・通知などの操作・BOOT_COMPLETED などはある（fgs/restrictions-bg-start）
- specialUse は他の型に当てはまらない用途向け。`PROPERTY_SPECIAL_USE_FGS_SUBTYPE` に用途を書き、Play Console の審査で見られる。権限は `FOREGROUND_SERVICE_SPECIAL_USE`（fgs/service-types）
- **確かめられなかった**: Play Console ヘルプの前面サービスの申告（この環境から読めない）。specialUse が審査で通るかは原文を読むまで分からない

## 今のコードとの食い違い
1. docs/android.md の「既定では充電中だけ」は実物と違う。既定は「電池 30% まで動き、下回ったら止まる」（`PREF_CHARGE_ONLY` の意味が変わった）
2. 充電で再開する `ResumeJobService` は「動いている job からは前面サービスを始めてよい」を前提にしているが、上の例外の一覧に無い。始められないと例外は握りつぶされ、黙って再開しない（実機・エミュレータでは未測定）
3. dataSync のままでは一晩の充電（8時間）の途中で6時間の上限に当たる。BOOT からの起動もできなくなる

## 比べた案
- A. **specialUse の前面サービス＋既定は充電中だけ**: 時間の上限なし、BOOT から起動可。審査で落ちる危険がある
- B. dataSync のまま: 6時間で止まる。群れのノードが毎晩途中で抜ける
- C. 充電中の JobScheduler の job だけで動く: job には実行時間の上限があり、ノードが細切れに出入りする（SWIM には悪い）
- D. アプリを開いている間だけ: 決まりの心配は無いが、参加がほぼ無い

## おすすめ（これで作る）
**A。** 既定を「充電中だけ」に戻し（docs の約束に合わせる）、充電が外れたら止まる。再開は「job から始めてみる → 断られたら『充電中です。参加を再開しますか』の通知を出し、利用者が押したら始める」（通知の操作は例外の一覧にある）。審査で specialUse が断られたら B に落とし、6時間で止まることを画面と説明文に正直に書く。

## どう確かめるか
- API 36 のエミュレータで: 充電の抜き差し（`adb shell dumpsys battery set ac`）で止まる・再開する（通知の経路も）、BOOT からの起動、8時間を超えても止まらない
- 陰性コントロール: dataSync のままのビルドで BOOT からの起動が例外になること、6時間で `onTimeout` が来ること（時間は `adb shell cmd activity` の設定で縮められるか、別に確かめる）
