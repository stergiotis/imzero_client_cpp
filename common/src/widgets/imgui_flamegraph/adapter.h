typedef struct flameGraphData {
	float *starts;
	float *ends;
	ImU8 *levels;
	const char **captions;
} flameGraphData;

static void flameGraphValueGetter(float* start, float* end, ImU8* level, const char** caption, const void* data, int idx) {
   auto d = (flameGraphData*)data;
   if(start != nullptr) {
      *start = d->starts[idx];
   }
   if(end != nullptr) {
      *end = d->ends[idx];
   }
   if(level != nullptr) {
      *level = d->levels[idx];
   }
   if(caption != nullptr) {
      *caption = d->captions[idx];
   }
}
