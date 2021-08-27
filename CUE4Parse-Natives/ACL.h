#pragma once
#include "ACLDecompress.h"

// acl allocator
DLLEXPORT void* nAllocate(size_t size, size_t alignment) { return ACLAllocatorImpl.allocate(size, alignment); }
DLLEXPORT void nDeallocate(void* ptr, size_t size) { ACLAllocatorImpl.deallocate(ptr, size); }

// util
DLLEXPORT const char* nCompressedTracks_IsValid(acl::compressed_tracks* tracks, bool checkHash) { return tracks->is_valid(checkHash).c_str(); }

// acl decompress
DLLEXPORT DecompContextDefault* nDecompContextDefault_Create() { return new DecompContextDefault(); }
DLLEXPORT const acl::compressed_tracks* nDecompContextDefault_GetCompressedTracks(DecompContextDefault* context) { return context->get_compressed_tracks(); }
DLLEXPORT bool nDecompContextDefault_Initialize(DecompContextDefault* context, acl::compressed_tracks& tracks) { return context->initialize(tracks); }
DLLEXPORT void nDecompContextDefault_Seek(DecompContextDefault* context, int64_t sampleTime, acl::sample_rounding_policy roundingPolicy) { context->seek(sampleTime, roundingPolicy); }
DLLEXPORT FUE4OutputWriter* nCreateOutputWriter(FTransform* atoms, const FAtomIndices* trackToAtomsMap) { return new FUE4OutputWriter(atoms, trackToAtomsMap); }
DLLEXPORT UE4OutputTrackWriter* nCreateOutputTrackWriter(FTransform& atom) { return new UE4OutputTrackWriter(atom); }
DLLEXPORT void nDecompContextDefault_DecompressTracks(DecompContextDefault* context, FUE4OutputWriter& writer) { context->decompress_tracks(writer); }
DLLEXPORT void nDecompContextDefault_DecompressTrack(DecompContextDefault* context, uint32_t trackIndex, UE4OutputTrackWriter& writer) { context->decompress_track(trackIndex, writer); }

// acl convert
DLLEXPORT const char* nConvertTrackList(const acl::compressed_tracks& tracks, acl::track_array_qvvf& outTracks) { return acl::convert_track_list(ACLAllocatorImpl, tracks, outTracks).c_str(); }
DLLEXPORT const char* nConvertACLTrack(const acl::track_array_qvvf& tracks, uint32_t trackIndex, FVector* outPosKeys, FQuat* outRotKeys, FVector* outScaleKeys)
{
    const uint32_t numSamples = tracks.get_num_samples_per_track();
    if (numSamples == 0)
    {
        return "Clip has no samples";
    }

    if (!outPosKeys) outPosKeys = (FVector*)malloc(numSamples * sizeof(FVector));
    if (!outRotKeys) outRotKeys = (FQuat*)malloc(numSamples * sizeof(FQuat));
    if (!outScaleKeys) outScaleKeys = (FVector*)malloc(numSamples * sizeof(FVector));
    const uint32_t numTracks = tracks.get_num_tracks();
    for (uint32_t trackIndex = 0; trackIndex < numTracks; ++trackIndex)
    {
        const acl::track_qvvf& track = tracks[trackIndex];

        for (uint32_t sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        {
            const FQuat rotation = QuatCast(rtm::quat_normalize(track[sampleIndex].rotation));
            outRotKeys[sampleIndex] = rotation;
        }

        for (uint32_t sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        {
            const FVector translation = VectorCast(track[sampleIndex].translation);
            outPosKeys[sampleIndex] = translation;
        }

        for (uint32_t sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        {
            const FVector scale = VectorCast(track[sampleIndex].scale);
            outScaleKeys[sampleIndex] = scale;
        }
    }
}