#ifndef GENERATION_DATA_H
#define GENERATION_DATA_H

struct EntityGenerationData
{
public:
    int EntityCount, LegPairCountMin, LegPairCountMax;
    float MoveSpeedMin, MoveSpeedMax;
};

struct SpiderGenerationData : public EntityGenerationData
{
public:
    float UpperLegSizeScaleMin, MiddleLegSizeScaleMin, LowerLegSizeScaleMin,
        UpperLegSizeScaleMax, MiddleLegSizeScaleMax, LowerLegSizeScaleMax;
};

struct CaterpillarGenerationData : public EntityGenerationData
{
};

struct ProducerGenerationData : public EntityGenerationData
{
};
#endif