#pragma once
#include "include/utils/SkEventTracer.h"
#include "tools/trace/EventTracingPriv.h"
#include "include/core/SkString.h"

class skiaTracyTracer : public SkEventTracer {
public:
    skiaTracyTracer() = default;

    skiaTracyTracer::Handle addTraceEvent(char phase,
                                        const uint8_t* categoryEnabledFlag,
                                        const char* name,
                                        uint64_t id,
                                        int numArgs,
                                        const char** argNames,
                                        const uint8_t* argTypes,
                                        const uint64_t* argValues,
                                        uint8_t flags) override;

    void updateTraceEventDuration(const uint8_t* categoryEnabledFlag,
                                  const char* name,
                                  skiaTracyTracer::Handle handle) override;

    const uint8_t* getCategoryGroupEnabled(const char* name) override {
        static uint8_t yes = SkEventTracer::kEnabledForRecording_CategoryGroupEnabledFlags;
        return &yes;
    }

    const char* getCategoryGroupName(const uint8_t* categoryEnabledFlag) override {
        static const char* category = "skiaTracyTracer";
        return category;
    }

    void newTracingSection(const char* name) override;

private:
    SkString fIndent;
    int fCnt = 0;
};
