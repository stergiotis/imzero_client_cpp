#include "src/core/SkTraceEvent.h"
#include "skiaTracyTracer.h"

#include <cinttypes>
#include "tracy/Tracy.hpp"

SkEventTracer::Handle skiaTracyTracer::addTraceEvent(char phase,
                                                    const uint8_t* categoryEnabledFlag,
                                                    const char* name,
                                                    uint64_t id,
                                                    int numArgs,
                                                    const char** argNames,
                                                    const uint8_t* argTypes,
                                                    const uint64_t* argValues,
                                                    uint8_t flags) { ZoneScoped;
    SkString args;
    for (int i = 0; i < numArgs; ++i) {
        if (i > 0) {
            args.append(", ");
        } else {
            args.append(" ");
        }

        uint64_t value = argValues[i];
        switch (argTypes[i]) {
            case TRACE_VALUE_TYPE_BOOL:
                args.appendf("%s=%s", argNames[i], value ? "true" : "false");
                break;
            case TRACE_VALUE_TYPE_UINT:
                args.appendf("%s=%" PRIu64, argNames[i], value);
                break;
            case TRACE_VALUE_TYPE_INT:
                args.appendf("%s=%" PRIi64, argNames[i], static_cast<int64_t>(value));
                break;
            case TRACE_VALUE_TYPE_DOUBLE:
                args.appendf("%s=%g", argNames[i], sk_bit_cast<double>(value));
                break;
            case TRACE_VALUE_TYPE_POINTER:
                args.appendf("%s=0x%p", argNames[i], skia_private::TraceValueAsPointer(value));
                break;
            case TRACE_VALUE_TYPE_STRING:
            case TRACE_VALUE_TYPE_COPY_STRING: {
                static constexpr size_t kMaxLen = 20;
                SkString string(skia_private::TraceValueAsString(value));
                size_t truncAt = string.size();
                size_t newLineAt = SkStrFind(string.c_str(), "\n");
                if (newLineAt > 0) {
                    truncAt = newLineAt;
                }
                truncAt = std::min(truncAt, kMaxLen);
                if (truncAt < string.size()) {
                    string.resize(truncAt);
                    string.append("...");
                }
                args.appendf("%s=\"%s\"", argNames[i], string.c_str());
                break;
            }
            default:
                args.appendf("%s=<unknown type>", argNames[i]);
                break;
        }
    }
    bool open = (phase == TRACE_EVENT_PHASE_COMPLETE);
    if (open) {
        const char* category = this->getCategoryGroupName(categoryEnabledFlag);
        char buf[4096];
        snprintf(buf,4096,"[% 2d]%s <%s> %s%s #%d {\n", (int)fIndent.size(), fIndent.c_str(), category, name,
                 args.c_str(), fCnt);
        TracyMessage(buf,sizeof(buf));
        //SkDebugf("[% 2d]%s <%s> %s%s #%d {\n", (int)fIndent.size(), fIndent.c_str(), category, name,
        //         args.c_str(), fCnt);
        fIndent.append(" ");
    } else {
        TracyMessage(args.c_str(),args.size());
        //SkDebugf("%s%s #%d\n", name, args.c_str(), fCnt);
        fprintf(stderr,"%s\n",args.c_str());
    }
    fprintf(stderr,"trace!!!\n");
    fflush(stderr);
    ++fCnt;
    return 0;
}

void skiaTracyTracer::updateTraceEventDuration(const uint8_t* categoryEnabledFlag,
                                              const char* name,
                                              SkEventTracer::Handle handle) {
    fIndent.resize(fIndent.size() - 1);
    SkDebugf("[% 2d]%s } %s\n", (int)fIndent.size(), fIndent.c_str(), name);
}

void skiaTracyTracer::newTracingSection(const char* name) {
    SkDebugf("\n\n- - - New tracing section: %s - - -\n", name);
    fprintf(stderr,"new section %s\n",name);
}
