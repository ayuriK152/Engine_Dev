#include "Common.hlsl"

struct KeyFrame
{
    float tick;
    float3 pos;
    float4 rot;
    float3 scl;
};

struct BoneHeader {
    uint keyOffset;
    uint keyCount;
    int parentIndex;
    uint boneIndex;
};

StructuredBuffer<KeyFrame> KeyFrames;
StructuredBuffer<BoneHeader> BoneHeaders;

uint FindKeyframe(uint offset, uint count, float tick)
{
    if (count < 2) return 0;

    uint left = 0;
    uint right = count - 2;

    while (left <= right) {
        uint mid = (left + right) / 2;
        float t0 = KeyFrames[offset + mid].tick;
        float t1 = KeyFrames[offset + mid + 1].tick;

        if (tick >= t0 && tick <= t1) return mid;
        else if (tick < t0) right = mid - 1;
        else left = mid + 1;
    }
    return count - 1;
}

KeyFrame InterpolateBone(uint bone, float tick)
{
    BoneHeader h = BoneHeaders[bone];
    uint idx = FindKeyframe(h.keyOffset, h.keyCount, tick);

    KeyFrame k0 = KeyFrames[h.keyOffset + idx];
    KeyFrame k1 = KeyFrames[h.keyOffset + idx + 1];

    float alpha = (tick - k0.tick) / (k1.tick - k0.tick);

    KeyFrame kout;
    kout.pos = lerp(k0.pos, k1.pos, alpha);
    //kout.rot = QuaternionSlerp(k0.rot, k1.rot, alpha);
    kout.scl = lerp(k0.scl, k1.scl, alpha);
    return kout;
}

[numthreads(256, 1, 1)]
void CS(int3 DTid : SV_DispatchThreadID)
{

}