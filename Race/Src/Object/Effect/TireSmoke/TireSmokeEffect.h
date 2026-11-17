#pragma once

#include <cstddef>
#include <random>
#include <vector>

#include "../EffectBase.h"

/// <summary>タイヤ1本分の白煙を発生させる、シーンまたは親Actorが所有するエフェクト。</summary>
class TireSmokeEffect : public EffectBase
{
public:
    /// <summary>発生停止状態で生成する。距離はワールド座標の単位を使用する。</summary>
    TireSmokeEffect();

    /// <summary>ShaderResourceManagerで管理するシェーダーから白煙画像を生成する。</summary>
    void Load(void) override;

    /// <summary>Updateの前にタイヤの接地点と車両の移動量を更新する。</summary>
    /// <param name="position">ワールド座標のタイヤ接地点。</param>
    /// <param name="frameVelocity">通常の60Hzフレームあたりの車両移動量。</param>
    void SetEmitter(const Vector3& position, const Vector3& frameVelocity);

    /// <summary>発生状態を設定する。停止後も既存の煙は自然に消滅する。</summary>
    /// <param name="slipping">このタイヤがスリップ中かどうか。</param>
    /// <param name="drifting">車両がドリフト中かどうか。</param>
    /// <param name="intensity">0から1の発生強度。ドリフト中は煙を濃くする。</param>
    void SetSlipState(bool slipping, bool drifting, float intensity = 1.0f);

    /// <summary>ワープやリスポーン時などに、残っている煙を即座に消去する。</summary>
    void Clear(void);

    /// <summary>白煙の初期直径をワールド座標の単位で設定する。初期値は18。</summary>
    void SetSmokeSize(float size);

    /// <summary>現在の白煙粒子数を取得する。</summary>
    std::size_t GetParticleCount(void) const { return particles.size(); }

protected:
    void SubInit(void) override;
    void SubUpdate(void) override;
    void SubDraw(void) override;
    void SubRelease(void) override;

private:
    struct Particle
    {
        Vector3 position;
        Vector3 velocity;
        float age;
        float lifetime;
        float size;
        float angle;
        float spin;
        float opacity;
    };

    static constexpr std::size_t MAX_PARTICLES = 256;
    std::vector<Particle> particles;
    std::vector<std::size_t> drawOrder;
    std::mt19937 random;
    Vector3 frameVelocity;
    Vector3 previousEmitter;
    bool hasPreviousEmitter;
    float strength;
    float smokeSize;
    float emissionRemainder;
    int smokeImage;

    float Random(float low, float high);
    void Emit(const Vector3& position);
};
