#pragma once

#include "../ActorBase/ActorBase.h"

/// <summary>ワールド上のエフェクトで共通利用する、Actorのライフサイクルと半透明描画の基底クラス。</summary>
class EffectBase : public ActorBase
{
public:
    /// <summary>物理挙動を無効にし、シーンの半透明描画で描くエフェクトを生成する。</summary>
    EffectBase()
    {
        SetDynamicFlg(false);
        SetGravityFlg(false);
        SetPushFlg(false);
        SetDrawType(ACTOR_DRAW_TYPE::Alpha);
    }

    virtual ~EffectBase() = default;
};
