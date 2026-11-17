#include "TireSmokeEffect.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

#include "../../../Manager/Shader/ShaderResourceManager.h"
#include "../../../Manager/TimeScale/TimeScale.h"

TireSmokeEffect::TireSmokeEffect() :
    random(std::random_device{}()),
    frameVelocity(), previousEmitter(), hasPreviousEmitter(false),
    strength(0.0f), smokeSize(18.0f), emissionRemainder(0.0f), smokeImage(-1)
{
    particles.reserve(MAX_PARTICLES);
    drawOrder.reserve(MAX_PARTICLES);
}

void TireSmokeEffect::Load(void)
{
    if (smokeImage >= 0) { return; }

    auto& shaders = ShaderResourceManager::GetIns();
    shaders.CreatePixelShader(PIXEL_SHADER_TYPE::TireSmoke);
    const int shader = shaders.GetPixelShader(PIXEL_SHADER_TYPE::TireSmoke);
    if (shader < 0) { throw std::runtime_error("白煙シェーダーの読み込みに失敗しました（TireSmokePS.cso）"); }

    constexpr int IMAGE_SIZE = 128;
    smokeImage = MakeScreen(IMAGE_SIZE, IMAGE_SIZE, true);
    if (smokeImage < 0) { throw std::runtime_error("白煙画像の作成に失敗しました"); }

    const int target = GetDrawScreen();
    int blend = 0, alpha = 0;
    GetDrawBlendMode(&blend, &alpha);

    SetDrawScreen(smokeImage);
    ClearDrawScreen();
    // アルファ値の二重乗算を防ぐため、色とアルファ値をそのまま画像に保存する。
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    SetUseVertexShader(-1);
    SetUsePixelShader(shader);

    VERTEX2DSHADER vertices[4]{};
    for (int i = 0; i < 4; ++i) {
        vertices[i].u = static_cast<float>(i % 2);
        vertices[i].v = static_cast<float>(i / 2);
        vertices[i].pos = VGet(vertices[i].u * IMAGE_SIZE, vertices[i].v * IMAGE_SIZE, 0.0f);
        vertices[i].rhw = 1.0f;
        vertices[i].dif = GetColorU8(255, 255, 255, 255);
    }
    const unsigned short indices[6] = { 0, 1, 2, 1, 3, 2 };
    const int result = DrawPolygonIndexed2DToShader(vertices, 4, indices, 2);

    SetUsePixelShader(-1);
    SetDrawScreen(target);
    SetDrawBlendMode(blend, alpha);
    if (result < 0) {
        DeleteGraph(smokeImage);
        smokeImage = -1;
        throw std::runtime_error("白煙画像の描画に失敗しました");
    }
}

void TireSmokeEffect::SetEmitter(const Vector3& position, const Vector3& movement)
{
    trans.pos = position;
    frameVelocity = movement;
}

void TireSmokeEffect::SetSlipState(bool slipping, bool drifting, float intensity)
{
    const float amount = std::isfinite(intensity) ? std::clamp(intensity, 0.0f, 1.0f) : 0.0f;
    strength = (slipping || drifting) ? amount * (drifting ? 1.0f : 0.65f) : 0.0f;
    if (strength == 0.0f) {
        emissionRemainder = 0.0f;
        hasPreviousEmitter = false;
    }
}

void TireSmokeEffect::SetSmokeSize(float size)
{
    if (std::isfinite(size) && size > 0.0f) { smokeSize = size; }
}

void TireSmokeEffect::Clear(void)
{
    particles.clear();
    drawOrder.clear();
    emissionRemainder = 0.0f;
    hasPreviousEmitter = false;
}

void TireSmokeEffect::SubInit(void)
{
    Clear();
}

float TireSmokeEffect::Random(float low, float high)
{
    return std::uniform_real_distribution<float>(low, high)(random);
}

void TireSmokeEffect::Emit(const Vector3& position)
{
    const float size = smokeSize * Random(0.8f, 1.2f);
    Particle particle{};
    particle.position = position + Vector3(Random(-0.12f, 0.12f) * size, size * 0.22f,
                                          Random(-0.12f, 0.12f) * size);
    // 煙はワールド座標に残し、車両の移動量は一部だけ引き継ぐ。
    particle.velocity = frameVelocity * 60.0f * 0.08f
        + Vector3(Random(-0.35f, 0.35f) * size, Random(0.5f, 1.0f) * size,
                  Random(-0.35f, 0.35f) * size);
    particle.lifetime = Random(0.75f, 1.35f);
    particle.size = size;
    particle.angle = Random(-3.14159265f, 3.14159265f);
    particle.spin = Random(-0.6f, 0.6f);
    particle.opacity = Random(0.35f, 0.6f) * strength;
    particles.push_back(particle);
}

void TireSmokeEffect::SubUpdate(void)
{
    const float dt = (std::max)(0.0f, TimeScale::Get()) / 60.0f;
    if (dt <= 0.0f) { return; }

    for (auto& particle : particles) {
        particle.age += dt;
        particle.position += particle.velocity * dt;
        particle.velocity *= std::exp(-1.4f * dt);
        particle.angle += particle.spin * dt;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.age >= p.lifetime; }), particles.end());

    if (strength > 0.0f) {
        if (!hasPreviousEmitter) { previousEmitter = trans.pos; }
        const float count = emissionRemainder + 75.0f * strength * dt;
        // 一度の発生数に上限を設け、タイヤの移動経路上を補間して煙の偏りを防ぐ。
        const float boundedCount = (std::min)(count, static_cast<float>(MAX_PARTICLES));
        const int emitCount = static_cast<int>(boundedCount);
        emissionRemainder = boundedCount - emitCount;
        for (int i = 0; i < emitCount && particles.size() < MAX_PARTICLES; ++i) {
            const float t = static_cast<float>(i + 1) / (emitCount + 1);
            Emit(previousEmitter + (trans.pos - previousEmitter) * t);
        }
        hasPreviousEmitter = true;
    }
    previousEmitter = trans.pos;
}

void TireSmokeEffect::SubDraw(void)
{
    if (smokeImage < 0 || particles.empty()) { return; }

    const Vector3 camera(GetCameraPosition());
    drawOrder.resize(particles.size());
    std::iota(drawOrder.begin(), drawOrder.end(), std::size_t{0});
    std::sort(drawOrder.begin(), drawOrder.end(), [&](std::size_t a, std::size_t b) {
        return (particles[a].position - camera).LengthSq()
             > (particles[b].position - camera).LengthSq();
    });

    int blend = 0, alpha = 0;
    GetDrawBlendMode(&blend, &alpha);
    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(false);
    for (const auto index : drawOrder) {
        const auto& p = particles[index];
        const float life = std::clamp(p.age / p.lifetime, 0.0f, 1.0f);
        const float fadeIn = (std::min)(life / 0.1f, 1.0f);
        const float opacity = p.opacity * fadeIn * (1.0f - life) * (1.0f - life);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * opacity));
        DrawBillboard3D(p.position.ToVECTOR(), 0.5f, 0.5f,
                        p.size * (1.0f + 2.4f * life), p.angle, smokeImage, true);
    }
    // SceneManagerの通常の3D描画設定に合わせて、深度書き込みを有効に戻す。
    SetWriteZBuffer3D(true);
    SetDrawBlendMode(blend, alpha);
}

void TireSmokeEffect::SubRelease(void)
{
    Clear();
    strength = 0.0f;
    if (smokeImage >= 0) {
        DeleteGraph(smokeImage);
        smokeImage = -1;
    }
    // 共有シェーダーの所有権はShaderResourceManagerにあるため、ここでは解放しない。
}
