# タイヤの白煙Actor

`TireSmokeEffect` は `EffectBase : ActorBase` を継承しています。
1つのインスタンスがタイヤ1本分の煙を管理します。車の走行処理には接続していません。
`SceneBase::AddActor` または親Actorの `AddChildActor` に渡して使います。
両方に同じポインタを登録しないでください。

親Actorのメンバーに `TireSmokeEffect* tireSmoke = nullptr;` を持たせる例：

```cpp
#include "Object/Effect/TireSmoke/TireSmokeEffect.h" // include元に応じて調整

// 親Actor::Load 内（初期化前に登録）
tireSmoke = new TireSmokeEffect();
AddChildActor(tireSmoke);

// 親Actor::SubUpdate 内（毎フレーム、子Actorが更新される前）
tireSmoke->SetEmitter(tireContactWorldPosition, displacementPerNormalFrame);
tireSmoke->SetSlipState(isSlipping, isDrifting, 1.0f);
```

- `SetEmitter` はワールド座標の接地点と、通常の60Hzフレームあたりの移動量を受け取ります。
  秒あたりの速度を渡す場合は60で割ります。タイムスケールを二重に掛けないでください。
- 親の移動後の接地点を渡してください。親子関係による座標変換は自動では行いません。
- 左右の後輪に出す場合はインスタンスを2つ作ります。
- `SetSlipState(false, false)` で発生停止。既存の煙は浮き上がり、拡大して消えます。
- `SetSmokeSize(18.0f)` で初期直径を調整できます。車両のワールドスケールに合わせてください。
- ワープ／リスポーン時は `Clear()` で残った煙と補間用の履歴を消してください。
- `TimeScale::Get()` に追従し、停止中は発生・移動・寿命の進行も止まります。
- シーンのAlphaパスで描画され、煙同士はインスタンス内で奥から手前に描きます。
  別Actorの半透明物との描画順は既存のシーンの登録順に従います。
- 親またはシーンが `Release` と削除を行うため、登録後に自前で削除する必要はありません。

Shaderは `Src/Shader/TireSmoke/TireSmokePS.hlsl` に置いています。
Visual Studioのx64ビルドで `Data/Shader/TireSmokePS.cso` が生成され、
`ShaderResourceManager` 経由で読み込みます。
Load時にノイズ付きの白煙画像を生成するため、追加のテクスチャ素材は不要です。
シーン内での `Load`／`AddActor` は通常描画の外で実行してください。
